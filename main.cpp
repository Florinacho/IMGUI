#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "WindowCanvas.h"
#define IMGUI_IMPLEMENTATION
#include "imgui.h"
#include "icons.h"
#include "desktop.h"

static const uint32_t FONT_BITMAP_WIDTH = 512;
static const uint32_t FONT_BITMAP_HEIGHT = 512;
static const float    FONT_SIZE = 16.0f;
static const uint8_t  FONT_BEGIN = ' ';
static const uint8_t  FONT_END = '~';
static const uint8_t  FONT_LENGTH = FONT_END - FONT_BEGIN;

typedef struct {
	uint8_t* bitmap;         // Bitmap data as a grayscale image
	stbtt_bakedchar* glyphs; // Character metadata
} STBFont;

static STBFont font;

int loadTTFFont(STBFont* font, const char* filename, float size) {
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

	font->bitmap = new uint8_t[FONT_BITMAP_WIDTH * FONT_BITMAP_HEIGHT];
	font->glyphs = new stbtt_bakedchar[FONT_LENGTH];
	stbtt_BakeFontBitmap(ttf_raw_data, 0, size, font->bitmap, FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT, FONT_BEGIN, FONT_LENGTH, font->glyphs);
	delete [] ttf_raw_data;

	return 0;
}

void freeTTFFont(STBFont* font) {
	delete [] font->bitmap;
	delete [] font->glyphs;
}

void drawQuad(GUIContext* context, const ivec4& bounds, const color_t& color) {
	uint32_t* pixels = (uint32_t*)context->opaqueData;

	for (int32_t y = std::max<int32_t>(bounds.y, 0); y < std::min<int32_t>(bounds.w, context->extents.y); ++y) {
		for (int32_t x = std::max<int32_t>(bounds.x, 0); x < std::min<int32_t>(bounds.z,context->extents.x); ++x) {
			pixels[y * context->extents.x + x] = color.x << 16 | color.y << 8 | color.z << 0;
		}
	}
}

void drawLine(GUIContext* context, const ivec2& begin, const ivec2& end, const color_t& color) {
	if (begin.x == end.x) {
		drawQuad(context, {begin.x, begin.y, end.x + 1, end.y}, color);
	} else if (begin.y == end.y){
		drawQuad(context, {begin.x, begin.y, end.x, end.y + 1}, color);
	} else {
		assert(!"Diagonal lines are not implemented");
	}
}

ivec2 charSize(GUIContext* guiContext, char c) {
	if (c < FONT_BEGIN && c >= FONT_END) {
		return {0, 0};
	}

	const stbtt_bakedchar *b = font.glyphs + c - FONT_BEGIN;
	return {(int32_t)floor(b->xadvance + 0.5f), (int32_t)FONT_SIZE};
}

void drawChar(GUIContext* context, char c, float pos_x, float pos_y, const color_t& color) {
	if (c < FONT_BEGIN && c >= FONT_END) {
		return;
	}

	const uint32_t pixel = color.w   << 24 | color.x   << 16 | color.y   <<  8 | color.z   <<  0;
	const stbtt_bakedchar *b = font.glyphs + c - 32;
	const bool clip_enabled = (((context->clip.z - context->clip.x) != 0) && ((context->clip.w - context->clip.y) != 0));
	const int minX = std::max<int>(context->clip.x, 0);
	const int minY = std::max<int>(context->clip.y, 0);
	const int maxX = std::min<int>(context->clip.z, context->extents.x);
	const int maxY = std::min<int>(context->clip.w, context->extents.y);

	uint32_t* pixels = (uint32_t*)context->opaqueData;

	for (int32_t y = 0; y < b->y1 - b->y0; ++y) {
		for (int32_t x = 0; x < b->x1 - b->x0; ++x) {
			const uint32_t sourceIndex = (b->y0 + y) * FONT_BITMAP_WIDTH + b->x0 + x;
			const int32_t px = pos_x + x + (b->xoff + 0.5f);
			const int32_t py = pos_y + y + (b->yoff + 0.5f);

			if ((px >= minX && px < maxX && py >= minY && py < maxY) ||
			    ((!clip_enabled) && (px >= 0 && px < (int32_t)context->extents.x && py >= 0 && py < (int32_t)context->extents.y))) {
				const uint32_t destIndex = py * context->extents.x + px;
				const uint8_t luminance = font.bitmap[sourceIndex];
				const uint32_t mask = luminance << 24 | luminance << 16 | luminance <<  8 | luminance <<  0;
				pixels[destIndex] = (pixels[destIndex] & ~mask) | (pixel & mask);
			}
		}
	}
}

void drawIcon(GUIContext* context, int32_t id, const ivec4& bounds, const color_t& color) {
	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;
	const int32_t length = std::min(bounds.z - bounds.x, bounds.w - bounds.y);
	const ivec2 boundsMiddle = {(bounds.z - bounds.x) / 2, (bounds.w - bounds.y) / 2};
	uint32_t* pixels = (uint32_t*)context->opaqueData;

	if ((length < ICON_LENGTH) || (id < 0) || (id > (int)(sizeof(ICONS) / ICON_SIZE))) {
		return;
	}

	for (int32_t y = 0; y < ICON_LENGTH; ++y) {
		for (int32_t x = 0; x < ICON_LENGTH; ++x) {
			const uint8_t value = ICONS[id * ICON_SIZE + y * ICON_LENGTH + x];
			const int32_t py = bounds.y + boundsMiddle.y + y - ICON_HALF_LENGTH;
			const int32_t px = bounds.x + boundsMiddle.x + x - ICON_HALF_LENGTH;
			const uint32_t dstIndex = py * context->extents.x + px;
			if (py >= 0 && py < context->extents.y && px >= 0 && px < context->extents.x) {
				pixels[dstIndex] = pixels[dstIndex] * !value + pixel * value;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	const uint32_t WINDOW_WIDTH = 1024;
	const uint32_t WINDOW_HEIGHT = 720;

	// Create window
	WCanvas canvas(WINDOW_WIDTH, WINDOW_HEIGHT, 32, "Immediate Mode GUI");
	uint32_t* pixelBuffer = (uint32_t*)canvas.getPixelBuffer();

	// Load TTF font
	if (loadTTFFont(&font, "consola.ttf", 16.0f) != 0) {
		printf("Failed to load font 'consola.ttf'.\n");
		return 1;
	}

	// Setup GUI
	GUIContext gui;
	guiContextInit(&gui, {0, 0, (int32_t)WINDOW_WIDTH, (int32_t)WINDOW_HEIGHT});
	gui.drawLine = drawLine;
	gui.drawQuad = drawQuad;
	gui.drawChar = drawChar;
	gui.drawIcon = drawIcon;
	gui.charSize = charSize;
	gui.opaqueData = pixelBuffer;
	gui.extents = {WINDOW_WIDTH, WINDOW_HEIGHT};
	gui.keyMap[GUI_KEY_BACK  ] = 0x16;
	gui.keyMap[GUI_KEY_HOME  ] = 0x6E;
	gui.keyMap[GUI_KEY_UP    ] = 0x6F;
	gui.keyMap[GUI_KEY_LEFT  ] = 0x71;
	gui.keyMap[GUI_KEY_RIGHT ] = 0x72;
	gui.keyMap[GUI_KEY_END   ] = 0x73;
	gui.keyMap[GUI_KEY_DOWN  ] = 0x74;
	gui.keyMap[GUI_KEY_DELETE] = 0x77;
	guiSetActiveContext(&gui);

	// Init demos
	desktopDemoInit();

	WEvent event;
	bool running = true;

	while(running) {
		// Parse events
		while (canvas.getEvent(event)) {
			switch (event.type) {
			case WEvent::Unknown :
			case WEvent::KeyReleased :
				break;
			case WEvent::WindowClose :
				running = false;
				break;
			case WEvent::KeyPressed :
				(event.ascii == '\0') ? guiOnKeyEvent(event.keyCode, true) : guiOnCharEvent(event.ascii);
				running = (event.keyCode != 0x09);
				break;
			case WEvent::CursorMove :
				WMOnCursorEvent(event.x, event.y);
				break;
			case WEvent::ButtonPressed :
				WMOnButtonEvent(event.button - 1, GUI_BUTTON_PRESSED);
				break;
			case WEvent::ButtonReleased :
				WMOnButtonEvent(event.button - 1, GUI_BUTTON_RELEASED);
				break;
			case WEvent::WheelDown :
				guiOnMouseWheelEvent(1);
				break;
			case WEvent::WheelUp :
				guiOnMouseWheelEvent(-1);
				break;
			}
		}

		memset(pixelBuffer, 100, WINDOW_WIDTH * WINDOW_HEIGHT * 4);

		GUIFrame() {
			desktopDemo();
		}

		canvas.blit();
	}
	freeTTFFont(&font);
	return 0;
}
