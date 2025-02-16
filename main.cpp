#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "WindowCanvas.h"
#define IMGUI_IMPLEMENTATION
#include "icons.h"
// #include "desktop.h"

#include "win95.h"

static const uint32_t WINDOW_WIDTH = 1024;
static const uint32_t WINDOW_HEIGHT = 720;
static const uint32_t FONT_BITMAP_WIDTH = 512;
static const uint32_t FONT_BITMAP_HEIGHT = 512;
static const uint32_t FONT_SIZE = 16;
static const uint8_t  FONT_BEGIN = ' ';
static const uint8_t  FONT_END = '~';
static const uint8_t  FONT_LENGTH = FONT_END - FONT_BEGIN;

typedef struct {
	uint8_t* bitmap;         // Bitmap data as a grayscale image
	stbtt_bakedchar* glyphs; // Character metadata
} STBFont;

static STBFont font;

int loadTTFFont(STBFont* font, const char* filename, uint32_t size) {
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
	stbtt_BakeFontBitmap(ttf_raw_data, 0, (float)size, font->bitmap, FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT, FONT_BEGIN, FONT_LENGTH, font->glyphs);
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
#include <assert.h>

void drawIconScale(GUIContext* context, int32_t id, const ivec4& bounds, const color_t& color, uint32_t flags) {
	const float sx = float(bounds.z - bounds.x) / float(ICON_LENGTH);
	const float sy = float(bounds.w - bounds.y) / float(ICON_LENGTH);
	const float srx = 1.0f / sx;
	const float sry = 1.0f / sy;

	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;
	uint32_t* pixels = (uint32_t*)context->opaqueData;

	if ((id < 0) || (id > (int)(sizeof(ICONS) / ICON_SIZE))) {
		return;
	}

	for (int32_t y = 0; y < int(float(ICON_LENGTH) * sy + 0.0f); ++y) {
		for (int32_t x = 0; x < int(float(ICON_LENGTH) * sx + 0.0f); ++x) {
			const uint32_t srcIndex = uint32_t(id * ICON_SIZE + int(float(y) * sry + 0.0f) * ICON_LENGTH + int(float(x) * srx + 0.0f));
			const uint8_t value = ICONS[srcIndex];
			const int32_t py = bounds.y + y;
			const int32_t px = bounds.x + x;
			const uint32_t dstIndex = py * context->extents.x + px;
			if (py >= 0 && py < context->extents.y && px >= 0 && px < context->extents.x) {
				pixels[dstIndex] = pixels[dstIndex] * !value + pixel * value;
			}
		}
	}
}

void drawIconFix(GUIContext* context, int32_t id, const ivec4& bounds, const color_t& color, uint32_t flags) {
	const int32_t length = std::min(bounds.z - bounds.x, bounds.w - bounds.y);
	const uint32_t pixel = color.x << 16 | color.y << 8 | color.z << 0;
	uint32_t* pixels = (uint32_t*)context->opaqueData;
	ivec2 boundsMiddle = {};

	switch (flags & (GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT)) {
	case GUI_ALIGN_LEFT :
		boundsMiddle.x = bounds.x;
		break;
	case GUI_ALIGN_RIGHT :
		boundsMiddle.x = bounds.z - ICON_LENGTH;
		break;
	case GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT :
		boundsMiddle.x = (bounds.z + bounds.x) / 2 - ICON_LENGTH / 2;
		break;
	default:
		boundsMiddle.x = bounds.x;
		break;
	}
	switch (flags & (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM)) {
	case GUI_ALIGN_TOP :
		boundsMiddle.y = bounds.y;
		break;
	case GUI_ALIGN_BOTTOM :
		boundsMiddle.y = bounds.w - ICON_LENGTH;
		break;
	case GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM :
		boundsMiddle.y = (bounds.w + bounds.y) / 2 - ICON_LENGTH / 2;
		break;
	default:
		boundsMiddle.y = bounds.y;
		break;
	}

	if ((length < ICON_LENGTH) || (id < 0) || (id > (int)(sizeof(ICONS) / ICON_SIZE))) {
		return;
	}

	for (int32_t y = 0; y < ICON_LENGTH; ++y) {
		for (int32_t x = 0; x < ICON_LENGTH; ++x) {
			const uint8_t value = ICONS[id * ICON_SIZE + y * ICON_LENGTH + x];
			const int32_t py = boundsMiddle.y + y;
			const int32_t px = boundsMiddle.x + x;
			const uint32_t dstIndex = py * context->extents.x + px;
			if (py >= 0 && py < context->extents.y && px >= 0 && px < context->extents.x) {
				pixels[dstIndex] = pixels[dstIndex] * !value + pixel * value;
			}
		}
	}
}

void drawIcon(GUIContext* context, int32_t id, const ivec4& bounds, const color_t& color, uint32_t flags) {
	if (flags & GUI_WINDOW_SIZE) {
		drawIconScale(context, id, bounds, color, flags);
	} else {
		drawIconFix(context, id, bounds, color, flags);
	}
}

const uint8_t IMG[] = {
	0, 0, 0, 0,
	0, 0, 2, 2,
	0, 2, 2, 2,
	0, 2, 2, 1,

	1, 2, 2, 1,
	1, 2, 2, 1,
	1, 2, 2, 1,
	1, 2, 2, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	1, 1, 0, 0,
	1, 1, 0, 0,

	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
};
static const uint32_t IMG_SX = 4;
static const uint32_t IMG_SY = 4;
static const uint32_t IMG_S = IMG_SX * IMG_SY;

const uint8_t IMG_NONE    = 0;
const uint8_t IMG_FLIP_X  = (1 << 0);
const uint8_t IMG_FLIP_Y  = (1 << 1);
const uint8_t IMG_SWAP_XY = (1 << 2);

const uint8_t IMG_ROT_90  = IMG_FLIP_X | IMG_SWAP_XY;
const uint8_t IMG_ROT_180 = IMG_FLIP_X | IMG_FLIP_Y;
const uint8_t IMG_ROT_270 = IMG_FLIP_Y | IMG_SWAP_XY;

int getPixelIndex(int x, int y, uint8_t flags) {
	x %= IMG_SX;
	y %= IMG_SY;

	int z;

	switch (flags) {
	case IMG_FLIP_X :
		x = IMG_SX  - 1 - x;
		break;
	case IMG_FLIP_Y :
		y = IMG_SY - 1 - y;
		break;
	case IMG_SWAP_XY :
		z = x;
		x = y;
		y = z;
		break;
	case IMG_FLIP_X | IMG_SWAP_XY :
		z = IMG_SX - 1 - x;
		x = y;
		y = z;
		break;
	case IMG_FLIP_Y | IMG_SWAP_XY :
		z = IMG_SY - 1 - y;
		y = x;
		x = z;
		break;
	case IMG_FLIP_X | IMG_FLIP_Y :
		x = IMG_SX  - 1 - x;
		y = IMG_SY - 1 - y;
		break;
	case IMG_FLIP_X | IMG_FLIP_Y | IMG_SWAP_XY:
		z = IMG_SX  - 1 - x;
		x = IMG_SY - 1 - y;
		y = z;
		break;
	}

	return (y * IMG_SX + x);
}

void drawTiledImage(uint32_t* pixels, const ivec4& bounds, uint32_t imgIndex = 0, uint8_t flags = 0) {
	const int32_t width  = abs(bounds.z - bounds.x);
	const int32_t height = abs(bounds.w - bounds.y);
	const int32_t px = std::min(bounds.x, bounds.z);
	const int32_t py = std::min(bounds.y, bounds.w);
	const uint32_t K[] = {
		0x00000000,
		0xFF555555,
		0xFFAAAAAA,
		0xFFAAAAAA,
	};

	if (width < (int)IMG_SX || height < (int)IMG_SY) {
		return;
	}

	for (int y = 0; y <  height; ++y) {
		for (int x = 0; x <  width; ++x) {
			const uint32_t cx = x + px;
			const uint32_t cy = y + py;
			if (cx < 0 || cx >= WINDOW_WIDTH || cy < 0 || cy >= WINDOW_HEIGHT) {
				continue;
			}
			const uint32_t dstIndex = cy * WINDOW_WIDTH + cx;
			const uint32_t sIndex = imgIndex * IMG_S + getPixelIndex(x, y, flags);
			const uint8_t src = IMG[sIndex];
			if (src > 0) {
				pixels[dstIndex] = K[src];
			}
		}
	}
}

void drawBounds(uint32_t* pixels, uint32_t imgIndex, const ivec4& bounds, uint32_t flags) {
	const uint32_t width  = abs(bounds.z - bounds.x);
	const uint32_t height = abs(bounds.w - bounds.y);
	const int32_t minX = std::min(bounds.x, bounds.z);
	const int32_t minY = std::min(bounds.y, bounds.w);
	const int32_t maxX = std::max(bounds.x, bounds.z);
	const int32_t maxY = std::max(bounds.y, bounds.w);

	if (width < IMG_SX * 2 || height < IMG_SY * 2) {
		return;
	}

	// Draw content
	drawTiledImage(pixels, {minX + (int)IMG_SX, minY + (int)IMG_SY, maxX - (int)IMG_SX, maxY - (int)IMG_SY}, imgIndex); 

	// Draw edges
	drawTiledImage(pixels, {minX, minY + (int)IMG_SY, minX + (int)IMG_SX, maxY - (int)IMG_SY}, 1, IMG_NONE); // LEFT
	drawTiledImage(pixels, {minX + (int)IMG_SX, minY, maxX - (int)IMG_SX, minY + (int)IMG_SY}, 1, IMG_ROT_90); // TOP
	drawTiledImage(pixels, {maxX - (int)IMG_SX, minY + (int)IMG_SY, maxX, maxY - (int)IMG_SY}, 1, IMG_ROT_180); // RIGHT
	drawTiledImage(pixels, {minX + (int)IMG_SX, maxY - (int)IMG_SY, maxX - (int)IMG_SX, maxY}, 1, IMG_ROT_270); // BOTTOM

	// Draw corners
	drawTiledImage(pixels, {minX, minY, minX + (int)IMG_SX, minY + (int)IMG_SY}, 0, IMG_NONE); // TOP LEFT
	drawTiledImage(pixels, {maxX, minY, maxX - (int)IMG_SX, minY + (int)IMG_SY}, 0, IMG_ROT_90); // TOP RIGHT
	drawTiledImage(pixels, {maxX, maxY, maxX - (int)IMG_SX, maxY - (int)IMG_SY}, 0, IMG_ROT_180); // BOTTOM RIGHT
	drawTiledImage(pixels, {minX, maxY, minX + (int)IMG_SX, maxY - (int)IMG_SY}, 0, IMG_ROT_270); // BOTTOM LEFT
}

void CustomDrawProcButton(const ivec4& bounds, const char* text, uint32_t flags) {
	int imgIndex = 0;
	if (flags & GUI_FOCUSED) imgIndex = 1;
	if (flags & GUI_CLICKED) imgIndex = 2;

	if (flags & GUI_VISIBLE) {
		drawBounds((uint32_t*)guiGetContext()->opaqueData, imgIndex, bounds, flags);

		const int offset = (flags & GUI_CLICKED) ? 1 : 0;
		guiDrawText(text, {bounds.x + offset, bounds.y + offset, bounds.z + offset, bounds.w + offset}, {255, 255, 255, 255}, flags);
	}
}

int main(int argc, char* argv[]) {
#if 0
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	// printf("Before: %d, %d, %d\n", a, b, c);
	// RUN_WITH(a, 11, b, 22, c, 33, d, 44) 
	// {
	// 	printf("Inside: %d, %d, %d\n", a, b, c);
	// }
	// printf("After: %d, %d, %d\n", a, b, c);
	int _ FOR_EACH(FE_BACKUP, a, 11);

	return 0;
#endif
	// Create window
	WCanvas canvas(WINDOW_WIDTH, WINDOW_HEIGHT, 32, "Immediate Mode GUI");
	uint32_t* pixelBuffer = (uint32_t*)canvas.getPixelBuffer();

	// Load TTF font
	if (loadTTFFont(&font, "consola.ttf", FONT_SIZE) != 0) {
		printf("Failed to load font 'consola.ttf'.\n");
		return 1;
	}

	// Init GUI
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
	guiSetContext(&gui);
	// guiSetLayout(GridLayout(1, 1));

	// Init demos
	// desktopDemoInit();

	WEvent event;
	bool running = true;

	while(running) {
		// Parse events
		while (canvas.getEvent(event)) {
			switch (event.type) {
			case WEvent::Unknown :
			case WEvent::WindowClose :
				running = false;
				break;
			case WEvent::KeyReleased :
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
				// if (event.button == 3) showControlPanel();
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
			static ivec4 bounds = {0, 0, 700, 400};
			static uint32_t flags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;
			Win95Window(&bounds, "", "", 0, &flags) { // Win95
				guiSetLayout(GridLayout(6, 3));

				// 0
				button<CustomDrawProcButton>("B1");

				// 1
				button("B2");

				// 2
				button(GUI_ICON_CHECK);

				// 3
				static int selectedTab = 0;
				Win95TabPanel("T0,T1,T2,T3", selectedTab, 3, GUI_FLAGS_PANEL | GUI_OUTLINE | GUI_BACKGROUND) {
					switch (selectedTab) {
					case 0 :
						PANEL(GridLayout(1, 5)) {
							static float f = 0.1f;
							win95::label("Test", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
							win95::horizontalSlider(f);
							PANEL(GridLayout(3, 1, GUI_HORIZONTAL, 1)) {
								// LAYOUT_WITH(max.x, 16) LAYOUT_WITH(max.y, 16) 
								{
									if (static bool value = false; win95::checkBox(value)) { }
									if (static bool value = true ; win95::checkBox(value)) { }
									if (static bool value = false; win95::checkBox(value)) { }
								}
							}
							static char text[32] = "win95";
							static int carrot = -1;
							win95::textBox(text, sizeof(text), carrot);
							win95::button("Start");
						}
						break;
					case 1 :
						PANEL(GridLayout(1, 5)) {
							static int index = 0;
							static const char* LABELS[] = {
								"label 0",
								"label 1",
								"label 2",
							};
							static const uint32_t LABEL_COUNT = sizeof(LABELS) / sizeof(LABELS[0]);
							static float progress = 0.1f;

							win95::spinner(index);
							win95::spinner(index, LABELS, LABEL_COUNT);
							win95::horizontalScrollBar(progress, 0.3f);
						}
						break;
					case 2 :
						static float tabProc = 0.5f;
						Win95SplitPanel(GUI_VERTICAL, tabProc, 0, GUI_FLAGS_PANEL | GUI_VISIBLE | GUI_BACKGROUND | GUI_OUTLINE) {
							win95::label("TOP");
							win95::label("BOT");
						}
						break;
					case 3 :
						static int offsetX = 0;
						static int offsetY = 0;
						Win95ScrollPanel(200, 200, &offsetX, &offsetY, 0, GUI_FLAGS_PANEL) { 
							guiSetLayout(GridLayout(2, 2));
							win95::button("TL");
							win95::button("TR");
							win95::button("BL");
							win95::button("BR");
						}
					}
				}

				// 4
				win95::label("Test Label", GUI_VISIBLE | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_LEFT_TOP);
				label(GUI_ICON_HOME, GUI_VISIBLE | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER);

				// 5
				if (static bool cb = false; checkBox(cb)) {}

				// 6
				if (static bool tg = false; toggle(tg)) { }

				// 6
				progressBar(0.33f);

				// Panel(SplitLayout(GUI_VERTICAL)) {
				// 	7
				// 	static int sp = 1;
				// 	if (spinner<CustomDrawProcButton>(sp)) {
				// 		printf("Spinner: %d\n", sp);
				// 	}

				// 	8
				// 	static const char* SP_TEXT[] = {
				// 		"Label 1",
				// 		"Label 2",
				// 		"Label 3",
				// 		"Label 4",
				// 	};
				// 	static const uint32_t SP_TEXT_COUNT = sizeof(SP_TEXT) / sizeof(SP_TEXT[0]);
				// 	static int sp2 = 0;
				// 	if (spinner<CustomDrawProcButton>(sp2, SP_TEXT, SP_TEXT_COUNT)) {
				// 		printf("Spinner: %d\n", sp2);
				// 	}
				// }

				// 9
				static char tb[32] = "Test string";
				static int carrot = -1;
				// GUI_MULTILINE, GUI_ALIGN_CENTER - broken
				if (textBox(tb, sizeof(tb), carrot, GUI_FLAGS_TEXTBOX)) {
					printf("TextBox: %.*s\n", carrot, tb);
				}

				// 10 - Horizontal sliders
				// LAYOUT_WITH(max.y, 16) 
				{
					if (static float value = 0.25f; slider(value, GUI_HORIZONTAL)) {
						printf("H slider: %f\n", value);
					}
				}

				// 11 - Vertical sliders
				// LAYOUT_WITH(max.x, 16) 
				{
					if (static float value = 0.25f; slider(value, GUI_VERTICAL)) {
						printf("V slider: %f\n", value);
					}
				}

				// 12 - Horizontal scrollbars
				// LAYOUT_WITH(max.y, 16) 
				{
					if (static float value = 0.25f; scrollBar(value, 0.5f, GUI_HORIZONTAL)) {
						printf("H scrollbar: %f\n", value);
					}
				}

				// 13 - Vertical scrollbars
				// RUN_WITH(guiGetLayout()->max.x, 16) 
				{
					if (static float value = 0.25f; scrollBar(value, 0.25f, GUI_VERTICAL)) {
					}
				}
			}
		}

		canvas.blit();
	}

	freeTTFFont(&font);
	return 0;
}
