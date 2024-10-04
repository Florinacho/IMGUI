/*
TODO:
	- implement new absolute coord solution?
	- remove custom vector structs?
	- split logic / draw ?

=======================================================


Implemented elements:
  - Label: text, icon
  - Button: text, icon, text&icon
  - Checkbox
  - Toggle: horizontal
  - Spinner: integer, strings
  - Textbox
  - Progressbar
  - Slider: vertical, horizontal
  - RangeSlider: horizontal
  - Scrollbar: vertical, horizontal


Implemented layouts:
  - absolute
    * uses the bounds provided by the user and updates the position based on the parent

  - split: fixed vertical, fixed horizontal, floating vertical, floating horizontal
    * fixed: tries to preserve the fixed size for the first element by scaling the second one
    * floating: tries to preserve the first and second element ratio

  - border: floating vertical, floating horizontal
    * similar to a floating split layout but with 3 regions

  - grid
    *divides the available space into a given number of collumns and rows


Implemented containers:
  - Panel
    * groups multiple elements into one element

  - SplitPane
    * groups two elements with user interactable ratio into one element

  - TabPane
    * groups multiple elements into separate tabs and display the selected one

  - ScrollPane
    *groups multiple elements into one with a virtual size and uses scrollbars for navigation

  - Frame


Pane and Panels are both containers but Panes provide additional functionality not just grouping.

+-----------------------+
|        Margin         |
| +-------------------+ |
| |      Border       | |
| | +---------------+ | |
| | |    Padding    | | |
| | |  +---------+  | | |
| | |  | Content |  | | |
| | |  +---------+  | | |
| | +---------------+ | |
| +-------------------+ |
+-----------------------+
*/

#ifndef __GUI_H__
#define __GUI_H__

// The Window Manager extension offers the ability to have multiple overlappig windows.
// This can be achieved by only storing the window's bounds, flags and ID. No Window code is stored.
// Uncomment the following line if you want Window Manager support. 
#define IMGUI_EXT_WINDOW_MANAGER

#include <stdint.h>
#include <stdbool.h>

#define GUI_VERTICAL       0x0
#define GUI_HORIZONTAL     0x1

#define GUI_LAYOUT_NONE    0x0
#define GUI_LAYOUT_GRID    0x1
#define GUI_LAYOUT_SPLIT   0x2
#define GUI_LAYOUT_BORDER  0x3

#define GUI_EVENT_KEY_DOWN  0x0
#define GUI_EVENT_KEY_UP    0x1
#define GUI_EVENT_CHAR      0x2

#define GUI_BUTTON_LEFT    0x0
#define GUI_BUTTON_MIDDLE  0x1
#define GUI_BUTTON_RIGHT   0x2

#define GUI_BUTTON_RELEASED  0x0
#define GUI_BUTTON_PRESSED   0x1

#define GUI_KEY_BACK    0x0
#define GUI_KEY_DELETE  0x1
#define GUI_KEY_HOME    0x2
#define GUI_KEY_END     0x3
#define GUI_KEY_LEFT    0x4
#define GUI_KEY_RIGHT   0x5
#define GUI_KEY_UP      0x6
#define GUI_KEY_DOWN    0x7
#define GUI_KEY_COUNT   0x8

#define GUI_ICON_CLOSE        0x00
#define GUI_ICON_ARROW_LEFT   0x01
#define GUI_ICON_ARROW_RIGHT  0x02
#define GUI_ICON_ARROW_DOWN   0x03
#define GUI_ICON_ARROW_UP     0x04
#define GUI_ICON_SIZE         0x05
#define GUI_ICON_CHECK        0x06
#define GUI_ICON_PLAY         0x07
#define GUI_ICON_PAUSE        0x08
#define GUI_ICON_STOP         0x09
#define GUI_ICON_REFRESH      0x0A
#define GUI_ICON_FILE         0x0B
#define GUI_ICON_FOLDER       0x0C
#define GUI_ICON_HOME         0x0D
#define GUI_ICON_CUSTOM       0x0D

#define GUI_MAX_KEY_EVENT_COUNT 4

// Skin colors
#define GUI_COLOR_TEXT            0x0
#define GUI_COLOR_TEXT_DISABLED   0x1
#define GUI_COLOR_BORDER          0x2
#define GUI_COLOR_PANEL           0x3
#define GUI_COLOR_TITLEBAR        0x4
#define GUI_COLOR_PANE            0x5
#define GUI_COLOR_FOCUSED         0x6
#define GUI_COLOR_ACTIVE          0x7
#define GUI_COLOR_COUNT           0x8

// Skin variables
#define GUI_VALUE_TITLEBAR_HEIGHT 0x0
#define GUI_VALUE_SLIDER_WIDTH    0x1
#define GUI_VALUE_COUNT           0x2

#define GUI_NONE                (0)
#define GUI_VISIBLE             (1 <<  0)
#define GUI_ENABLED             (1 <<  1)
#define GUI_BACKGROUND          (1 <<  2)
#define GUI_FOREGROUND          (1 <<  3)
#define GUI_OUTLINE             (1 <<  4)
#define GUI_FOCUSED             (1 <<  5)
#define GUI_CLICKED             (1 <<  6)
#define GUI_WINDOW_TITLEBAR     (1 <<  7)
#define GUI_WINDOW_CLOSE        (1 <<  8)
#define GUI_WINDOW_MOVE         (1 <<  9)
#define GUI_WINDOW_SIZE         (1 << 10)
#define GUI_WINDOW_DECORATION   (GUI_WINDOW_TITLEBAR | GUI_WINDOW_CLOSE)
#define GUI_HIDDEN              (1 << 11)
#define GUI_MULTILINE           (1 << 12)
#define GUI_ALIGN_LEFT          (1 << 13)
#define GUI_ALIGN_BOTTOM        (1 << 14)
#define GUI_ALIGN_RIGHT         (1 << 15)
#define GUI_ALIGN_TOP           (1 << 16)

#define GUI_ALIGN_LEFT_TOP       (GUI_ALIGN_TOP                    | GUI_ALIGN_LEFT)
#define GUI_ALIGN_LEFT_CENTER    (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT)
#define GUI_ALIGN_LEFT_BOTTOM    (                GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT)
#define GUI_ALIGN_BOTTOM_CENTER  (                GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT)
#define GUI_ALIGN_BOTTOM_RIGHT   (                GUI_ALIGN_BOTTOM                  | GUI_ALIGN_RIGHT)
#define GUI_ALIGN_CENTER_RIGHT   (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM                  | GUI_ALIGN_RIGHT)
#define GUI_ALIGN_TOP_RIGHT      (GUI_ALIGN_TOP                                     | GUI_ALIGN_RIGHT)
#define GUI_ALIGN_TOP_CENTER     (GUI_ALIGN_TOP                    | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT)
#define GUI_ALIGN_CENTER         (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT)

#define GUI_ALIGN_MASK           (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT)

#define GUI_FLAGS_LABEL          (GUI_VISIBLE |                                GUI_FOREGROUND |               GUI_ALIGN_CENTER)
#define GUI_FLAGS_BUTTON         (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER)
#define GUI_FLAGS_SPINNER        (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND |               GUI_ALIGN_CENTER)
#define GUI_FLAGS_CHECKBOX       (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE)
#define GUI_FLAGS_PROGRESSBAR    (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE)
#define GUI_FLAGS_SLIDER         (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE)
#define GUI_FLAGS_TEXTBOX        (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_LEFT_TOP)
#define GUI_FLAGS_PANEL          (GUI_VISIBLE)
#define GUI_FLAGS_WINDOW         (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION)  /*| GUI_WINDOW_SIZE*/

typedef struct {
	int32_t x, y;
} ivec2;

typedef struct {
	uint8_t x, y, z, w;
} color_t;

typedef struct IVEC4 {
	int32_t x, y, z, w;
	bool operator != (const struct IVEC4& other) const {
		return (x != other.x || y != other.y || z != other.z || w == other.w);
	}
} ivec4;

typedef struct Layout {
	union {
		struct {
			ivec2 count;
			ivec2 size;  // cached data
		} grid;
		struct {
			float weight;
			uint8_t orientation;
			uint8_t separator;
		} split;
		struct {
			float weight[3];
			uint8_t orientation;
		} border;
	};
	ivec4 backup_clip;
	ivec4 backup_viewport;
	ivec2 max;
	int32_t elementIndex;
	uint8_t margin;
	uint8_t type;
	bool run_statement;
} Layout;

typedef struct {
	color_t colors[GUI_COLOR_COUNT];
	uint8_t values[GUI_VALUE_COUNT];
} GUISkin;

typedef struct {
	uint32_t value;
	uint8_t type;
} KeyEvent;

struct GUIContext;
typedef void  (*DrawLineProc)(GUIContext*, const ivec2&, const ivec2&, const color_t&);
typedef void  (*DrawQuadProc)(GUIContext*, const ivec4&, const color_t&);
typedef void  (*DrawCharProc)(GUIContext*, char c, float pos_x, float pos_y, const color_t& color);
typedef void  (*DrawTextProc)(GUIContext*, const char*, const ivec4&, const color_t&, uint32_t);
typedef void  (*DrawIconProc)(GUIContext*, int32_t, const ivec4&, const color_t&, uint32_t);
typedef void  (*DrawBorderProc)(GUIContext*, const ivec4&, const color_t&);
typedef ivec2 (*CharSizeProc)(GUIContext*, char);
typedef ivec2 (*TextSizeProc)(GUIContext*, const char*, uint32_t);

#if defined IMGUI_EXT_WINDOW_MANAGER

#define WM_WINDOW_COUNT  16

// Window Manager Flags
#define WMF_NONE        0 
#define WMF_MODAL_LOCK  1

// TODO: Split functionality?
// #define WMF_MODAL_LOCK_POSITION  (1 << 0)
// #define WMF_MODAL_LOCK_VISIBLE   (1 << 1)
// #define WMF_MODAL_LOCK_EVENTS    (1 << 2)
// #define WMF_MODAL_SOFT           (WMF_NONE)
// #define WMF_MODAL_HARD           (WMF_MODAL_SOFT | WMF_MODAL_LOCK_POSITION | WMF_MODAL_LOCK_VISIBLE | WMF_MODAL_LOCK_EVENTS)

typedef struct WindowInfo {
	ivec4* bounds;
	uint32_t* flags;
	int32_t id;
	bool receiveEvents;
} WindowInfo;

typedef struct WindowManager {
	WindowInfo windows[WM_WINDOW_COUNT];
	uint32_t count;
	uint32_t flags;
	int32_t modal;
} WindowManager;

#endif // IMGUI_EXT_WINDOW_MANAGER

typedef struct GUIContext {
	Layout layout;
	GUISkin skin;
	uint32_t keyMap[GUI_KEY_COUNT];

	void* opaqueData;	
	DrawLineProc drawLine;
	DrawQuadProc drawQuad;
	DrawCharProc drawChar;
	DrawTextProc drawText;
	DrawIconProc drawIcon;
	DrawBorderProc drawBorder;
	CharSizeProc charSize;
	TextSizeProc textSize;

	ivec2 mousePosition;
	ivec2 lastMousePosition;
	bool mouseButtonLeft;
	bool lastMouseButtonLeft;
	int32_t mouse_wheel_delta;

	KeyEvent keyEvents[GUI_MAX_KEY_EVENT_COUNT];
	uint32_t keyEventCount;
	bool events_enabled;

	ivec4 viewport;
	ivec4 clip;
	ivec2 extents; // used by drawing as stride
#if defined IMGUI_EXT_WINDOW_MANAGER
	WindowManager windowManager;
#endif
} GUIContext;

// Tools
void guiDrawChar(char c, const ivec2& position, const color_t& color);
void guiDrawText(const char* text, const ivec4& bounds, const color_t& color, uint32_t flags = 0);
void guiDrawLine(const ivec2& begin, const ivec2& end, const color_t& color);
void guiDrawQuad(const ivec4& bounds, const color_t& color);
void guiDrawIcon(int32_t id, const ivec4& bounds, const color_t& color, uint32_t flags = 0);
void guiDrawBorder(const ivec4& rect, const color_t& color);

// Events
void guiOnCursorEvent(int32_t x, int32_t y);
void guiOnMouseWheelEvent(int32_t delta);
void guiOnButtonEvent(uint32_t button, bool status);
void guiOnKeyEvent(uint32_t key, bool status);
void guiOnCharEvent(char c);

void guiContextInit(GUIContext* context, const ivec4& viewport);
void guiSetContext(GUIContext*);
GUIContext* guiGetContext();
int32_t guiBeginFrame();
void guiEndFrame();

// Layouts
Layout AbsoluteLayout(uint32_t margin = 0);
Layout SplitLayout(uint8_t orientation, float weight = 0.5f, uint32_t separator = 0, uint32_t margin = 0);
Layout FixSplitLayout(uint8_t orientation, int32_t size, uint32_t separator = 0, uint32_t margin = 0);
Layout BorderLayout(uint8_t orientation, float headerWeight = 0.33f, float footerWeight = 0.33f, uint32_t margin = 0);
Layout GridLayout(uint32_t x, uint32_t y, uint32_t margin = 2);
void guiSetLayout(const Layout& layout);
Layout* guiGetLayout();
ivec4 guiLayoutGetAbsoluteBounds(bool advance = true);

// Widgets
void DummyElement(uint32_t count = 1);
void Label(const char* text, uint32_t flags = GUI_FLAGS_LABEL);
void Label(int32_t iconID, uint32_t flags = GUI_FLAGS_LABEL);
bool Button(const char* text, uint32_t flags = GUI_FLAGS_BUTTON);
bool Button(int32_t iconID, uint32_t flags = GUI_FLAGS_BUTTON);
bool Button(int32_t inconID, const char* text, uint32_t flags = GUI_FLAGS_BUTTON);
bool CheckBox(bool& state, uint32_t flags = GUI_FLAGS_CHECKBOX);
bool Toggle(bool& state, uint32_t flags = GUI_FLAGS_CHECKBOX);
bool Spinner(int& value, int32_t step = 1, uint32_t flags = GUI_FLAGS_SPINNER);
bool Spinner(int& value, const char** textValues, uint32_t textCount, int32_t step = 1, uint32_t flags = GUI_FLAGS_SPINNER);
void ProgressBar(float progress, uint32_t flags = GUI_FLAGS_PROGRESSBAR);
bool Slider(float& value, uint8_t orientation, uint32_t flags = GUI_FLAGS_SLIDER);
bool RangeSlider(float& minValue, float& maxValue, uint8_t = GUI_VERTICAL, uint32_t flags = GUI_FLAGS_SLIDER);
bool Scrollbar(float& progress, float barProc = 0.1f, uint8_t orientation = GUI_VERTICAL, float step = 0.1f);
bool TextBox(char* text, uint32_t textLength, int32_t& carrot, uint32_t flags = GUI_FLAGS_TEXTBOX, uint32_t padding = 3);
bool TextArea(char*, uint32_t, int32_t&, uint32_t = GUI_FLAGS_TEXTBOX, uint32_t padding = 2);

// Containers
Layout guiBeginWindow(ivec4* bounds, const char* title = nullptr, const char* footer = nullptr, uint32_t padding = 0, uint32_t* flags = nullptr);
Layout guiBeginPanel(const Layout& layout = AbsoluteLayout(), uint32_t padding = 0, uint32_t flags = GUI_FLAGS_PANEL);
Layout guiBeginSplitPanel(uint8_t orientation, float& , uint32_t padding = 0, uint32_t flags = GUI_FLAGS_PANEL);
Layout guiBeginTabPanel(const char* names, int& selectedTab, uint32_t padding = 0, uint32_t flags = 0);
Layout guiBeginScrollPanel(int, int, int* = nullptr, int* = nullptr, uint32_t padding = 0, uint32_t flags = 0);
void guiEndPanel(Layout* backup);

#define GUIFrame(...)     for(int32_t __tmp = guiBeginFrame(__VA_ARGS__);       !__tmp; (__tmp += 1), guiEndFrame())
#define Panel(...)        for(Layout ___tmp = guiBeginPanel(__VA_ARGS__);       ___tmp.run_statement; guiEndPanel(&___tmp))
#define SplitPanel(...)   for(Layout ___tmp = guiBeginSplitPanel(__VA_ARGS__);  ___tmp.run_statement; guiEndPanel(&___tmp))
#define TabPanel(...)     for(Layout ___tmp = guiBeginTabPanel(__VA_ARGS__);    ___tmp.run_statement; guiEndPanel(&___tmp))
#define ScrollPanel(...)  for(Layout ___tmp = guiBeginScrollPanel(__VA_ARGS__); ___tmp.run_statement; guiEndPanel(&___tmp))
#define Window(...)       for(Layout ___tmp = guiBeginWindow(__VA_ARGS__);      ___tmp.run_statement; guiEndPanel(&___tmp))

#if defined (IMGUI_EXT_WINDOW_MANAGER)
bool WMRegister(ivec4* bounds, uint32_t* flags, int32_t id = -1);
bool WMUnregister(GUIContext* context, int32_t id);

void WMBringIndexToFront(int32_t selected);
int32_t WMGetIndexByID(int32_t id);
void WMBringIDToFront(int32_t id);

void WMOnCursorEvent(int32_t x, int32_t y);
void WMOnButtonEvent(int32_t button, int32_t value);
#endif // IMGUI_EXT_WINDOW_MANAGER

#ifdef IMGUI_IMPLEMENTATION
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RECT_CONTAINS_POINT(RECT, POINT) (((POINT).x >= (RECT).x) && ((POINT).x < (RECT).z) && ((POINT).y >= (RECT).y) && ((POINT).y < (RECT).w))

inline ivec4 min(const ivec4& A, const ivec4& B) {
	return { std::max(A.x, B.x), std::max(A.y, B.y), std::min(A.z, B.z), std::min(A.w, B.w), };
}

void RectClip(ivec4& rect, const ivec4& bounds) {
	rect.x = std::clamp(rect.x, bounds.x, bounds.z);
	rect.y = std::clamp(rect.y, bounds.y, bounds.w);
	rect.z = std::clamp(rect.z, bounds.x, bounds.z);
	rect.w = std::clamp(rect.w, bounds.y, bounds.w);
}
int RectGetArea(const ivec4& rect) {
	int width = rect.z - rect.x;
	int height = rect.w - rect.y;
	return width * height;
}

// Global gui context pointer.
static GUIContext* __imgui_context = NULL;

void guiSetContext(GUIContext* context) {
	__imgui_context = context;
	
	assert(__imgui_context != NULL);
	assert(__imgui_context->drawLine != NULL);
	assert(__imgui_context->drawQuad != NULL);
	assert(__imgui_context->drawChar != NULL);
	assert(__imgui_context->drawText != NULL);
	assert(__imgui_context->drawIcon != NULL);
	assert(__imgui_context->charSize != NULL);
	assert(__imgui_context->textSize != NULL);
}

GUIContext* guiGetContext() {
	return __imgui_context;
}

// Lib tools
void guiDrawChar(char c, const ivec2& position, const color_t& color) {
	if (__imgui_context != NULL) {
		__imgui_context->drawChar(__imgui_context, c, position.x, position.y, color);
	}
}

void guiDrawText(const char* text, const ivec4& rect, const color_t& color, uint32_t flags) {
	if (__imgui_context != NULL) {
		// Clipping is done in the renderer
		__imgui_context->drawText(__imgui_context, text, rect, color, flags);
	}
}

void guiDrawLine(const ivec2& begin, const ivec2& end, const color_t& color) {
	if (__imgui_context != NULL) {
		__imgui_context->drawLine(__imgui_context, begin, end, color);
	}
}

void guiDrawQuad(const ivec4& rect, const color_t& color) {
	ivec4 bounds = rect;
	if (__imgui_context == NULL) {
		return;
	}
	if (RectGetArea(__imgui_context->clip) > 0) {
		RectClip(bounds, __imgui_context->clip);
	}
	__imgui_context->drawQuad(__imgui_context, bounds, color);
}

void guiDrawIcon(int32_t id, const ivec4& rect, const color_t& color, uint32_t flags) {
	ivec4 bounds = rect;
	if (__imgui_context == NULL) {
		return;
	}
	// This should be reated in the drawIcon proc
	// if (RectGetArea(__imgui_context->clip) > 0) {
	// 	RectClip(bounds, __imgui_context->clip);
	// }
	__imgui_context->drawIcon(__imgui_context, id, bounds, color, flags);
}

void guiDrawBorder(const ivec4& rect, const color_t& color) {
	ivec4 bounds = rect;
	if (__imgui_context == NULL) {
		return;
	}
	if (RectGetArea(__imgui_context->clip) > 0) {
		RectClip(bounds, __imgui_context->clip);
	}
	__imgui_context->drawBorder(__imgui_context, bounds, color);
}

void DrawBorder(GUIContext* context, const ivec4& bounds, const color_t& color) {
	if (((bounds.w - bounds.y) <= 0) || ((bounds.z - bounds.x) <= 0)) {
		return;
	}
	guiDrawLine({bounds.x, bounds.y}, {bounds.x, bounds.w}, color);
	guiDrawLine({bounds.x, bounds.w - 1}, {bounds.z, bounds.w - 1}, color);
	guiDrawLine({bounds.z - 1, bounds.y}, {bounds.z - 1, bounds.w}, color);
	guiDrawLine({bounds.x, bounds.y}, {bounds.z, bounds.y}, color);
}

ivec2 TextSize(GUIContext* context, const char* text, uint32_t length) {
	int maxX = 0;
	ivec2 ans = {0, 0};
	for (uint32_t index = 0; (index < length) && (text[index] != '\0'); ++index) {
		switch (text[index]) {
		case '\n' :
			ans.x = 0;
			ans.y += 16;
			break;
		default :
			const ivec2 charSize = context->charSize(context, text[index]);
			ans.x += charSize.x;
			ans.y = std::max(ans.y, charSize.y);
			maxX = std::max(maxX, ans.x);
			break;
		}
	}
	ans.x = std::max(ans.x, maxX);
	return ans;
}

// Text utility function
#define CUR_POS_BEGIN      0
#define CUR_POS_END        1
#define CUR_POS_PREV_LINE  2
#define CUR_POS_NEXT_LINE  3

int GetCursorPosition(const char* text, int cursor) {
	for (int ans = 0; ans < cursor; ++ans) {
		if (text[cursor - ans - 1] == '\n') {
			return ans;
		}
	}
	return cursor - 1;
}

int MoveCursor(const char* text, int cursor, int location) {
	int leftCount = 0;
	int max = 0;

	switch (location) {
	case CUR_POS_BEGIN :
		while ((cursor > 0) && (text[cursor - 1] != '\n')) --cursor;
		break;
	case CUR_POS_END :
		while (text[cursor] != '\0' && text[cursor] != '\n') ++cursor;
		break;
	case CUR_POS_PREV_LINE :
		leftCount = GetCursorPosition(text, cursor);
		for (cursor -= (leftCount + (leftCount != cursor)); ((cursor > 0) && (text[cursor - 1] != '\n')); --cursor) {
			++max;
		}
		cursor += std::min(leftCount, max);
		break;
	case CUR_POS_NEXT_LINE :
		leftCount = GetCursorPosition(text, cursor);

		cursor = MoveCursor(text, cursor, CUR_POS_END);
		cursor += (text[cursor] != '\0');

		for (int index = 0; (index < leftCount) && (text[cursor] != '\0') && (text[cursor] != '\n'); ++index) {
			++cursor;
		}
		break;
	}
	
	return cursor;
}

int GetLineXOffset(const char* text, const ivec4& bounds, uint32_t flags) {
	const int lineSize = __imgui_context->textSize(__imgui_context, text, MoveCursor(text, 0, CUR_POS_END)).x;
	const int halfLineSize = lineSize / 2;

	switch (flags & (GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT)) {
	case GUI_ALIGN_LEFT :
		return bounds.x;
	case GUI_ALIGN_RIGHT :
		return bounds.z - lineSize;
	case GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT :
		const int center = (bounds.z + bounds.x) / 2;
		const int halfBoundSize = (bounds.z - bounds.x) / 2 + 1;
		return center - std::min(halfLineSize, halfBoundSize);
	}
	return bounds.x;
}

int GetLineYOffset(const char* text, const ivec4& bounds, uint32_t flags) {
	const int midBounds = (bounds.y + bounds.w) / 2;
	const int halfBounds = (bounds.w - bounds.y) / 2 + 1;

	const int textSize = __imgui_context->textSize(__imgui_context, text, strlen(text)).y;
	const int halfTextSize = textSize / 2;

	switch (flags & (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM)) {
	case GUI_ALIGN_TOP :
		return bounds.y;
	case GUI_ALIGN_BOTTOM :
		return bounds.w - textSize;
		break;
	case GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM :
		return midBounds - std::min(halfTextSize, halfBounds);
		break;
	}

	return bounds.y;
}

void DrawText(GUIContext* context, const char* text, const ivec4& bounds, const color_t& color, uint32_t flags) {
	float pos_x = GetLineXOffset(text, bounds, flags);
	float pos_y = GetLineYOffset(text, bounds, flags);

	pos_y += 13; // WTF ?!
	while (*text) {
		switch (*text) {
		case '\n' :
			if (flags & GUI_MULTILINE) {
				pos_y += 16.0f;
				pos_x = GetLineXOffset(text + 1, bounds, flags);
			}
			break;
		default :
			const int charWidth = context->charSize(context, *text).x;
			if ((pos_x + charWidth <= bounds.z && pos_y <= bounds.w)) { // seems to draw 1 more char 
				context->drawChar(context, *text, pos_x, pos_y, color);
				pos_x += charWidth;
			}
			break;
		}
		++text;
	}
}

bool GetMouseWheelDelta(int32_t *delta, bool force) {
	if ((__imgui_context == NULL) || (__imgui_context->mouse_wheel_delta == 0) || (force == false && __imgui_context->events_enabled == false) 
#if defined IMGUI_EXT_WINDOW_MANAGER
		|| (__imgui_context->windowManager.modal != -1)
#endif
	)  {
		return false;
	}
	*delta = __imgui_context->mouse_wheel_delta;
	return true;
}

ivec2 GetMousePosition(bool force = false) {
	if (__imgui_context != NULL && (__imgui_context->events_enabled || force)) {
		return __imgui_context->mousePosition;
	}
	return {-1, -1};
}
ivec2 GetLastMousePosition(bool force = false) {
	if (__imgui_context != NULL && (__imgui_context->events_enabled || force)) {
		return __imgui_context->lastMousePosition;
	}
	return {-2, -2};
}

bool GetMouseLeftButton(bool force = false) {
	return (__imgui_context != NULL && (__imgui_context->events_enabled || force) && __imgui_context->mouseButtonLeft);
}

bool GetLastMouseLeftButton(bool force = false) {
	return (__imgui_context != NULL && (__imgui_context->events_enabled || force) && __imgui_context->lastMouseButtonLeft);
}

inline bool IsFocused(const ivec4& bounds) {
	return RECT_CONTAINS_POINT(bounds, GetMousePosition());
}

inline bool WasFocused(const ivec4& bounds) {
	return RECT_CONTAINS_POINT(bounds, GetLastMousePosition());
}

inline bool IsClicked(const ivec4& bounds) {
	return ((IsFocused(bounds) || WasFocused(bounds)) && GetMouseLeftButton());
}

bool Movable(ivec4& bounds) {
	if (IsClicked(bounds)) {
		const ivec2 mousePosition = GetMousePosition();
		const ivec2 lastMousePosition = GetLastMousePosition();
		ivec2 delta =  {mousePosition.x - lastMousePosition.x, mousePosition.y - lastMousePosition.y};
		bounds.x += delta.x;
		bounds.y += delta.y;
		bounds.z += delta.x;
		bounds.w += delta.y;
		return true;
	}
	return false;
}

void guiContextInit(GUIContext* context, const ivec4& viewport) {
	context->layout = AbsoluteLayout();
	
	context->viewport = viewport;
		
	context->drawLine = NULL;
	context->drawQuad = NULL;
	context->drawChar = NULL;
	context->drawIcon = NULL;
	context->charSize = NULL;
	context->opaqueData = NULL;
	
	context->drawBorder = DrawBorder;
	context->drawText = DrawText;
	context->textSize = TextSize;
	
	context->lastMousePosition = {-1, -1};
	context->mousePosition = {-1, -1};
	context->lastMouseButtonLeft = false;
	context->mouseButtonLeft = false;
	context->keyEventCount = 0;
	context->mouse_wheel_delta = 0;
	context->events_enabled = true;
	
	// Default theme
	context->skin.colors[GUI_COLOR_TEXT]          = {230, 230, 230, 255};
	context->skin.colors[GUI_COLOR_TEXT_DISABLED] = {150, 150, 150, 255};
	context->skin.colors[GUI_COLOR_TITLEBAR]      = { 25,  25,  25, 200};
	context->skin.colors[GUI_COLOR_PANEL]         = { 50,  50,  50, 255};
	context->skin.colors[GUI_COLOR_PANE]          = { 75,  75,  75, 255};
	context->skin.colors[GUI_COLOR_FOCUSED]       = {110, 110, 120, 255};
	context->skin.colors[GUI_COLOR_ACTIVE]        = { 75,  75, 255, 255};
	context->skin.colors[GUI_COLOR_BORDER]        = { 25,  25,  25, 255};
	
	context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT] = 20;
	context->skin.values[GUI_VALUE_SLIDER_WIDTH]    =  7;

#if defined IMGUI_EXT_WINDOW_MANAGER
	context->windowManager.count = 0;
	context->windowManager.flags = WMF_NONE;
	context->windowManager.modal = -1;
#endif // IMGUI_EXT_WINDOW_MANAGER
	context->clip = {0, 0, 0, 0};
}

void guiOnCursorEvent(int32_t x, int32_t y) {
	__imgui_context->mousePosition.x = x;
	__imgui_context->mousePosition.y = y;
}

void guiOnMouseWheelEvent(int32_t delta) {
	__imgui_context->mouse_wheel_delta += delta;
}

void guiOnButtonEvent(uint32_t button, bool status) {
	if (button == 0) {
		__imgui_context->mouseButtonLeft = status;
	}
}

void guiOnKeyEvent(uint32_t key, bool status) {
	if (__imgui_context->keyEventCount >= GUI_MAX_KEY_EVENT_COUNT) {
		return;
	}
	(void)status;
	if (__imgui_context->keyEventCount < GUI_MAX_KEY_EVENT_COUNT) {
		__imgui_context->keyEvents[__imgui_context->keyEventCount++] = {key, GUI_EVENT_KEY_DOWN};
	}
}

void guiOnCharEvent(char c) {
	if (__imgui_context->keyEventCount >= GUI_MAX_KEY_EVENT_COUNT) {
		return;
	}
	if ((c >= 32 && c < 128) || (c == '\n')) {
		if (__imgui_context->keyEventCount < GUI_MAX_KEY_EVENT_COUNT) {
			__imgui_context->keyEvents[__imgui_context->keyEventCount++] = {(uint32_t)c, GUI_EVENT_CHAR};
		}
	}
}

int32_t guiBeginFrame() {
	// guiSetLayout(GridLayout(1, 1, 0));
	return 0;
}

void guiEndFrame() {
	__imgui_context->lastMouseButtonLeft = __imgui_context->mouseButtonLeft;
	__imgui_context->lastMousePosition = __imgui_context->mousePosition;
	__imgui_context->mouse_wheel_delta = 0;
	__imgui_context->keyEventCount = 0;
}

ivec4 guiLayoutGetBounds(bool advance) {
	ivec4 ans;

	switch (__imgui_context->layout.type) {
	case GUI_LAYOUT_NONE :
		ans.x = 0;
		ans.y = 0;
		ans.z = __imgui_context->viewport.z - __imgui_context->viewport.x;
		ans.w = __imgui_context->viewport.w - __imgui_context->viewport.y;
		break;
	case GUI_LAYOUT_SPLIT :
		{
		const int32_t width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
		const int32_t height = __imgui_context->viewport.w - __imgui_context->viewport.y;
		const int32_t index = __imgui_context->layout.elementIndex % 2;
		switch (__imgui_context->layout.split.orientation) {
		case GUI_VERTICAL :
			{
			const int32_t topHeight = (float)(height - __imgui_context->layout.split.separator) * __imgui_context->layout.split.weight + 0.5f;
			const int32_t botHeight = height - __imgui_context->layout.split.separator - topHeight;
			ans.x = 0;
			ans.y =             (topHeight + __imgui_context->layout.split.separator) * index;
			ans.z = width;
			ans.w = topHeight + (__imgui_context->layout.split.separator + botHeight) * index;
			}
			break;
		case GUI_HORIZONTAL :
			{
			const int32_t leftWidth  = (float)(width - __imgui_context->layout.split.separator) * __imgui_context->layout.split.weight + 0.5f;
			const int32_t rightWidth =  width - __imgui_context->layout.split.separator - leftWidth;
			ans.x =             (leftWidth + __imgui_context->layout.split.separator) * index;
			ans.y = 0;
			ans.z = leftWidth + (rightWidth + __imgui_context->layout.split.separator) * index;
			ans.w = height;
			} 
			break;
		}
		{
			const float midX = (float)(ans.x + ans.z) / 2.0f;
			const float midY = (float)(ans.w + ans.y) / 2.0f;
			const float width  = std::min<float>((ans.z - ans.x) / 2.0f + 0.5f, __imgui_context->layout.max.x / 2);
			const float height = std::min<float>((ans.w - ans.y) / 2.0f + 0.5f, __imgui_context->layout.max.y / 2);
			ans.x = midX - width;
			ans.y = midY - height;
			ans.z = midX + width;
			ans.w = midY + height;
		}
		}
		break;
	case GUI_LAYOUT_BORDER :
		{
		const float width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
		const float height = __imgui_context->viewport.w - __imgui_context->viewport.y;
		const int vertical = (__imgui_context->layout.border.orientation == GUI_VERTICAL);
		switch(__imgui_context->layout.elementIndex % 3) {
		case 0 : // header
			ans.x = 0;
			ans.y = 0;
			ans.z = vertical ? width : ((float)width * __imgui_context->layout.border.weight[0] + 0.5f);
			ans.w = vertical ? ((float)height * __imgui_context->layout.border.weight[0] + 0.5f) : height;
			break;
		case 1 : // body
			ans.x = vertical ? 0 : ((float)width * __imgui_context->layout.border.weight[0] + 0.5f);
			ans.y = vertical ? ((float)height * __imgui_context->layout.border.weight[0] + 0.5f) : 0;
			ans.z = vertical ? width : ans.x + ((float)width * __imgui_context->layout.border.weight[1] + 0.5f);
			ans.w = vertical ? ans.y + ((float)height * __imgui_context->layout.border.weight[1] + 0.5f) : height;
			break;
		case 2 : // footer
			ans.x = vertical ? 0 : width - ((float)width * __imgui_context->layout.border.weight[2] + 0.5f);
			ans.y = vertical ? height - ((float)height * __imgui_context->layout.border.weight[2] + 0.5f) : 0;
			ans.z = width;
			ans.w = height;
			break;
		}
		}
		{ // TMP: clamp max Y size
			const int32_t midX = (float)(ans.x + ans.z) / 2.0f;
			const int32_t midY = (float)(ans.w + ans.y) / 2.0f;
			int32_t width  = std::min<int>(ans.z - ans.x, __imgui_context->layout.max.x);
			int32_t height = std::min<int>(ans.w - ans.y, __imgui_context->layout.max.y);
			ans.x = midX - width / 2;
			ans.y = midY - height / 2;
			ans.z = midX + width / 2;
			ans.w = midY + height / 2;
		}
		break;
	case GUI_LAYOUT_GRID:
		{
		const int32_t indexX = (__imgui_context->layout.elementIndex % __imgui_context->layout.grid.count.x) % __imgui_context->layout.grid.count.x;
		const int32_t indexY = (__imgui_context->layout.elementIndex / __imgui_context->layout.grid.count.x) % __imgui_context->layout.grid.count.y;
		ans.x = (indexX + 0) * __imgui_context->layout.grid.size.x,
		ans.y = (indexY + 0) * __imgui_context->layout.grid.size.y,
		ans.z = (indexX + 1) * __imgui_context->layout.grid.size.x,
		ans.w = (indexY + 1) * __imgui_context->layout.grid.size.y;

		const float width  = std::min(ans.z - ans.x, (int)__imgui_context->layout.max.x);
		const float height = std::min(ans.w - ans.y, (int)__imgui_context->layout.max.y);
		const float midX = (float)(ans.z + ans.x) / 2.0f;
		const float midY = (float)(ans.w + ans.y) / 2.0f;
		ans.x = midX - width  / 2.0f;
		ans.y = midY - height / 2.0f;
		ans.z = midX + width  / 2.0f;
		ans.w = midY + height / 2.0f;
		}
		break;
	default :
		assert(!"Invalid branch!");
	}

	ans.x += __imgui_context->layout.margin;
	ans.y += __imgui_context->layout.margin;
	ans.z -= __imgui_context->layout.margin;
	ans.w -= __imgui_context->layout.margin;
	__imgui_context->layout.elementIndex += advance;

	return ans;
}

ivec4 guiLayoutGetAbsoluteBounds(bool advance) {
	ivec4 ans = guiLayoutGetBounds(advance);
	ans.x += __imgui_context->viewport.x;
	ans.y += __imgui_context->viewport.y;
	ans.z += __imgui_context->viewport.x;
	ans.w += __imgui_context->viewport.y;
	return ans;
}

Layout AbsoluteLayout(uint32_t margin) {
	Layout ans = {0};
	ans.type = GUI_LAYOUT_NONE;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	return ans;
}

Layout GridLayout(uint32_t x, uint32_t y, uint32_t margin) {
	Layout ans = {};
	ans.type = GUI_LAYOUT_GRID;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.grid.size.x = 1;
	ans.grid.size.y = 1;
	ans.margin = margin;
	ans.grid.count.x = std::max<int>(x, 1);
	ans.grid.count.y = std::max<int>(y, 1);
	return ans;
}

Layout SplitLayout(uint8_t orientation, float weight, uint32_t separator, uint32_t margin) {
	Layout ans = {};
	ans.type = GUI_LAYOUT_SPLIT;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.split.orientation = orientation;
	ans.split.separator = separator;
	ans.split.weight = weight;
	return ans;
}

Layout FixSplitLayout(uint8_t orientation, int32_t size, uint32_t separator, uint32_t margin) {
	const ivec4 bounds = guiLayoutGetBounds(false);
	const int wh[] = {
		bounds.z - bounds.x,
		bounds.w - bounds.y
	};
	
	Layout ans = {};
	ans.type = GUI_LAYOUT_SPLIT;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.split.orientation = orientation;
	ans.split.separator = separator;
	ans.split.weight = std::min<float>((float)size / (float)(wh[orientation == GUI_VERTICAL]),  1.0f);
	return ans;
}

Layout BorderLayout(uint8_t orientation, float headerWeight, float footerWeight, uint32_t margin) {
	assert((headerWeight + footerWeight) <= 1.0f);

	Layout ans = {};
	ans.type = GUI_LAYOUT_BORDER;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.border.weight[0] = headerWeight;
	ans.border.weight[1] = 1.0f - (headerWeight + footerWeight);
	ans.border.weight[2] = footerWeight;
	ans.border.orientation = orientation;
	return ans;
}

void DummyElement(uint32_t count) {
	__imgui_context->layout.elementIndex += count;
}

void LabelInternal(uint32_t flags, const ivec4& bounds) {
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
	}
}

void Label(const char* text, uint32_t flags) {
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);

	LabelInternal(flags, absoluteBounds);
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		guiDrawText(text, absoluteBounds, __imgui_context->skin.colors[GUI_COLOR_TEXT], (flags & GUI_ALIGN_CENTER));
	}
}

void Label(int32_t id, uint32_t flags) {
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);

	LabelInternal(flags, absoluteBounds);
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		guiDrawIcon(id, absoluteBounds, __imgui_context->skin.colors[GUI_COLOR_TEXT], flags);
	}
}

bool ButtonInternal(uint32_t flags, const ivec4& bounds) {
	ivec4 cbounds = bounds;
	if (RectGetArea(__imgui_context->clip) > 0) {
		RectClip(cbounds, __imgui_context->clip);
	}
	
	const bool focused = ((RECT_CONTAINS_POINT(cbounds, GetMousePosition()) && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED));
	const bool clicked = ((focused && GetMouseLeftButton()) || (flags & GUI_CLICKED));

	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			guiDrawQuad(bounds, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
	}

	return (focused && !GetMouseLeftButton() && GetLastMouseLeftButton());
}

bool Button(int32_t icon, const char* text, uint32_t flags) {
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		const int width = absoluteBounds.z - absoluteBounds.x;
		const int height = absoluteBounds.w - absoluteBounds.y;
		const int min = width < height ? width : height;
		guiDrawIcon(icon, {absoluteBounds.x, absoluteBounds.y, absoluteBounds.x + min, absoluteBounds.y + min}, __imgui_context->skin.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], flags);
		guiDrawText(text, {absoluteBounds.x + min, absoluteBounds.y, absoluteBounds.z, absoluteBounds.w}, __imgui_context->skin.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
	}

	return ans;
}

bool Button(const char* text, uint32_t flags) {
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	const bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		guiDrawText(text, absoluteBounds, __imgui_context->skin.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
	}

	return ans;
}

bool Button(int32_t icon, uint32_t flags) {
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	const bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		guiDrawIcon(icon, absoluteBounds, __imgui_context->skin.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED]/*, (flags & GUI_ALIGN_CENTER) */, flags);
	}

	return ans;
}

bool CheckBox(bool &checked, uint32_t flags) {
	__imgui_context->layout.max = {22, 22};
	ivec4 rect = guiLayoutGetAbsoluteBounds(true);
	__imgui_context->layout.max = {0x1FFFFFFF, 0x1FFFFFFF};

	const bool focused = RECT_CONTAINS_POINT(rect, GetMousePosition());
	bool ans = false;
	
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			guiDrawQuad(rect, __imgui_context->skin.colors[focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			guiDrawBorder(rect, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
		if ((flags & GUI_FOREGROUND) && checked) {
			guiDrawIcon(GUI_ICON_CHECK, {rect.x + 1, rect.y + 1, rect.z - 1, rect.w - 1}, __imgui_context->skin.colors[GUI_COLOR_ACTIVE], flags);
		}
	}
	if ((flags & GUI_ENABLED) && focused && !GetMouseLeftButton() && GetLastMouseLeftButton()) {
		checked = !checked;
		ans = true;
	}

	return ans;
}

bool Toggle(bool &enabled, uint32_t flags) {
	static const int32_t PADDING = 2;
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	const int32_t halfWidth = (absoluteBounds.z - absoluteBounds.x) / 2;
	const bool focused = RECT_CONTAINS_POINT(absoluteBounds, GetMousePosition());
	bool ans = false;

	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			guiDrawQuad(absoluteBounds, __imgui_context->skin.colors[focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_FOREGROUND) {
			const ivec4 buttonBounds = {absoluteBounds.x + halfWidth * enabled + PADDING * !enabled, absoluteBounds.y + PADDING, absoluteBounds.z - halfWidth * !enabled - PADDING * enabled, absoluteBounds.w - PADDING};
			guiDrawQuad(buttonBounds, __imgui_context->skin.colors[enabled ? GUI_COLOR_ACTIVE : GUI_COLOR_PANE]);
			guiDrawBorder(buttonBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
		if (flags & GUI_OUTLINE) {
			guiDrawBorder(absoluteBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
	}
	if ((flags & GUI_ENABLED) && focused && !GetMouseLeftButton() && GetLastMouseLeftButton()) {
		enabled = !enabled;
		ans = true;
	}

	return ans;
}

bool SpinnerInternal(int& value, const char* text, int speed, uint32_t flags) {
	static const uint32_t LABEL_FLAG_MASK = GUI_VISIBLE | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_MASK;
	static const uint32_t BUTTON_FLAG_MASK = GUI_VISIBLE | GUI_ENABLED;
	static const uint32_t BUTTON_FLAGS = GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
	static const uint32_t button_flags = BUTTON_FLAGS | (flags & BUTTON_FLAG_MASK);

	const ivec4 bounds = guiLayoutGetAbsoluteBounds(false);
	const int32_t buttonWidth = std::min((bounds.z - bounds.x) / 2, (int)__imgui_context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	const float buttonWidthProc = std::min<float>((float)buttonWidth/ (float)(bounds.z - bounds.x), 0.5f);
	const int oldValue = value;

	const bool focused = RECT_CONTAINS_POINT(bounds, GetMousePosition());// && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED));
	if (focused) {
		int32_t mouseWheelDelta = 0;
		if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
			value += mouseWheelDelta * speed;
		}
	}

	Panel(BorderLayout(GUI_HORIZONTAL, buttonWidthProc, buttonWidthProc, 0)) {
		value -= Button(GUI_ICON_ARROW_LEFT, button_flags);
		Label(text, flags & LABEL_FLAG_MASK);
		value += Button(GUI_ICON_ARROW_RIGHT, button_flags);
	}

	return (oldValue != value);
}


bool Spinner(int& value, int speed, uint32_t flags) {
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "%d", value);

	return SpinnerInternal(value, tmp, speed, flags);
}

bool Spinner(int& value, const char** labels, uint32_t count, int speed, uint32_t flags) {
	if (SpinnerInternal(value, labels[std::clamp<int>(value, 0, count - 1)], speed, flags)) {
		value = std::clamp<int>(value, 0, count - 1);
		return true;
	}
	return false;
}

void ProgressBar(float proc, uint32_t flags) {
	static const int32_t K = 2;
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	const int32_t width = absoluteBounds.z - absoluteBounds.x - K * 1;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;

	if ((width <= 0) || (height <= 0)) {
		return;
	}

	if (flags & GUI_VISIBLE) {
		ivec4 barBounds;
		barBounds.x = absoluteBounds.x + K;
		barBounds.y = absoluteBounds.y + K;
		barBounds.z = absoluteBounds.x + ((float)width * proc) + 0.5f;
		barBounds.w = absoluteBounds.w - K;

		if (flags & GUI_BACKGROUND) {
			guiDrawQuad(absoluteBounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			guiDrawBorder(absoluteBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
		if (flags & GUI_FOREGROUND) {
			guiDrawQuad(barBounds, __imgui_context->skin.colors[GUI_COLOR_ACTIVE]);
			guiDrawBorder(barBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
	}
}

bool SliderInternal(float& proc, float step, int32_t boxLength, uint8_t orientation, uint32_t flags, const ivec4& bounds) {
	const int32_t width  = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;
	ivec4 boxBounds;
	ivec2 begin;
	ivec2 end;
	bool ans = false;
	
	switch (orientation) {
	case GUI_VERTICAL:
		{
		const int32_t mid = (bounds.x + bounds.z) / 2;
		begin = {mid, bounds.y};
		end = {mid, bounds.w};
		int32_t k = (float)(height - boxLength) * proc;
		boxBounds = {bounds.x, bounds.w - k - boxLength, bounds.z, bounds.w - k};
		}
		break;
	case GUI_HORIZONTAL :
		{
		const int32_t mid = (bounds.w + bounds.y) / 2;
		begin = {bounds.x, mid};
		end = {bounds.z, mid};
		int32_t k = (float)(width - boxLength) * proc;
		boxBounds = {bounds.x + k, bounds.y, bounds.x + k + boxLength, bounds.w};
		}
		break;
	default :
		assert(false);
	}
	const bool focused = (flags & GUI_ENABLED) && (RECT_CONTAINS_POINT(bounds, GetMousePosition()) || RECT_CONTAINS_POINT(bounds, GetLastMousePosition()));
	const bool clicked = (focused && GetMouseLeftButton());

	if (focused) {
		int32_t mouseWheelDelta = 0;
		if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
			proc -= step * (float)mouseWheelDelta;
			proc = std::clamp(proc, 0.0f, 1.0f);
			ans = true;
		}
	}
	if (clicked) {
		float newValue;
		
		switch (orientation) {
		case GUI_VERTICAL:
			newValue = 1.0f - (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(height - boxLength);
			newValue = std::clamp(newValue, 0.0f, 1.0f);
			break;
		case GUI_HORIZONTAL:
			newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
			newValue = std::clamp(newValue, 0.0f, 1.0f);
			break;
		}
		if (newValue != proc) {
			proc = newValue;
			ans = true;
		}
	}
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			switch (orientation) {
			case GUI_VERTICAL :
				guiDrawQuad({begin.x - 0, begin.y,     end.x + 1, boxBounds.y}, __imgui_context->skin.colors[GUI_COLOR_PANE]);
				guiDrawQuad({begin.x - 1, boxBounds.w, end.x + 2, end.y      }, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				break;
			case GUI_HORIZONTAL:
				guiDrawQuad({begin.x,     begin.y - 1, boxBounds.x, end.y + 2}, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				guiDrawQuad({boxBounds.z, begin.y - 0, end.x,       end.y + 1}, __imgui_context->skin.colors[GUI_COLOR_PANE]);
				break;
			}
		}
		if (flags & GUI_FOREGROUND) {
			guiDrawQuad(boxBounds, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
			if (flags & GUI_OUTLINE) {
				guiDrawBorder(boxBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
			}
		}
		// if (flags & GUI_OUTLINE) {
		// 	guiDrawBorder(bounds, context->skin.colors[GUI_COLOR_BORDER]);
		// }
	}

	return ans;
}

bool Slider(float& proc, uint8_t orientation, uint32_t flags) {
	return SliderInternal(proc, 0.1f, __imgui_context->skin.values[GUI_VALUE_SLIDER_WIDTH], orientation, flags, guiLayoutGetAbsoluteBounds(true));
}

bool RangeSliderInternal(float& procMin, float& procMax, float step, int32_t boxLength, uint8_t orientation, uint32_t flags, const ivec4& bounds) {
	const int32_t width  = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;
	int32_t mid;
	ivec4 boxBounds[2] = {};
	bool ans = false;
	bool selectedLeft;

	switch (orientation) {
	case GUI_VERTICAL:
		{
		mid = (bounds.x + bounds.z) / 2;
		int32_t minPosition = (float)(height - boxLength * 2) * procMin;
		int32_t maxPosition = (float)(height - boxLength * 2) * procMax;
		boxBounds[0] = {
			bounds.x, bounds.w - minPosition - boxLength * 1,
			bounds.z, bounds.w - minPosition - boxLength * 0,
		};
		boxBounds[1] = {
			bounds.x, bounds.w - maxPosition - boxLength * 2,
			bounds.z, bounds.w - maxPosition - boxLength * 1,
		};
		selectedLeft = abs(GetMousePosition().y - boxBounds[0].w) < abs(GetMousePosition().y - boxBounds[1].y);
		}
		break;
	case GUI_HORIZONTAL:
		{
		mid = (bounds.w + bounds.y) / 2;
		int32_t minPosition = (float)(width - boxLength * 2) * procMin;
		int32_t maxPosition = (float)(width - boxLength * 2) * procMax;
		boxBounds[0] = {
			bounds.x + minPosition + boxLength * 0, bounds.y,
			bounds.x + minPosition + boxLength * 1, bounds.w,
		};
		boxBounds[1] = {
			bounds.x + maxPosition + boxLength * 1, bounds.y,
			bounds.x + maxPosition + boxLength * 2, bounds.w,
		};
		selectedLeft = abs(GetMousePosition().x - boxBounds[0].z + boxLength / 2) < abs(GetMousePosition().x - boxBounds[1].x - boxLength / 2);
		}
		break;
	default:
		assert(false);
		break;
	}
	const bool focused = (flags & GUI_ENABLED) && (RECT_CONTAINS_POINT(bounds, GetMousePosition()) || RECT_CONTAINS_POINT(bounds, GetLastMousePosition()));
	const bool clicked = (focused && GetMouseLeftButton());

	// Mouse wheel event
	if (focused) {
		int32_t mouseWheelDelta = 0;
		if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
			if (selectedLeft) {
				procMin = std::clamp(procMin - step * (float)mouseWheelDelta, 0.0f, procMax);
			} else {
				procMax = std::clamp(procMax - step * (float)mouseWheelDelta, procMin, 1.0f);
			}
			ans = true;
		}
	}

	// Mouse button event
	if (clicked) {
		float newValue = 0.0f;
		switch (orientation) {
		case GUI_VERTICAL :
			newValue = 1.0f - (float)(GetMousePosition().y -  selectedLeft * boxLength - boxLength / 2 - bounds.y) / (float)(height - boxLength * 2);
			break;
		case GUI_HORIZONTAL :
			newValue =        (float)(GetMousePosition().x - !selectedLeft * boxLength * 1 - boxLength / 2 - bounds.x) / (float)(width  - boxLength * 2);
			break;
		}
		if (selectedLeft) {
			procMin = std::clamp<float>(newValue, 0.0f, procMax);
		} else {
			procMax = std::clamp<float>(newValue, procMin, 1.0f);
		}
		ans = true;
	}

	// Render
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			switch (orientation) {
			case GUI_VERTICAL:
				guiDrawQuad({mid - 0,        bounds.y,       mid + 1, bounds.w      }, __imgui_context->skin.colors[GUI_COLOR_PANE]);
				guiDrawQuad({mid - 1,        boxBounds[1].y, mid + 2, boxBounds[0].y}, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				break;
			case GUI_HORIZONTAL:
				guiDrawQuad({bounds.x,       mid - 0, bounds.z,       mid + 1       }, __imgui_context->skin.colors[GUI_COLOR_PANE]);
				guiDrawQuad({boxBounds[0].z, mid - 1, boxBounds[1].x, mid + 2       }, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				break;
			}
		}
		if (flags & GUI_FOREGROUND) {
			guiDrawQuad(boxBounds[0], __imgui_context->skin.colors[(clicked &&  selectedLeft) ? GUI_COLOR_ACTIVE : (focused &&  selectedLeft) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
			guiDrawQuad(boxBounds[1], __imgui_context->skin.colors[(clicked && !selectedLeft) ? GUI_COLOR_ACTIVE : (focused && !selectedLeft) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			guiDrawBorder(boxBounds[0], __imgui_context->skin.colors[GUI_COLOR_BORDER]);
			guiDrawBorder(boxBounds[1], __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}
	}

	return ans;
}

bool RangeSlider(float& procMin, float& procMax, uint8_t orientation, uint32_t flags) {
	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	
	return RangeSliderInternal(procMin, procMax, 0.1f, __imgui_context->skin.values[GUI_VALUE_SLIDER_WIDTH], orientation, flags, absoluteBounds);
}

bool Scrollbar(float& value, float barProc, uint8_t orientation, float step) {
	const ivec4 rbounds = guiLayoutGetBounds(false);
	uint8_t buttonIcons[2];
	bool dir;
	int length;
	bool ans = false;

	switch (orientation) {
	case GUI_VERTICAL :
		length = rbounds.w - rbounds.y;
		buttonIcons[0] = GUI_ICON_ARROW_UP;
		buttonIcons[1] = GUI_ICON_ARROW_DOWN;
		dir = true;
		break;
	case GUI_HORIZONTAL :
		length = rbounds.z - rbounds.x;
		buttonIcons[0] = GUI_ICON_ARROW_LEFT;
		buttonIcons[1] = GUI_ICON_ARROW_RIGHT;
		dir = false;
		break;
	default :
		assert(false);
	}

	if (length < 32) { // TODO: Read Icon glyps size
		return false;
	}

	const int barLength = length - 32;
	const float buttonProc = std::min<float>(16.0f / (float)length, 0.5f);

	Panel(BorderLayout(orientation, buttonProc, buttonProc, 0), 0, GUI_FLAGS_PANEL) {
		if (Button(buttonIcons[0])) {
			value += dir ? step : - step;
			ans = true;
		}
		// if (Slider(value, barLength * barProc, orientation, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE)) {
		if (SliderInternal(value, step, barLength * barProc, orientation, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE, guiLayoutGetAbsoluteBounds(true))) {
			ans = true;
		}
		if (Button(buttonIcons[1])) {
			value += dir ? -step : step;
			ans = true;
		}
		value = std::clamp<float>(value, 0.0f, 1.0f);
	}

	return ans;
}

bool TextBox(char* text, const uint32_t max_length, int& carrot, uint32_t flags, uint32_t padding) {
	const int32_t CARROT_WIDTH = 1;
	const int32_t FONT_HEIGHT = 16;

	static time_t lastBlinkTime = 0;

	const ivec4 absoluteBounds = guiLayoutGetAbsoluteBounds(true);
	const ivec4 textBounds = {absoluteBounds.x + (int)padding, absoluteBounds.y + (int)padding, absoluteBounds.z - (int)padding, absoluteBounds.w - (int)padding};
	const int32_t width = textBounds.z - textBounds.x;
	const int32_t height = textBounds.w - textBounds.y;
	const bool focused = RECT_CONTAINS_POINT(textBounds, GetMousePosition(true)) && __imgui_context->events_enabled;
	const int offsetY = GetLineYOffset(text, textBounds, flags);
	int offsetX = 0;
	
	int32_t length = strlen(text);
	int32_t offset = 0;
	bool ans = false;

	// Offset text to keep carrot in viewport
	if (carrot > 0) {
		ivec2 pos = __imgui_context->textSize(__imgui_context, text, carrot);
		while (pos.x > width) {
			pos.x -= __imgui_context->textSize(__imgui_context, text, 1).x;
			++offset;
		}
	}

	// Read mouse events
	if (flags & GUI_ENABLED) {
		if (GetLastMouseLeftButton(true) && (GetMouseLeftButton(true) == false)) {
			if (focused) {
				// Jump lines
				int jumpLineCount = (GetMousePosition().y - offsetY) / 16;
				for (char* ptr = text; (*ptr != '\0') && (jumpLineCount > 0); ++ptr, ++offset) {
					switch (*ptr) {
					case '\n' :
						--jumpLineCount;
						break;
					}
				}
				// jump chars
				offsetX = ((flags & GUI_MULTILINE) ? GetLineXOffset(text + offset, textBounds, flags) : textBounds.x);
				int32_t pos_x = 0;
				for (carrot = offset; (carrot < length && text[carrot] != '\n'); ++carrot){ 
					int32_t charWidth = __imgui_context->textSize(__imgui_context, text + carrot, 1).x;
					if (pos_x + charWidth >= GetMousePosition().x - offsetX) {
						lastBlinkTime = time(NULL);
						break;
					} else {
						pos_x += charWidth;
					}
				}
				lastBlinkTime = time(NULL);
			} else {
				carrot = -1;
			}
		}
	} else {
		carrot = -1;
	}

	// Read key events
	if (carrot >= 0) {
		for (uint32_t index = 0; index < __imgui_context->keyEventCount; ++index) {
			switch (__imgui_context->keyEvents[index].type) {
			case GUI_EVENT_KEY_DOWN :
				if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_HOME]) {
					carrot = MoveCursor(text, carrot, CUR_POS_BEGIN);
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_END]) {
					carrot = MoveCursor(text, carrot, CUR_POS_END);
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_UP]) {
					carrot = MoveCursor(text, carrot, CUR_POS_PREV_LINE);
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_DOWN]) {
					carrot = MoveCursor(text, carrot, CUR_POS_NEXT_LINE);
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_LEFT]) {
					carrot = std::max(carrot - 1, 0);
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_RIGHT]) {
					carrot = std::min(carrot + 1, length);
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_BACK]) {
					if (carrot > 0) {
						for (int32_t index = --carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						ans = true;
					}
					lastBlinkTime = time(NULL);
				} else if (__imgui_context->keyEvents[index].value == __imgui_context->keyMap[GUI_KEY_DELETE]) {
					if (carrot < length) {
						for (int32_t index = carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						ans = true;
					}
					lastBlinkTime = time(NULL);
				}
				break;
			case GUI_EVENT_CHAR :
				switch (__imgui_context->keyEvents[index].value) {
				case '\n' : if ((flags & GUI_MULTILINE) == 0) break;
				default :
					if (length < (int)max_length - 1) {
						for (int32_t index = ++length; index >= carrot; --index) {
							text[index] = text[index - 1];
						}
						text[carrot++] = (char)__imgui_context->keyEvents[index].value;
						lastBlinkTime = time(NULL);
						ans = true;
					}
				}
				break;
			}
		}
	}
	
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			guiDrawQuad(absoluteBounds, __imgui_context->skin.colors[(carrot >= 0) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}

		if (flags & GUI_FOREGROUND) {
			if (height >= FONT_HEIGHT) {
				if (flags & GUI_HIDDEN) {
					// TODO: DrawIcon(GUI_ICON_CLOSE) ?
				} else {
					guiDrawText(text + (offset > 0) * offset, textBounds, __imgui_context->skin.colors[GUI_COLOR_TEXT], (flags & (GUI_MULTILINE | GUI_ALIGN_CENTER)));
				}
			}
			if ((carrot >= 0) && ((time(NULL) - lastBlinkTime) % 2 == 0)) {
				ivec2 carrotPosition = {0, 0};
				for (int index = 0; index < std::max(carrot - offset, 0) && (text[index] != '\0'); ++index) {
					switch (text[index]) {
					case '\n' :
						carrotPosition.x = 0;
						carrotPosition.y += 16;
						break;
					default :
						carrotPosition.x += __imgui_context->charSize(__imgui_context, text[index]).x;
						break;
					}
				}
				int carrotHeight = std::min(height, FONT_HEIGHT);
				
				
				offsetX = ((flags & GUI_MULTILINE) ? GetLineXOffset(text + MoveCursor(text, carrot, CUR_POS_BEGIN), textBounds, flags) : textBounds.x);
				guiDrawQuad({offsetX + carrotPosition.x, offsetY + carrotPosition.y, offsetX + carrotPosition.x + CARROT_WIDTH, offsetY + carrotPosition.y + carrotHeight}, __imgui_context->skin.colors[GUI_COLOR_TEXT]);
			}
		}

		if (flags & GUI_OUTLINE) {
			guiDrawBorder(absoluteBounds, __imgui_context->skin.colors[(carrot >= 0) ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_BORDER]);
		}
	}

	return ans;
}

bool TextArea(char* text, const uint32_t max_length, int& carrot, uint32_t flags, uint32_t padding) {
	bool ans = false;
	static int offsetX = 0;
	static int offsetY = 0;
	
	ivec2 size = __imgui_context->textSize(__imgui_context, text, strlen(text));
	size.x += padding * 2;
	size.y += padding * 2;
	
	// ScrollPanel(size.x, size.y, &offsetX, &offsetY, 0, GUI_FLAGS_PANEL) 
	{
		ans = TextBox(text, max_length, carrot, flags);
	}

	(void)offsetX;
	(void)offsetY;
	(void)size;
	
	return ans;
}

Layout guiBeginPanel(const Layout& layout, uint32_t padding, uint32_t flags) {
	__imgui_context->layout.backup_viewport = __imgui_context->viewport;
	__imgui_context->layout.backup_clip = __imgui_context->clip;

	__imgui_context->viewport = guiLayoutGetAbsoluteBounds(true);
	const int32_t width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
	const int32_t height = __imgui_context->viewport.w - __imgui_context->viewport.y;

	Layout ans = __imgui_context->layout;
	ans.run_statement = (width >= (int32_t)padding * 2) && (height >= (int32_t)padding * 2);
	if (ans.run_statement == false) {
		return ans;
	}
	
	__imgui_context->viewport.x += padding;
	__imgui_context->viewport.y += padding;
	__imgui_context->viewport.z -= padding;
	__imgui_context->viewport.w -= padding;

	if (RectGetArea(__imgui_context->clip) > 0) {
		__imgui_context->clip = {
			std::max(__imgui_context->clip.x, __imgui_context->viewport.x),
			std::max(__imgui_context->clip.y, __imgui_context->viewport.y),
			std::min(__imgui_context->clip.z, __imgui_context->viewport.z),
			std::min(__imgui_context->clip.w, __imgui_context->viewport.w)
		};
	} else {
		__imgui_context->clip = __imgui_context->viewport;
	}
	
	guiSetLayout(layout);

	return ans;
}

void guiEndPanel(Layout* bkp_layout) {
	bkp_layout->run_statement = false;

	__imgui_context->layout = *bkp_layout;
	__imgui_context->viewport = __imgui_context->layout.backup_viewport;
	__imgui_context->clip = __imgui_context->layout.backup_clip;
}

Layout guiBeginSplitPanel(uint8_t orientation, float& weight, uint32_t padding, uint32_t flags) {
	static const int32_t SEPARATOR = 4;
	const Layout ans = guiBeginPanel(SplitLayout(orientation, weight, SEPARATOR), padding, flags);
	const ivec4 absoluteBounds = __imgui_context->viewport; // DAFUQ ?!
	const int32_t width  = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;
	ivec4 separatorBounds = absoluteBounds;

	switch (orientation) {
	case GUI_VERTICAL :
		separatorBounds.y += (float)(height - SEPARATOR) * weight;
		separatorBounds.w  = separatorBounds.y + SEPARATOR;
		break;
	case GUI_HORIZONTAL :
		separatorBounds.x += (float)(width - SEPARATOR) * weight;
		separatorBounds.z  = separatorBounds.x + SEPARATOR;
		break;
	default:
		assert(false);
	}

	const bool focused = (RECT_CONTAINS_POINT(separatorBounds, GetMousePosition()) || RECT_CONTAINS_POINT(separatorBounds, GetLastMousePosition()));
	const bool clicked = focused && GetMouseLeftButton();

	if (clicked) {
		switch (orientation) {
		case GUI_VERTICAL :
			weight = (float)(GetMousePosition().y - SEPARATOR / 2 - absoluteBounds.y) / (float)(height - SEPARATOR);
			break;
		case GUI_HORIZONTAL:
			weight = (float)(GetMousePosition().x - SEPARATOR / 2 - absoluteBounds.x) / (float)(width - SEPARATOR);
			break;
		}
		weight = std::clamp<float>(weight, 0.01f, 0.99f);
	}
	guiDrawQuad(separatorBounds, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);

	return ans;
}

// ScrollPanel components
inline void ScrollPanelClient(ivec4& bounds) {
	bounds = guiLayoutGetAbsoluteBounds(true);
}

inline void ScrollPanelHorizontal(ivec4& clientBounds, int32_t width, int* offset) {
	static const int HEIGHT = 18;

	const ivec4 bounds = guiLayoutGetBounds(false);
	const float visibleWidth = bounds.z - bounds.x;
	const float diff = width - visibleWidth;

	if ((diff > HEIGHT) && (offset != NULL)) {
		Panel(FixSplitLayout(GUI_VERTICAL, -HEIGHT, 0, 0)) {
			ScrollPanelClient(clientBounds);

			float progress = (float)*offset / diff;
			if (Scrollbar(progress, diff / (float)width, GUI_HORIZONTAL)) {
				*offset = progress * diff;
			}
		}
	} else {
		ScrollPanelClient(clientBounds);
	}
}

inline void ScrollPanelVertical(ivec4& clientBounds, int32_t width, int32_t height, int* offsetX, int* offsetY) {
	static const int WIDTH = 16;

	const ivec4 bounds = guiLayoutGetBounds(false);
	const float visibleHeight = bounds.w - bounds.y;
	const float diff = height - visibleHeight;

	if ((diff > WIDTH) && (offsetY != NULL)) {
		Panel(FixSplitLayout(GUI_HORIZONTAL, -WIDTH, 0, 0)) {
			ScrollPanelHorizontal(clientBounds, width, offsetX);

			float progress = 1.0f - (float)*offsetY / diff;
			if (Scrollbar(progress, diff / (float)height, GUI_VERTICAL)) {
				*offsetY = (1.0f - progress) * diff;
			}
		}

		if (RECT_CONTAINS_POINT(clientBounds, GetMousePosition())) {
			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				// TODO: Check ALT to update offsetX ?
				*offsetY = std::clamp<int32_t>(*offsetY + (diff / 10) * mouseWheelDelta, 0, diff);
			}
		}
	} else {
		ScrollPanelHorizontal(clientBounds, width, offsetX);

		if (offsetX && RECT_CONTAINS_POINT(clientBounds, GetMousePosition())) {
			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				const float visibleWidth = bounds.z - bounds.x;
				const float diff = width - visibleWidth;
				*offsetX = std::clamp<int32_t>(*offsetX + (diff / 10) * mouseWheelDelta, 0, diff);
			}
		}
	}
}

Layout guiBeginScrollPanel(int width, int height, int* offsetX, int* offsetY, uint32_t margin, uint32_t flags) {
	Layout ans = guiBeginPanel();
	const ivec4 bounds = guiLayoutGetBounds(false);
	ivec4 clientBounds = {};

	ScrollPanelVertical(clientBounds, width, height, offsetX, offsetY);

	RectClip(clientBounds, __imgui_context->clip);
	__imgui_context->clip = clientBounds;

	__imgui_context->viewport.x = clientBounds.x - (offsetX ? *offsetX : 0);
	__imgui_context->viewport.y = clientBounds.y - (offsetY ? *offsetY : 0);
	__imgui_context->viewport.z = __imgui_context->viewport.x + (width == 0 ? bounds.z - bounds.x : width);
	__imgui_context->viewport.w = __imgui_context->viewport.y + (height == 0 ? bounds.w - bounds.y : height);

	return ans;
}

Layout guiBeginTabPanel(const char* names, int& selected, uint32_t margin, uint32_t flags) {
	uint32_t count = (*names != '\0');
	for(const char* ptr = names, *begin = ptr; *ptr !='\0'; ++ptr) {
		if (*ptr == ',') {
			if (ptr - begin) {
				count += 1;
			}
			begin = ptr + 1;
		}
	}

	ivec4 absoluteRectangle = guiLayoutGetBounds(false);
	const int32_t height = absoluteRectangle.w - absoluteRectangle.y;
	const int32_t tabHeight = std::min(absoluteRectangle.w - absoluteRectangle.y, (int)__imgui_context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	const float tabHeightProc = (float)tabHeight / (float)height;
	Layout ans = guiBeginPanel(SplitLayout(GUI_VERTICAL, tabHeightProc, 0, 0));
	if (count == 0) {
		ans.run_statement = false;
	}
	if (ans.run_statement == false) {
		return ans;
	}

	Panel(GridLayout(count, 1, 0)) {
		int index = 0;
		const char* begin = names;
		const char* ptr = names;
		char tmp[128]; // TODO: implemented button with length string
		do {
			switch (*ptr) {
			case ',' :
			case '\0' :
				if (ptr - begin) {
					size_t length = (size_t)(ptr - begin);
					strncpy(tmp, begin, std::min(length, sizeof(tmp)));
					tmp[length] = 0;
					if (Button(tmp, GUI_FLAGS_BUTTON | ((index == selected) ? GUI_CLICKED | GUI_FOCUSED : GUI_NONE))) {
						selected = index;
					}
				}
				++index;
				begin = ptr + 1;
				break;
			}
		} while (*ptr++ != '\0');
	}

	absoluteRectangle = guiLayoutGetAbsoluteBounds(false);
	guiDrawBorder(absoluteRectangle, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	guiDrawQuad(absoluteRectangle, __imgui_context->skin.colors[GUI_COLOR_PANEL]);

	return ans;
}

Layout guiBeginWindow(ivec4* rbounds, const char* title, const char* footer, uint32_t margin, uint32_t* flags) {
	assert(__imgui_context != NULL);

	const int32_t TitlebarHeight = __imgui_context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT];
	// ivec4 bounds = guiLayoutGetAbsoluteBounds(*rbounds, true);
	ivec4 bounds = *rbounds;
	ivec4 contentBounds = bounds;

	uint32_t defaultFlags = GUI_FLAGS_WINDOW  & ~GUI_WINDOW_CLOSE;
	Layout ans = guiBeginPanel();
	__imgui_context->viewport = bounds;

	if (flags == NULL) {
		flags = & defaultFlags;
	} else if ((*flags & GUI_VISIBLE) == 0) {
		ans.run_statement = false;
		guiEndPanel(&ans);
		return ans;
	}

	if (*flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANEL]);
	}

	if (*flags & GUI_WINDOW_TITLEBAR) {
		const int height = bounds.w - bounds.y;
		const int width  = bounds.z - bounds.x;

		Panel(FixSplitLayout(GUI_VERTICAL, TitlebarHeight, 0, 0)) {
			// Top element
			Panel(FixSplitLayout(GUI_HORIZONTAL, -TitlebarHeight, 0, 0)) {
				// Left element
				ivec4 titlebarBounds = guiLayoutGetAbsoluteBounds(false);
				guiDrawQuad(titlebarBounds, __imgui_context->skin.colors[GUI_COLOR_TITLEBAR]);
				if (Movable(titlebarBounds)) {
					bounds = {titlebarBounds.x, titlebarBounds.y, titlebarBounds.x + width, titlebarBounds.y + height};
				}

				Label(title, GUI_VISIBLE | GUI_FOREGROUND | GUI_ALIGN_CENTER);

				// Right element
				if ((*flags & GUI_WINDOW_CLOSE) && (Button(GUI_ICON_CLOSE))) {
					*flags &= ~GUI_VISIBLE;
				}
			}

			// Bottom element
			contentBounds = guiLayoutGetAbsoluteBounds(true);
		}
	} else {
		// TODO
	}
	__imgui_context->viewport = contentBounds;

	if (*flags & GUI_WINDOW_SIZE) {
		const int32_t width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
		const int32_t height = __imgui_context->viewport.w - __imgui_context->viewport.y;
		Panel(FixSplitLayout(GUI_VERTICAL, height - TitlebarHeight, 0, 0)) {
			// Top element
			contentBounds = guiLayoutGetAbsoluteBounds(true);

			// Bottom element
			Panel(FixSplitLayout(GUI_HORIZONTAL, width - TitlebarHeight, 0)) {
				Label(footer == nullptr ? "" : footer);

				ivec4 sizeBounds = {bounds.z - TitlebarHeight, bounds.w - TitlebarHeight, bounds.z, bounds.w}; // LayoutGetAbsoluteBounds(true)
				if (Movable(sizeBounds)) {
					bounds.z = sizeBounds.z;
					bounds.w = sizeBounds.w;

					// Enforce smallest width and height values to maintain proper functionality
					if (bounds.z - bounds.x < TitlebarHeight) {
						bounds.z = bounds.x + TitlebarHeight;
					}
					if (bounds.w - bounds.y < TitlebarHeight * 2) {
						bounds.w = bounds.y + TitlebarHeight * 2;
					}
				}

				const bool focused = RECT_CONTAINS_POINT(sizeBounds, GetMousePosition());
				const bool clicked = focused && GetMouseLeftButton();
				guiDrawIcon(GUI_ICON_SIZE, sizeBounds, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE], GUI_NONE);
			}
		}
		__imgui_context->viewport = contentBounds;
	}

	if (*flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		__imgui_context->viewport.x += 1;
		__imgui_context->viewport.y += 1;
		__imgui_context->viewport.z -= 1;
		__imgui_context->viewport.w -= 1;
	}
	// guiDrawQuad(__imgui_context->viewport, {255, 255, 255, 255});

	*rbounds = bounds;
	return ans;
}

void guiSetLayout(const Layout& layout) {
	__imgui_context->layout = layout;
	__imgui_context->layout.elementIndex = 0;
	switch (__imgui_context->layout.type) {
	case GUI_LAYOUT_BORDER :
		break;
	case GUI_LAYOUT_GRID:
		__imgui_context->layout.grid.size.x = (__imgui_context->viewport.z - __imgui_context->viewport.x) / __imgui_context->layout.grid.count.x;
		__imgui_context->layout.grid.size.y = (__imgui_context->viewport.w - __imgui_context->viewport.y) / __imgui_context->layout.grid.count.y;
		break;
	case GUI_LAYOUT_SPLIT :
		if (__imgui_context->layout.split.weight < 0) {
			__imgui_context->layout.split.weight = 1.0f + __imgui_context->layout.split.weight;
		}
		break;
	}
}

Layout* guiGetLayout() {
	return &__imgui_context->layout;
}

#if defined (IMGUI_EXT_WINDOW_MANAGER)

void WMBringIndexToFront(int32_t selected) {
	if (selected >= 0 && selected < (int32_t)__imgui_context->windowManager.count) {
		const WindowInfo tmp = __imgui_context->windowManager.windows[selected];
		for (int32_t index = selected; index < (int32_t)__imgui_context->windowManager.count - 1; ++index) {
			__imgui_context->windowManager.windows[index] = __imgui_context->windowManager.windows[index + 1];
		}
		__imgui_context->windowManager.windows[__imgui_context->windowManager.count - 1] = tmp;
	}
}

int32_t WMGetIndexByID(int32_t id) {
	for (uint32_t index = 0; index < __imgui_context->windowManager.count; ++index) {
		if (__imgui_context->windowManager.windows[index].id == id) {
			return index;
		}
	}
	return -1;
}

bool WMRegister(ivec4* bounds, uint32_t* flags, int32_t id) {
	if (__imgui_context != nullptr && __imgui_context->windowManager.count < WM_WINDOW_COUNT) {
		__imgui_context->windowManager.windows[__imgui_context->windowManager.count].bounds = bounds;
		__imgui_context->windowManager.windows[__imgui_context->windowManager.count].flags = flags;
		__imgui_context->windowManager.windows[__imgui_context->windowManager.count].id = (id == -1) ? __imgui_context->windowManager.count : id;
		__imgui_context->windowManager.count += 1;
		return true;
	}
	return false;
}

bool WMUnregister(int32_t id) {
	int32_t windowIndex = WMGetIndexByID(id);
	if (windowIndex < 0) {
		return false;
	}
	for (uint32_t index = windowIndex; index < __imgui_context->windowManager.count - 1; ++index) {
		__imgui_context->windowManager.windows[index] = __imgui_context->windowManager.windows[index + 1];
	}
	__imgui_context->windowManager.count--;
	return true;
}


void WMBringIDToFront(int32_t id) {
	WMBringIndexToFront(WMGetIndexByID(id));
}

void WMOnCursorEvent(int32_t x, int32_t y) {
	bool receiveEvents = true;
	if (!__imgui_context->mouseButtonLeft || !__imgui_context->lastMouseButtonLeft) 
	for (int32_t index = __imgui_context->windowManager.count - 1; index >= 0; --index) {
		if (((*__imgui_context->windowManager.windows[index].flags) & GUI_VISIBLE) && 
			(RECT_CONTAINS_POINT(*__imgui_context->windowManager.windows[index].bounds, __imgui_context->mousePosition))&&
			(RECT_CONTAINS_POINT(*__imgui_context->windowManager.windows[index].bounds, __imgui_context->lastMousePosition))) {
			__imgui_context->windowManager.windows[index].receiveEvents = receiveEvents;
			receiveEvents = false;
		} else {
			__imgui_context->windowManager.windows[index].receiveEvents = false;
		}
	}
	guiOnCursorEvent(x, y);
}

void WMOnButtonEvent(int32_t button, int32_t value) {
	guiOnButtonEvent(button, value);

	if ((value != GUI_BUTTON_PRESSED)) {
		return;
	}

	if (__imgui_context->windowManager.modal >= 0) {
		const int32_t index = WMGetIndexByID(__imgui_context->windowManager.modal);
		if (index >= 0 && index <= (int32_t)__imgui_context->windowManager.count) {
			if ((*__imgui_context->windowManager.windows[index].flags) & GUI_VISIBLE) {
				if (!RECT_CONTAINS_POINT(*__imgui_context->windowManager.windows[index].bounds, __imgui_context->mousePosition)) {
					if (__imgui_context->windowManager.flags & WMF_MODAL_LOCK) {
						printf("\a"); // terminal alert
						return;
					} else {
						(*__imgui_context->windowManager.windows[index].flags) &= ~GUI_VISIBLE;
						__imgui_context->windowManager.modal = -1;
					}
				}
			} else {
				__imgui_context->windowManager.modal = -1;
			}
		}
	}
	bool receiveEvents = true;
	for (int32_t index = __imgui_context->windowManager.count - 1; index >= 0; --index) {
		if (((*__imgui_context->windowManager.windows[index].flags) & GUI_VISIBLE) 
			&& (RECT_CONTAINS_POINT(*__imgui_context->windowManager.windows[index].bounds, __imgui_context->mousePosition))) {
			__imgui_context->windowManager.windows[index].receiveEvents = receiveEvents;
			receiveEvents = false;
		} else {
			__imgui_context->windowManager.windows[index].receiveEvents = false;
		}
	}
	
	for (int32_t index = __imgui_context->windowManager.count - 1; index >= 0; --index) {
		if (((*__imgui_context->windowManager.windows[index].flags) & GUI_VISIBLE) && (RECT_CONTAINS_POINT(*__imgui_context->windowManager.windows[index].bounds, __imgui_context->mousePosition))) {
			WMBringIndexToFront(index);
			break;
		}
	}
}
#endif // IMGUI_EXT_WINDOW_MANAGER

#undef MIN
#undef MIN2
#undef MIN3
#undef RECT_CONTAINS_POINT

#endif // IMGUI_IMPLEMENTATION
#endif /* __GUI_H__ */
