#ifndef __BINDING_H__
#define __BINDING_H__

#include <algorithm>

#define IMGUI_INCLUDE_WINDOW_MANAGER
#include "GUI.h"

#include "stb_truetype.h"

typedef struct {
	uint8_t* bitmap;
	stbtt_bakedchar* glyphs;
	uint8_t begin;
	uint8_t end;
	float size;
} Font;

typedef struct {
	GUIContext context;
	Font font;
} Win32GUIContext;

int LoadTTFFont(Font* font, const char* filename, float size);
void FreeTTFFont(Font* font);

int Win32GUIInit(Win32GUIContext* context, const char* fontFilename, float fontSize);
void Win32GUIUninit(Win32GUIContext* context);

void MyDrawLine(GUIContext* context, const ivec2& begin, const ivec2& end, const color& color);
void MyDrawQuad(GUIContext* context, const ivec4& bounds, const color& color);
void MyDrawChar(GUIContext* context, char c, float& pos_x, float& pos_y, const color& color);
void MyDrawText(GUIContext* context, const char* text, const ivec4& bounds, const color& color, uint32_t flags);
void MyDrawIcon(GUIContext* context, int32_t , const ivec4& bounds, const color& color);
ivec2 MyGetTextSize(GUIContext* guiContext, const char* text, uint32_t length);

#endif // __BINDING_H__