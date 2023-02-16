#ifndef __RENDERER_H__
#define __RENDERER_H__

//#define IMGUI_INCLUDE_WINDOW_MANAGER
#include "imgui.h"
#include "stb_truetype.h"

#define GUI_ICON_PLAY         (GUI_ICON_CUSTOM + 0)
#define GUI_ICON_PAUSE        (GUI_ICON_CUSTOM + 1)
#define GUI_ICON_STOP         (GUI_ICON_CUSTOM + 2)
#define GUI_ICON_REFRESH      (GUI_ICON_CUSTOM + 3)
#define GUI_ICON_FILE         (GUI_ICON_CUSTOM + 4)
#define GUI_ICON_FOLDER       (GUI_ICON_CUSTOM + 5)

typedef struct {
	uint8_t* bitmap;         // Bitmap data as a grayscale image
	uint32_t width, height;
	stbtt_bakedchar* glyphs; // Character metadata
	uint8_t begin;           // Begin character included in the bitmap
	uint8_t end;             // End character included in the bitmap
	float size;              // Character height
} STBFont;

typedef struct {
	GUIContext context;      // imGUI lib context
	STBFont font;               // Font 
} SoftGUI;

int  SoftGUIInit(SoftGUI* context, uint32_t width, uint32_t height, void* pixelBuffer, const char* fontFilename, float fontSize);
void SoftGUIUninit(SoftGUI* context);

#endif // __RENDERER_H__