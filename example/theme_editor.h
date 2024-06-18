#ifndef __DEMO_THEME_EDITOR_H__
#define __DEMO_THEME_EDITOR_H__

#include <imgui.h>
#include <stdint.h>

void colorEntry(const char* label, color_t& color) {	
	auto u8Slider = [](uint8_t& value) {
		GUIContext* gui = GUIGetActiveContext();
		const ivec4 bounds = LayoutGetAbsoluteBounds(false);
		bool ans = false;
		char tmp[4];
		
		float fvalue = (float)value / 255.0f;
		if (Slider(fvalue, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE)) {
			value = fvalue * 255.0f;
			ans = true;
		}
		snprintf(tmp, 4, "%.hhu", value);
		GUIDrawText(tmp, bounds, gui->style.colors[GUI_COLOR_TEXT], GUI_ALIGN_CENTER);
		return ans;
	};
	
	auto coloredRect = [](const color_t& color) {
		GUIDrawQuad(LayoutGetAbsoluteBounds(), color);
	};

	Panel(GridLayout(5, 1)) {
		Label(label);
		u8Slider(color.x);
		u8Slider(color.y);
		u8Slider(color.z);
		coloredRect(color);
	}
}

void themeEditorPanel() {
	GUIContext* gui = GUIGetActiveContext();
	Panel(GridLayout(1, 9)) {
		colorEntry("Text",     gui->style.colors[GUI_COLOR_TEXT]);
		colorEntry("Disabled", gui->style.colors[GUI_COLOR_TEXT_DISABLED]);
		colorEntry("Border",   gui->style.colors[GUI_COLOR_BORDER]);
		colorEntry("Panel",    gui->style.colors[GUI_COLOR_PANEL]);
		colorEntry("Titlebar", gui->style.colors[GUI_COLOR_TITLEBAR]);
		colorEntry("Pane",     gui->style.colors[GUI_COLOR_PANE]);
		colorEntry("Focused",  gui->style.colors[GUI_COLOR_FOCUSED]);
		colorEntry("Active",   gui->style.colors[GUI_COLOR_ACTIVE]);

		if (Button("Reset")) {
			gui->style.colors[GUI_COLOR_TEXT]          = {230, 230, 230, 255};
			gui->style.colors[GUI_COLOR_TEXT_DISABLED] = {150, 150, 150, 255};
			gui->style.colors[GUI_COLOR_TITLEBAR]      = { 25,  25,  25, 200};
			gui->style.colors[GUI_COLOR_PANEL]         = { 50,  50,  50, 255};
			gui->style.colors[GUI_COLOR_PANE]          = { 75,  75,  75, 255};
			gui->style.colors[GUI_COLOR_FOCUSED]       = {110, 110, 120, 255};
			gui->style.colors[GUI_COLOR_ACTIVE]        = { 75,  75, 255, 255};
			gui->style.colors[GUI_COLOR_BORDER]        = { 25,  25,  25, 255};
		}
	}
}

static ivec4 themeEditorBounds = {200, 200, 600, 500};
static uint32_t themeEditorFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

void themeEditorWindow() {
	Window(&themeEditorBounds, "Theme editor", 2, &themeEditorFlags) {
		themeEditorPanel();
	}
}

#endif // __DEMO_THEME_EDITOR_H__
