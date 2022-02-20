#include "Backend.h"

#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


Font gfont;
Font* font = &gfont;

int LoadTTFFont(Font* __font, const char* filename, float size) {
	static const uint32_t BITMAP_WIDTH = 512;
	static const uint32_t BITMAP_HEIGHT = 512;
	static const uint8_t  FONT_BEGIN = ' ';
	static const uint8_t  FONT_END = '~';
	static const uint8_t  FONT_LENGTH = FONT_END - FONT_BEGIN;
	
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		return 1;
	}
	
	fseek(file, 0, SEEK_END);
	const uint32_t fileSize = ftell(file); 
	fseek(file, 0, SEEK_SET);
	
	uint8_t* ttf_raw_data = new uint8_t[fileSize];
	if (fread(ttf_raw_data, 1, fileSize, file) != fileSize) {
		fclose(file);
		return 1;
	}
	fclose(file);
	
	font->bitmap = new uint8_t[BITMAP_WIDTH * BITMAP_HEIGHT];
	font->glyphs = new stbtt_bakedchar[FONT_LENGTH];
	font->begin = FONT_BEGIN;
	font->end = FONT_END;
	font->size = size;
	stbtt_BakeFontBitmap(ttf_raw_data, 0, size, font->bitmap, BITMAP_WIDTH, BITMAP_HEIGHT, FONT_BEGIN, FONT_LENGTH, font->glyphs);
	delete [] ttf_raw_data;
	
	return 0;
}

void MyDrawLine(GUIContext* context, const ivec2& begin, const ivec2& end, const color& color) {
	if (begin.x == end.x) {
		MyDrawQuad(context, {begin.x, begin.y, end.x + 1, end.y}, color);
	} else if (begin.y == end.y){
		MyDrawQuad(context, {begin.x, begin.y, end.x, end.y + 1}, color);
	}
}

void MyDrawQuad(GUIContext* context, const ivec4& bounds, const color& color) {
	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;
	
	const int32_t begin_x = std::max<int32_t>(bounds.x, 0);
	const int32_t begin_y = std::max<int32_t>(bounds.y, 0);
	const int32_t end_x   = std::max(std::min<int32_t>(bounds.z, 1024), 0);
	const int32_t end_y   = std::max(std::min<int32_t>(bounds.w,  720), 0);

	uint32_t* pixels = (uint32_t*)context->opaqueData;
	
	for (uint32_t y = begin_y; y < end_y; ++y) {
		for (uint32_t x = begin_x; x < end_x; ++x) {
			pixels[y * 1024 + x] = pixel;
		}
	}
}

ivec2 MyGetTextSize(GUIContext* guiContext, const char* text, uint32_t length) {
	static const uint8_t FONT_BEGIN = 32;
	static const uint8_t FONT_END = 128;
	float x = 0.0f;
	float y = 16.0f;
	for (uint32_t index = 0; index < length; ++index) {
		if (text[index] >= FONT_BEGIN && text[index] < FONT_END) {
			const stbtt_bakedchar *b = font->glyphs + text[index] - FONT_BEGIN;
			x += b->xadvance;
		}
	}
	return {(int32_t)x, (int32_t)y};
}

void MyDrawChar(GUIContext* context, char c, float& pos_x, float& pos_y, const color& color) {
	const int32_t SCREEN_WIDTH = 1024;
	const int32_t SCREEN_HEIGHT = 720;
	const int32_t FONT_BITMAP_WIDTH = 512;
	const int32_t FONT_BITMAP_HEIGHT = 512;

	if (c < 32 && c >= 128) {
		return;
	}

	const stbtt_bakedchar *b = font->glyphs + c - 32;
	uint32_t* pixels = (uint32_t*)context->opaqueData;
	
	const uint32_t width  = b->x1 - b->x0;
	const uint32_t height = b->y1 - b->y0;
	
	for (int32_t y = 0; y < height; ++y) {
		for (int32_t x = 0; x < width; ++x) {
			const uint32_t sourceIndex = (b->y0 + y) * FONT_BITMAP_WIDTH + b->x0 + x;
			const int32_t px = pos_x + x + (b->xoff + 0.5f);
			const int32_t py = pos_y + y + (b->yoff + 0.5f);
			
			if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
				const uint32_t destIndex = py * SCREEN_WIDTH + px;
				const uint8_t luminance = font->bitmap[sourceIndex];
				
				const uint32_t mask  = luminance << 24 | luminance << 16 | luminance <<  8 | luminance <<  0;
				const uint32_t pixel = color.w   << 24 | color.x   << 16 | color.y   <<  8 | color.z   <<  0;
				
				pixels[destIndex] = (pixels[destIndex] & ~mask) | (pixel & mask);
			}
		}
	}
	pos_x += b->xadvance;
}

void MyDrawText(GUIContext* context, const char* text, const ivec4& bounds, const color& color, uint32_t flags) {
	const ivec2 midBounds = {(bounds.z + bounds.x) / 2, (bounds.y + bounds.w) / 2};
	const ivec2 textSize = context->getTextSize(context, text, strlen(text));
	const ivec2 halfTextSize = {textSize.x / 2, textSize.y / 2};
	const ivec2 halfBounds = {(bounds.z - bounds.x) / 2, (bounds.w - bounds.y) / 2};
	
	const float start_x = ((flags & GUI_ALIGN_CENTER) == GUI_ALIGN_CENTER) ? midBounds.x - std::min(halfTextSize.x, halfBounds.x) : bounds.x;
	const float start_y = ((flags & GUI_ALIGN_CENTER) == GUI_ALIGN_CENTER) ? midBounds.y - std::min(halfTextSize.y, halfBounds.y) : bounds.y;
	float pos_x = start_x;
	float pos_y = start_y;
	int charWidth = 0;
	
	pos_y += 13; // WTF ?!
	while (*text) {
		switch (*text) {
		case '\n' :
			pos_x = start_x;
			pos_y += 16.0f;
			break;
		default :
			charWidth = MyGetTextSize(context, text, 1).x;
			if ((pos_x + charWidth <= bounds.z && pos_y <= bounds.w)) { // seems to draw 1 more char 
				MyDrawChar(context, *text, pos_x, pos_y, color);
			}
			break;
		}
		++text;
	}
}

void MyDrawIcon(GUIContext* context, int32_t id, const ivec4& bounds, const color& color) {
	static const uint32_t ICON_LENGTH = 15;
	static const uint32_t ICON_HALF_LENGTH = ICON_LENGTH / 2;
	static const uint32_t ICON_SIZE = ICON_LENGTH * ICON_LENGTH;
	static const uint8_t ICONS[] = {
			// GUI_ICON_CLOSE
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,
			0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,
			0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,
			0,0,0,0,1,1,1,0,1,1,1,0,0,0,0,
			0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
			0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
			0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
			0,0,0,0,1,1,1,0,1,1,1,0,0,0,0,
			0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,
			0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,
			0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

			// GUI_ICON_ARROW_LEFT
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
			0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
			0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
			0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			
			// GUI_ICON_ARROW_RIGHT
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
			0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
			0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
			0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			
			// GUI_ICON_ARROW_DOWN
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
			0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,
			0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,
			0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
			0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,
			0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
			0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			
			// GUI_ICON_ARROW_UP
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
			0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
			0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,
			0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
			0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,
			0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,
			0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			
			// GUI_ICON_SIZE
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,
			0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
			0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			
			// GUI_ICON_CHECK
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,
			0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,
			0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,
			0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,
			0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,
			0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,
			0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
			0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;
	const int32_t l = std::min(bounds.z - bounds.x, bounds.w - bounds.y);
	const ivec2 boundsMiddle = {(bounds.z - bounds.x) / 2, (bounds.w - bounds.y) / 2};
	
	uint32_t* pixels = (uint32_t*)context->opaqueData;
	
	if (l < ICON_LENGTH) {
		return;
	}
	
	if (id < 0 || id > 6) {
		return;
	}
	
	for (int32_t y = 0; y < ICON_LENGTH; ++y) {
		for (int32_t x = 0; x < ICON_LENGTH; ++x) {
			const uint8_t value = ICONS[id * ICON_SIZE + y * ICON_LENGTH + x];
			const int32_t py = bounds.y + boundsMiddle.y + y - ICON_HALF_LENGTH;
			const int32_t px = bounds.x + boundsMiddle.x + x - ICON_HALF_LENGTH;
			const uint32_t dstIndex = py * 1024 + px;
			if (py >= 0 && py < 720 && px >= 0 && px < 1024) {
				pixels[dstIndex] = pixels[dstIndex] * !value + pixel * value;
			}
		}
	}
}