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
	Font font;
	uint32_t pixels;
	uint32_t screen_width;
	uint32_t screen_height;
} MyContext;

int LoadTTFFont(Font* font, const char* filename, float size);

void MyDrawLine(GUIContext* context, const ivec2& begin, const ivec2& end, const color& color);
void MyDrawQuad(GUIContext* context, const ivec4& bounds, const color& color);
void MyDrawChar(GUIContext* context, char c, float& pos_x, float& pos_y, const color& color);
void MyDrawText(GUIContext* context, const char* text, const ivec4& bounds, const color& color, uint32_t flags);
void MyDrawIcon(GUIContext* context, int32_t , const ivec4& bounds, const color& color);
ivec2 MyGetTextSize(GUIContext* guiContext, const char* text, uint32_t length);

#endif // __BINDING_H__