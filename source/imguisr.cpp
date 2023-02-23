#include "imguisr.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
#include <algorithm>

const uint32_t BITMAP_WIDTH = 512;
const uint32_t BITMAP_HEIGHT = 512;
const uint8_t  FONT_BEGIN = ' ';
const uint8_t  FONT_END = '~';
const uint8_t  FONT_LENGTH = FONT_END - FONT_BEGIN;

STBFont* pfont = NULL; // TODO: Clean this junk

void MyDrawLine(GUIContext* context, const ivec2& begin, const ivec2& end, const color_t& color);
void MyDrawQuad(GUIContext* context, const ivec4& bounds, const color_t& color);
void MyDrawChar(GUIContext* context, char c, float x, float y, const color_t& color);
void MyDrawIcon(GUIContext* context, int32_t , const ivec4& bounds, const color_t& color);
ivec2 MyGetCharSize(GUIContext* guiContext, char c);

int LoadTTFFont(STBFont* font, const char* filename, float size);
void FreeTTFFont(STBFont* font);

int LoadTTFFont(STBFont* font, const char* filename, float size) {
	char fullFilename[64];
	snprintf(fullFilename, 64, "%s", filename);

	FILE* file = fopen(fullFilename, "rb");
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

	font->width = BITMAP_WIDTH;
	font->height = BITMAP_HEIGHT;
	font->bitmap = new uint8_t[BITMAP_WIDTH * BITMAP_HEIGHT];
	font->glyphs = new stbtt_bakedchar[FONT_LENGTH];
	font->begin = FONT_BEGIN;
	font->end = FONT_END;
	font->size = size;
	stbtt_BakeFontBitmap(ttf_raw_data, 0, size, font->bitmap, BITMAP_WIDTH, BITMAP_HEIGHT, FONT_BEGIN, FONT_LENGTH, font->glyphs);
	delete [] ttf_raw_data;

	pfont = font;

	return 0;
}

void FreeTTFFont(STBFont* font) {
	delete [] font->bitmap;
	delete [] font->glyphs;
}

int SoftGUIInit(SoftGUI* gui, uint32_t width, uint32_t height, void* pixelBuffer, const char* fontFilename, float fontSize) {
	GUIContextInit(&gui->context);
	gui->context.drawLine = MyDrawLine;
	gui->context.drawQuad = MyDrawQuad;
	gui->context.drawChar = MyDrawChar;
	gui->context.drawIcon = MyDrawIcon;
	gui->context.getCharSize = MyGetCharSize;

	gui->context.opaqueData = pixelBuffer;
	gui->context.viewport = {0, 0, (int32_t)width, (int32_t)height};
	gui->context.extents.x = width;
	gui->context.extents.y = height;

	if (LoadTTFFont(&gui->font, fontFilename, fontSize) != 0) {
		printf("Failed to load font '%s'.\n", fontFilename);
		return 1;
	}
	GUISetActiveContext(&gui->context);
	return 0;
}
void SoftGUIUninit(SoftGUI* gui) {
	FreeTTFFont(&gui->font);
}

void MyDrawLine(GUIContext* context, const ivec2& begin, const ivec2& end, const color_t& color) {
	if (begin.x == end.x) {
		MyDrawQuad(context, {begin.x, begin.y, end.x + 1, end.y}, color);
	} else if (begin.y == end.y){
		MyDrawQuad(context, {begin.x, begin.y, end.x, end.y + 1}, color);
	} else {
		assert(!"Diagonal lines are not implemented");
	}
}

void MyDrawQuad(GUIContext* context, const ivec4& bounds, const color_t& color) {
	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;

	const int32_t begin_x = std::max<int32_t>(bounds.x, 0);
	const int32_t begin_y = std::max<int32_t>(bounds.y, 0);
	const int32_t end_x   = std::max(std::min<int32_t>(bounds.z, context->extents.x), 0);
	const int32_t end_y   = std::max(std::min<int32_t>(bounds.w,  context->extents.y), 0);

	uint32_t* pixels = (uint32_t*)context->opaqueData;

	for (int32_t y = begin_y; y < end_y; ++y) {
		for (int32_t x = begin_x; x < end_x; ++x) {
			pixels[y * context->extents.x + x] = pixel;
		}
	}
}

ivec2 MyGetCharSize(GUIContext* guiContext, char c) {
	ivec2 ans = {0, 0};

	if (c >= FONT_BEGIN && c < FONT_END) {
		const stbtt_bakedchar *b = pfont->glyphs + c - FONT_BEGIN;
		ans.x = floor((float)ans.x + b->xadvance + 0.5f);
		ans.y = 16;
	}

	return ans;
}

void MyDrawChar(GUIContext* context, char c, float pos_x, float pos_y, const color_t& color) {
	if (c < pfont->begin && c >= pfont->end) {
		return;
	}

	const uint32_t pixel = color.w   << 24 | color.x   << 16 | color.y   <<  8 | color.z   <<  0;
	const stbtt_bakedchar *b = pfont->glyphs + c - 32;

	uint32_t* pixels = (uint32_t*)context->opaqueData;
	const bool clip_enabled = (((context->clip.z - context->clip.x) != 0) && ((context->clip.w - context->clip.y) != 0));

	const int minX = std::max<int>(context->clip.x, 0);
	const int minY = std::max<int>(context->clip.y, 0);
	const int maxX = std::min<int>(context->clip.z, context->extents.x);
	const int maxY = std::min<int>(context->clip.w, context->extents.y);

	for (int32_t y = 0; y < b->y1 - b->y0; ++y) {
		for (int32_t x = 0; x < b->x1 - b->x0; ++x) {
			const uint32_t sourceIndex = (b->y0 + y) * BITMAP_WIDTH + b->x0 + x;
			const int32_t px = pos_x + x + (b->xoff + 0.5f);
			const int32_t py = pos_y + y + (b->yoff + 0.5f);

			if ((px >= minX && px < maxX && py >= minY && py < maxY) ||
			    ((!clip_enabled) && (px >= 0 && px < (int32_t)context->extents.x && py >= 0 && py < (int32_t)context->extents.y))) {
				const uint32_t destIndex = py * context->extents.x + px;
				const uint8_t luminance = pfont->bitmap[sourceIndex];
				const uint32_t mask = luminance << 24 | luminance << 16 | luminance <<  8 | luminance <<  0;
				pixels[destIndex] = (pixels[destIndex] & ~mask) | (pixel & mask);
			}
		}
	}
}

void MyDrawIcon(GUIContext* context, int32_t id, const ivec4& bounds, const color_t& color) {
	static const int32_t ICON_LENGTH = 15;
	static const int32_t ICON_HALF_LENGTH = ICON_LENGTH / 2;
	static const int32_t ICON_SIZE = ICON_LENGTH * ICON_LENGTH;
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

		// GUI_ICON_PLAY
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

		// GUI_ICON_PAUSE
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

		// GUI_ICON_STOP
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

		// GUI_ICON_LOOP
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
		0,1,1,1,1,1,0,0,0,0,0,1,0,0,0,
		0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,
		0,0,0,1,0,0,0,0,0,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

		// GUI_ICON_FILE
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,
		0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,
		0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,
		0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
		0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

		// GUI_ICON_FODLER
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,
		0,0,1,0,0,0,0,1,1,1,1,1,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
		0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;
	const int32_t length = std::min(bounds.z - bounds.x, bounds.w - bounds.y);
	const ivec2 boundsMiddle = {(bounds.z - bounds.x) / 2, (bounds.w - bounds.y) / 2};

	uint32_t* pixels = (uint32_t*)context->opaqueData;

	if (length < ICON_LENGTH) {
		return;
	}

	if (id < 0 || id > (int)(sizeof(ICONS) / (ICON_LENGTH * ICON_LENGTH))) {
		return;
	}

	for (int32_t y = 0; y < ICON_LENGTH; ++y) {
		for (int32_t x = 0; x < ICON_LENGTH; ++x) {
			const uint8_t value = ICONS[id * ICON_SIZE + y * ICON_LENGTH + x];
			const int32_t py = bounds.y + boundsMiddle.y + y - ICON_HALF_LENGTH;
			const int32_t px = bounds.x + boundsMiddle.x + x - ICON_HALF_LENGTH;
			const uint32_t dstIndex = py * context->extents.x + px;
			if (py >= 0 && py < context->extents.y && px >= 0 && px < context->extents.x) {
				// TODO implement clipping
				pixels[dstIndex] = pixels[dstIndex] * !value + pixel * value;
			}
		}
	}
}