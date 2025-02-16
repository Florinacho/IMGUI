#ifndef __IMGUI_SKIN_WIN95_H__
#define __IMGUI_SKIN_WIN95_H__

#include "imgui.h"

namespace win95 {

static const color_t light1 = {255, 255, 255, 255};
static const color_t light2 = {223, 223, 223, 255};
static const color_t dark1  = {  0,   0,   0, 255};
static const color_t dark2  = {128, 128, 128, 255};
static const color_t panel =  {192, 192, 192, 255};
static const color_t blue  =  {  0,   0, 128, 255};

inline void drawBounds(const ivec4& bounds, bool invert = false) {
	guiDrawLine({bounds.x + 0, bounds.y + 0}, {bounds.z - 0, bounds.y + 0}, invert ? dark1 : light1); // Top
	guiDrawLine({bounds.x + 0, bounds.y + 0}, {bounds.x + 0, bounds.w + 0}, invert ? dark1 : light1); // Left
	guiDrawLine({bounds.z - 1, bounds.y + 1}, {bounds.z - 1, bounds.w - 1}, invert ? light2 : dark2); // Right
	guiDrawLine({bounds.x + 0, bounds.w - 0}, {bounds.z + 1, bounds.w - 0}, invert ? light2 : dark2); // Bottom
}

inline void drawDoubleBounds(const ivec4& bounds, bool invert = false) {
	drawBounds(bounds, invert);

	guiDrawLine({bounds.x + 1, bounds.y + 1}, {bounds.z - 1, bounds.y + 1}, invert ? dark2 : light2); // Top
	guiDrawLine({bounds.x + 1, bounds.y + 1}, {bounds.x + 1, bounds.w - 1}, invert ? dark2 : light2); // Left
	guiDrawLine({bounds.z - 0, bounds.y + 0}, {bounds.z - 0, bounds.w - 0}, invert ? light1 : dark1); // Right
	guiDrawLine({bounds.x + 0, bounds.w - 0}, {bounds.z + 1, bounds.w - 0}, invert ? light1 : dark1); // Bottom
}

inline void drawLabel(const ivec4& bounds, const char* text, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
        return;
    }

    if (flags & GUI_BACKGROUND) {
        guiDrawQuad(bounds, panel);
    }

    if (flags & GUI_OUTLINE) {
        drawDoubleBounds(bounds, true);
    }

    if (flags & GUI_FOREGROUND) {
        guiDrawText(text, {bounds.x + 2, bounds.y + 2, bounds.z - 2, bounds.w - 2}, dark1, flags);
    }
}

inline void drawWhiteLabel(const ivec4& bounds, const char* text, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
        return;
    }

    if (flags & GUI_BACKGROUND) {
        guiDrawQuad(bounds, light1);
    }

    if (flags & GUI_OUTLINE) {
        drawBounds(bounds, true);
    }

    if (flags & GUI_FOREGROUND) {
        guiDrawText(text, {bounds.x + 2, bounds.y + 2, bounds.z - 2, bounds.w - 2}, dark1, flags);
    }
}

inline void drawHorizontalSlider(const ivec4& bounds, const ivec4& box, float prog, uint32_t flags) {
	const int32_t middle = (bounds.w + bounds.y) / 2;

	auto drawStuff = [](const ivec4& bounds) {
		// Top
		guiDrawLine({bounds.x + 0, bounds.y + 0}, {bounds.z - 0, bounds.y + 0}, dark2);
		guiDrawLine({bounds.x + 1, bounds.y + 1}, {bounds.z - 1, bounds.y + 1}, dark1);
		// Left
		guiDrawLine({bounds.x + 0, bounds.y + 0}, {bounds.x + 0, bounds.w + 0}, dark2);
		guiDrawLine({bounds.x + 1, bounds.y + 1}, {bounds.x + 1, bounds.w - 1}, dark1);

		// Right
		guiDrawLine({bounds.z - 1, bounds.y + 1}, {bounds.z - 1, bounds.w - 1}, light2);
		guiDrawLine({bounds.z - 0, bounds.y + 0}, {bounds.z - 0, bounds.w - 0}, light1);

		// Bottom
		guiDrawLine({bounds.x + 1, bounds.w - 1}, {bounds.z - 0, bounds.w - 1}, light2);
		guiDrawLine({bounds.x + 0, bounds.w - 0}, {bounds.z + 1, bounds.w - 0}, light1);
	};

	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, panel);

		drawStuff({bounds.x, middle - 2, bounds.z, middle + 1});
	}

	if (flags & GUI_FOREGROUND) {
		guiDrawQuad(box, panel);
		drawDoubleBounds(box, flags & GUI_CLICKED);
	}
}

inline void drawHorizontalSlider2(const ivec4& bounds, const ivec4& box, float prog, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, light2);
	}

	if (flags & GUI_FOREGROUND) {
		guiDrawQuad(box, panel);
		drawDoubleBounds(box, flags & GUI_CLICKED);
	}
}

inline void drawButton(const ivec4& bounds, const char* text, uint32_t flags) {
	const int offset = (flags & GUI_CLICKED) ? 1 : 0;

	if ((flags & GUI_VISIBLE) == 0) {
        return;
    }

    if (flags & GUI_BACKGROUND) {
        guiDrawQuad(bounds, panel);
    }

    if (flags & GUI_OUTLINE) {
        drawDoubleBounds(bounds, flags & GUI_CLICKED);
    }

    if (flags & GUI_FOREGROUND) {
        guiDrawText(text, {bounds.x + 2 + offset, bounds.y + 2 + offset, bounds.z - 2 + offset, bounds.w - 2 + offset}, dark1, flags);
    }
}

inline void drawIconButton(const ivec4& bounds, int icon, uint32_t flags) {
	const int offset = (flags & GUI_CLICKED) ? 1 : 0;

	if ((flags & GUI_VISIBLE) == 0) {
        return;
    }

    if (flags & GUI_BACKGROUND) {
        guiDrawQuad(bounds, panel);
    }

    if (flags & GUI_OUTLINE) {
        drawDoubleBounds(bounds, flags & GUI_CLICKED);
    }

    if (flags & GUI_FOREGROUND) {
        guiDrawIcon(icon, {bounds.x + offset, bounds.y + offset, bounds.z + offset, bounds.w + offset}, dark1, (flags & GUI_ALIGN_CENTER));
	}
}


inline void drawCheckBox(const ivec4& bounds, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, light1);
	}

	if (flags & GUI_OUTLINE) {
        drawDoubleBounds(bounds, true);
	}

	if ((flags & GUI_FOREGROUND) && (flags & GUI_CLICKED)) {
		guiDrawIcon(GUI_ICON_CHECK, {bounds.x + 2, bounds.y + 2, bounds.z - 2, bounds.w - 2}, dark1, flags);
	}
}

inline void drawTextBox(const ivec4& absoluteBounds, const char* text, const ivec2& carrotPosition, uint32_t time, uint32_t flags) {
	const int32_t CARROT_WIDTH = 2;
	const int32_t PADDING = 0; // weird stuff happening

	if (absoluteBounds.z - absoluteBounds.x < PADDING || absoluteBounds.w - absoluteBounds.y < PADDING) {
		return;
	}

	const ivec4 bounds = {absoluteBounds.x + PADDING, absoluteBounds.y + PADDING, absoluteBounds.z - PADDING, absoluteBounds.w - PADDING};
	const int32_t height = bounds.w - bounds.y;
	const int32_t carrotHeight = std::min(height, guiGetCharSize(' ').y);

	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, light1);
	}

	if (flags & GUI_FOREGROUND) {
		if (flags & GUI_HIDDEN) {
			// TODO: DrawIcon(GUI_ICON_CLOSE) ?
		} else {
			guiDrawText(text, {bounds.x + 2, bounds.y + 2, bounds.z - 2, bounds.w - 2}, dark1, (flags & (GUI_MULTILINE | GUI_ALIGN_CENTER)));
		}

		if ((flags & GUI_CLICKED) && (time % 2 == 0)) {
			guiDrawQuad({carrotPosition.x, carrotPosition.y, carrotPosition.x + CARROT_WIDTH, carrotPosition.y + carrotHeight}, dark1);
		}
	}

	if (flags & GUI_OUTLINE) {
		drawDoubleBounds(bounds, true);
	}
}

inline void drawWindowHeaderLabel(const ivec4& bounds, const char* text, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
        return;
    }

    if (flags & GUI_BACKGROUND) {
        guiDrawQuad(bounds, blue);
    }

    if (flags & GUI_FOREGROUND) {
        guiDrawText(text, {bounds.x + 2, bounds.y + 2, bounds.z - 2, bounds.w - 2}, light1, flags);
    }
}

inline void drawPanel(const ivec4& bounds, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, panel);
	}

	if (flags & GUI_OUTLINE) {
		drawDoubleBounds(bounds);
	}
}

inline void drawInnerPanel(const ivec4& bounds, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, light1);
	}

	if (flags & GUI_OUTLINE) {
		drawDoubleBounds(bounds, true);
	}
}

inline void label(const char* text, uint32_t flags = GUI_FLAGS_LABEL) {
    ::label<drawLabel>(text, flags);
}

inline bool button(const char* text, uint32_t flags = GUI_FLAGS_BUTTON) {
    return ::button<drawButton>(text, flags);
}

inline bool horizontalSlider(float& proc, float step = 0.1f, float boxProc = 0, uint32_t flags = GUI_FLAGS_SLIDER) {
    return ::horizontalSlider<drawHorizontalSlider>(proc, step, boxProc, flags);
}

inline bool checkBox(bool &checked, uint32_t flags = GUI_FLAGS_CHECKBOX) {
    return ::checkBox<drawCheckBox>(checked, flags);
}

inline bool textBox(char* text, const uint32_t max_length, int& carrot, uint32_t flags = GUI_FLAGS_TEXTBOX, uint32_t padding = 2) {
    return ::textBox<drawTextBox>(text, max_length, carrot, flags, padding);
}

inline bool spinner(int& value, int step = 1, uint32_t flags = GUI_FLAGS_SPINNER) {
    return ::spinner<drawIconButton, drawWhiteLabel>(value, step, flags);
}

bool spinner(int& value, const char** labels, uint32_t count, int step = 1, uint32_t flags = GUI_FLAGS_SPINNER) {
    return ::spinner<drawIconButton, drawWhiteLabel>(value, labels, count, step, flags);
}

inline bool horizontalScrollBar(float& value, float barProc, float step = 0.1f) {
    return ::horizontalScrollBar<drawIconButton, drawHorizontalSlider2>(value, barProc, step);
}

Layout beginSplitPanel(uint8_t orientation, float& weight, uint32_t padding = 0, uint32_t flags = GUI_FLAGS_PANEL) {
	return ::beginSplitPanel<drawInnerPanel, drawIconButton>(orientation, weight, padding, flags);
}

Layout beginTabPanel(const char* names, int& selected, uint32_t margin, uint32_t flags = GUI_FLAGS_PANEL) {
	return ::beginTabPanel<drawPanel, drawButton>(names, selected, margin, flags);
}

Layout beginScrollPanel(int width, int height, int* offsetX, int* offsetY, uint32_t margin, uint32_t flags) {
	return ::beginScrollPanel<drawPanel, drawIconButton, drawHorizontalSlider2, drawHorizontalSlider2>(width, height, offsetX, offsetY, margin, flags);
}

Layout beginWindow(ivec4* rbounds, const char* title, const char* footer, uint32_t margin, uint32_t* flags) {
	return ::beginWindow<drawPanel, drawWindowHeaderLabel, drawIconButton>(rbounds, title, footer, margin, flags);
}

#define Win95SplitPanel(...)   for(Layout ___tmp = win95::beginSplitPanel(__VA_ARGS__);  ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define Win95TabPanel(...)     for(Layout ___tmp = win95::beginTabPanel(__VA_ARGS__);    ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define Win95ScrollPanel(...)  for(Layout ___tmp = win95::beginScrollPanel(__VA_ARGS__); ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define Win95Window(...)       for(Layout ___tmp = win95::beginWindow(__VA_ARGS__);      ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))

} // namespace win95

#endif // __IMGUI_SKIN_WIN95_H__