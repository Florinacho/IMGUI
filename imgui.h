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

#define GUI_LAYOUT_INVALID   0x0  // Not a valid layout. Used to indicate an error
#define GUI_LAYOUT_NONE      0x1  // No space partiioning applied
#define GUI_LAYOUT_GRID_H    0x2  // Split the space into a column major grid
#define GUI_LAYOUT_GRID_V    0x3  // Split the space into a row major grid
#define GUI_LAYOUT_SPLIT_H   0x4  // Split the space horizontaly
#define GUI_LAYOUT_SPLIT_V   0x5  // Split the space vertically
#define GUI_LAYOUT_BORDER_H  0x6  // Split the space into 3 horizontal slots
#define GUI_LAYOUT_BORDER_V  0x7  // Split the space into 3 vertical slots

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
#define GUI_FLAGS_CHECKBOX       (GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER)
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
	bool contains(const ivec2& point) const {
		return ((point.x >= x) && (point.x < z) && (point.y >= y) && (point.y < w));
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
			uint8_t separator;
		} split;
		struct {
			float weight[3];
		} border;
	};
	ivec4 backup_clip;
	ivec4 backup_viewport;
	ivec2 max;
	int32_t elementIndex;
	uint8_t margin;
	uint8_t type;
} Layout;

struct GUIContext;
typedef void  (*DrawLineProc)(GUIContext*, const ivec2&, const ivec2&, const color_t&);
typedef void  (*DrawQuadProc)(GUIContext*, const ivec4&, const color_t&);
typedef void  (*DrawCharProc)(GUIContext*, char c, float pos_x, float pos_y, const color_t& color);
typedef void  (*DrawTextProc)(GUIContext*, const char*, const ivec4&, const color_t&, uint32_t);
typedef void  (*DrawIconProc)(GUIContext*, int32_t, const ivec4&, const color_t&, uint32_t);
typedef ivec2 (*IconSizeProc)(GUIContext*, int32_t);
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

typedef struct WindowManager {
	struct WindowInfo {
		ivec4* bounds;
		uint32_t* flags;
		int32_t id;
		bool receiveEvents;
	} windows[WM_WINDOW_COUNT];

	uint32_t count;
	uint32_t flags;
	int32_t modal;
} WindowManager;

#endif // IMGUI_EXT_WINDOW_MANAGER

typedef struct GUIContext {
	Layout layout;
	struct GUISkin {
		color_t colors[GUI_COLOR_COUNT];
		uint8_t values[GUI_VALUE_COUNT];
	} skin;
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

	struct KeyEvent {
		uint32_t value;
		uint8_t type;
	} keyEvents[GUI_MAX_KEY_EVENT_COUNT];
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
ivec2 guiGetCharSize(char c);
ivec2 guiGetTextSize(const char* text, uint32_t carrot);

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
Layout GridLayout(uint32_t x, uint32_t y, uint8_t orientation = GUI_HORIZONTAL, uint32_t margin = 2);
void guiSetLayout(const Layout& layout);
Layout* guiGetLayout();
ivec4 guiGetAbsoluteBounds(bool advance = true);

void dummyElement(uint32_t count = 1);


#define GUIFrame(...)      for(int32_t __tmp = guiBeginFrame(__VA_ARGS__);    !__tmp; (__tmp += 1), guiEndFrame())
#define PANEL(...)         for(Layout ___tmp = beginPanel(__VA_ARGS__);       ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define SPLIT_PANEL(...)   for(Layout ___tmp = beginSplitPanel(__VA_ARGS__);  ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define TAB_PANEL(...)     for(Layout ___tmp = beginTabPanel(__VA_ARGS__);    ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define SCROLL_PANEL(...)  for(Layout ___tmp = beginScrollPanel(__VA_ARGS__); ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
#define WINDOW(...)        for(Layout ___tmp = beginWindow(__VA_ARGS__);      ___tmp.type != GUI_LAYOUT_INVALID; guiEndPanel(&___tmp))
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
		if (begin.x == end.x) {
			if (__imgui_context->clip.x < begin.x && __imgui_context->clip.z > begin.x) {
				__imgui_context->drawLine(__imgui_context, {begin.x, std::max(begin.y, __imgui_context->clip.y)}, {end.x, std::min(end.y, __imgui_context->clip.w)}, color);
			}
		} else if (begin.y == end.y) {
			if (__imgui_context->clip.y < begin.y && __imgui_context->clip.w > begin.y) {
				__imgui_context->drawLine(__imgui_context, {std::max(begin.x, __imgui_context->clip.x), begin.y}, {std::min(end.x, __imgui_context->clip.z), end.y}, color);
			}
		}
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

ivec2 guiGetCharSize(char c) {
	return __imgui_context->charSize(__imgui_context, c);
}

ivec2 guiGetTextSize(const char* text, uint32_t carrot) {
	return __imgui_context->textSize(__imgui_context, text, carrot);
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
	return bounds.contains(GetMousePosition());
}

inline bool WasFocused(const ivec4& bounds) {
	return bounds.contains(GetLastMousePosition());
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

ivec4 guiGetBounds(bool advance) {
	const int32_t width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
	const int32_t height = __imgui_context->viewport.w - __imgui_context->viewport.y;

	ivec4 ans;

	switch (__imgui_context->layout.type) {
	case GUI_LAYOUT_INVALID :
		return ans;
	case GUI_LAYOUT_NONE :
		ans.x = 0;
		ans.y = 0;
		ans.z = ans.x + width;
		ans.w = ans.y + height;
		break;
	case GUI_LAYOUT_SPLIT_H :
		{
		const int32_t index = __imgui_context->layout.elementIndex % 2;
		const int32_t leftWidth  = float(width - __imgui_context->layout.split.separator) * __imgui_context->layout.split.weight + 0.5f;
		const int32_t rightWidth =  width - __imgui_context->layout.split.separator - leftWidth;
		ans.x = 0 +         (leftWidth  + __imgui_context->layout.split.separator) * index;
		ans.y = 0;
		ans.z = leftWidth + (rightWidth + __imgui_context->layout.split.separator) * index;
		ans.w = height;
		}
		break;
	case GUI_LAYOUT_SPLIT_V :
		{
		const int32_t index = __imgui_context->layout.elementIndex % 2;
		const int32_t topHeight = float(height - __imgui_context->layout.split.separator) * __imgui_context->layout.split.weight + 0.5f;
		const int32_t botHeight = height - __imgui_context->layout.split.separator - topHeight;
		ans.x = 0;
		ans.y = 0 +         (topHeight + __imgui_context->layout.split.separator) * index;
		ans.z = width;
		ans.w = topHeight + (botHeight + __imgui_context->layout.split.separator) * index;
		}
		break;
	case GUI_LAYOUT_BORDER_H :
		switch(__imgui_context->layout.elementIndex % 3) {
		case 0 : // header
			ans.x = 0;
			ans.y = 0;
			ans.z =         float(width) * __imgui_context->layout.border.weight[0] + 0.5f;
			ans.w = height;
			break;
		case 1 : // body
			ans.x =         float(width) * __imgui_context->layout.border.weight[0] + 0.5f;
			ans.y = 0;
			ans.z = ans.x + float(width) * __imgui_context->layout.border.weight[1] + 0.5f;
			ans.w = height;
			break;
		case 2 : // footer
			ans.x = width - float(width) * __imgui_context->layout.border.weight[2] + 0.5f;
			ans.y = 0;
			ans.z = width;
			ans.w = height;
			break;
		}
		break;
	case GUI_LAYOUT_BORDER_V :
		switch(__imgui_context->layout.elementIndex % 3) {
		case 0 : // header
			ans.x = 0;
			ans.y = 0;
			ans.z = width;
			ans.w = 0 +      float(height) * __imgui_context->layout.border.weight[0] + 0.5f;
			break;
		case 1 : // body
			ans.x = 0;
			ans.y = 0 +      float(height) * __imgui_context->layout.border.weight[0] + 0.5f;
			ans.z = width;
			ans.w = ans.y +  float(height) * __imgui_context->layout.border.weight[1] + 0.5f;
			break;
		case 2 : // footer
			ans.x = 0;
			ans.y = height - float(height) * __imgui_context->layout.border.weight[2] + 0.5f;
			ans.z = width;
			ans.w = height;
			break;
		}
		break;
	case GUI_LAYOUT_GRID_H:
		{
		int32_t indexX = (__imgui_context->layout.elementIndex % __imgui_context->layout.grid.count.x) % __imgui_context->layout.grid.count.x;
		int32_t indexY = (__imgui_context->layout.elementIndex / __imgui_context->layout.grid.count.x) % __imgui_context->layout.grid.count.y;
		ans.x = (indexX + 0) * __imgui_context->layout.grid.size.x,
		ans.y = (indexY + 0) * __imgui_context->layout.grid.size.y,
		ans.z = (indexX + 1) * __imgui_context->layout.grid.size.x,
		ans.w = (indexY + 1) * __imgui_context->layout.grid.size.y;
		}
		break;
	case GUI_LAYOUT_GRID_V:
		{
		int32_t indexX = (__imgui_context->layout.elementIndex / __imgui_context->layout.grid.count.y) % __imgui_context->layout.grid.count.x;
		int32_t indexY = (__imgui_context->layout.elementIndex % __imgui_context->layout.grid.count.y) % __imgui_context->layout.grid.count.y;
		ans.x = (indexX + 0) * __imgui_context->layout.grid.size.x,
		ans.y = (indexY + 0) * __imgui_context->layout.grid.size.y,
		ans.z = (indexX + 1) * __imgui_context->layout.grid.size.x,
		ans.w = (indexY + 1) * __imgui_context->layout.grid.size.y;
		}
		break;
	default :
		assert(!"Invalid branch!");
	}

	// Apply margins
	ans.x += __imgui_context->layout.margin;
	ans.y += __imgui_context->layout.margin;
	ans.z -= __imgui_context->layout.margin;
	ans.w -= __imgui_context->layout.margin;

	if (__imgui_context->layout.type != GUI_LAYOUT_NONE) {
		// Center element
		const float midX = float(ans.x + ans.z) / 2.0f;
		const float midY = float(ans.w + ans.y) / 2.0f;
		const float nwidth  = std::min<float>(ans.z - ans.x, __imgui_context->layout.max.x) / 2.0f;
		const float nheight = std::min<float>(ans.w - ans.y, __imgui_context->layout.max.y) / 2.0f;
		ans.x = midX - nwidth;
		ans.y = midY - nheight;
		ans.z = midX + nwidth;
		ans.w = midY + nheight;
	}

	__imgui_context->layout.elementIndex += int(advance);

	return ans;
}

ivec4 guiGetAbsoluteBounds(bool advance) {
	ivec4 ans = guiGetBounds(advance);
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

Layout GridLayout(uint32_t x, uint32_t y, uint8_t orientation, uint32_t margin) {
	Layout ans = {};
	ans.type = (orientation == GUI_VERTICAL ? GUI_LAYOUT_GRID_V : GUI_LAYOUT_GRID_H);
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
	ans.type = (orientation == GUI_VERTICAL ? GUI_LAYOUT_SPLIT_V : GUI_LAYOUT_SPLIT_H);
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.split.separator = separator;
	ans.split.weight = weight;
	return ans;
}

Layout FixSplitLayout(uint8_t orientation, int32_t size, uint32_t separator, uint32_t margin) {
	const ivec4 bounds = guiGetBounds(false);
	
	Layout ans = {};
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.split.separator = separator;

	if (orientation == GUI_VERTICAL) {
		ans.type = GUI_LAYOUT_SPLIT_V;
		ans.split.weight = std::min<float>(float(size) / float(bounds.w - bounds.y),  1.0f);
	} else {
		ans.type = GUI_LAYOUT_SPLIT_H;
		ans.split.weight = std::min<float>(float(size) / float(bounds.z - bounds.x),  1.0f);
	}

	return ans;
}

Layout BorderLayout(uint8_t orientation, float headerWeight, float footerWeight, uint32_t margin) {
	assert((headerWeight + footerWeight) <= 1.0f);

	Layout ans = {};
	ans.type = (orientation == GUI_VERTICAL ? GUI_LAYOUT_BORDER_V : GUI_LAYOUT_BORDER_H);
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.border.weight[0] = headerWeight;
	ans.border.weight[1] = 1.0f - (headerWeight + footerWeight);
	ans.border.weight[2] = footerWeight;
	return ans;
}

void dummyElement(uint32_t count) {
	__imgui_context->layout.elementIndex += count;
}

void guiSetLayout(const Layout& layout) {
	__imgui_context->layout = layout;
	__imgui_context->layout.elementIndex = 0;
	switch (__imgui_context->layout.type) {
	case GUI_LAYOUT_BORDER_H :
	case GUI_LAYOUT_BORDER_V :
		break;
	case GUI_LAYOUT_GRID_H:
	case GUI_LAYOUT_GRID_V:
		__imgui_context->layout.grid.size.x = (__imgui_context->viewport.z - __imgui_context->viewport.x) / __imgui_context->layout.grid.count.x;
		__imgui_context->layout.grid.size.y = (__imgui_context->viewport.w - __imgui_context->viewport.y) / __imgui_context->layout.grid.count.y;
		break;
	case GUI_LAYOUT_SPLIT_H :
	case GUI_LAYOUT_SPLIT_V :
		if (__imgui_context->layout.split.weight < 0) {
			__imgui_context->layout.split.weight = 1.0f + __imgui_context->layout.split.weight;
		}
		break;
	}
}

void guiEndPanel(Layout* bkp_layout) {
	__imgui_context->layout = *bkp_layout;
	__imgui_context->viewport = __imgui_context->layout.backup_viewport;
	__imgui_context->clip = __imgui_context->layout.backup_clip;

	bkp_layout->type = GUI_LAYOUT_INVALID;
}
Layout* guiGetLayout() {
	return &__imgui_context->layout;
}

/****************** TEMPLATE TEST BEGIN ******************/
// ================ Panel ================
using DrawPanelProc = void (*) (const ivec4&, uint32_t);

inline void DefaultDrawPanel(const ivec4& bounds, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANEL]);
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template <DrawPanelProc drawPanel = DefaultDrawPanel>
Layout beginPanel(const Layout& layout = AbsoluteLayout(), uint32_t padding = 0, uint32_t flags = GUI_FLAGS_PANEL) {
	__imgui_context->layout.backup_viewport = __imgui_context->viewport;
	__imgui_context->layout.backup_clip = __imgui_context->clip;

	__imgui_context->viewport = guiGetAbsoluteBounds(true);
	const int32_t width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
	const int32_t height = __imgui_context->viewport.w - __imgui_context->viewport.y;

	Layout ans = __imgui_context->layout;
	if ((width < (int32_t)padding * 2) || (height < (int32_t)padding * 2)) {
		return { .type = GUI_LAYOUT_INVALID, };
	}

	ans = __imgui_context->layout;

	__imgui_context->viewport.x += padding;
	__imgui_context->viewport.y += padding;
	__imgui_context->viewport.z -= padding;
	__imgui_context->viewport.w -= padding;

	// TODO ?
	if (RectGetArea(__imgui_context->clip) > 0) {
		__imgui_context->clip = {
			std::max(__imgui_context->clip.x, __imgui_context->viewport.x),
			std::max(__imgui_context->clip.y, __imgui_context->viewport.y),
			std::min(__imgui_context->clip.z + 1, __imgui_context->viewport.z),
			std::min(__imgui_context->clip.w + 1, __imgui_context->viewport.w)
		};
	} else {
		__imgui_context->clip = __imgui_context->viewport;
	}

	guiSetLayout(layout);

	drawPanel(__imgui_context->viewport, flags);

	return ans;
}

// ================ Label ================
using LabelDrawProc = void (*) (const ivec4&, const char*, uint32_t);

inline void DefaultDrawLabel(const ivec4& bounds, const char* text, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
	}

	if (flags & GUI_FOREGROUND) {
		guiDrawText(text, bounds, __imgui_context->skin.colors[GUI_COLOR_TEXT], (flags & GUI_ALIGN_CENTER));
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template<LabelDrawProc drawLabel = DefaultDrawLabel>
void label(const char* text, uint32_t flags = GUI_FLAGS_LABEL) {
	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);

	drawLabel(absoluteBounds, text, flags);
}

// ================ Icon Label ================
using IconLabelDrawProc = void (*) (const ivec4&, int, uint32_t);

inline void DefaultDrawIconLabel(const ivec4& bounds, int icon, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
	}

	if (flags & GUI_FOREGROUND) {
		guiDrawIcon(icon, bounds, __imgui_context->skin.colors[GUI_COLOR_TEXT], (flags & GUI_ALIGN_CENTER));
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template<IconLabelDrawProc drawLabel = DefaultDrawIconLabel>
void label(int icon, uint32_t flags = GUI_FLAGS_LABEL) {
	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);

	drawLabel(absoluteBounds, icon, flags);
}

// ================ Icon Button ================
using IconButtonDrawProc = void(*)(const ivec4& bounds, int icon, uint32_t flags);

inline void DefaultDrawIconButton(const ivec4& bounds, int icon, uint32_t flags) {
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			if (flags & GUI_CLICKED) {
				guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_ACTIVE]);
			} else if (flags & GUI_FOCUSED) {
				guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_FOCUSED]);
			} else {
				guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
			}
		}

		if (flags & GUI_OUTLINE) {
			guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}

		if (flags & GUI_FOREGROUND) {
			guiDrawIcon(icon, bounds, __imgui_context->skin.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
		}
	}
}

template<IconButtonDrawProc draw = DefaultDrawIconButton>
bool button(int icon, uint32_t flags = GUI_FLAGS_BUTTON) {
	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);

	if ((absoluteBounds.contains(GetMousePosition()) && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED)) {
		flags |= (GetMouseLeftButton() ? GUI_CLICKED : GUI_FOCUSED);
	}

	draw(absoluteBounds, icon, flags);

	return ((flags & GUI_FOCUSED) && !GetMouseLeftButton() && GetLastMouseLeftButton());
}

// ================ SplitPanel ================
template <DrawPanelProc drawPanel = DefaultDrawPanel, IconButtonDrawProc drawButton = DefaultDrawIconButton>
Layout beginSplitPanel(uint8_t orientation, float& weight, uint32_t padding, uint32_t flags) {
	static const int32_t SEPARATOR = 8;
	static const float THRESHOLD_MIN = 0.01f;
	static const float THRESHOLD_MAX = 0.99f;

	const Layout ans = beginPanel<drawPanel>(SplitLayout(orientation, weight, SEPARATOR), padding, flags);
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

	const bool focused = (separatorBounds.contains(GetMousePosition()) || separatorBounds.contains(GetLastMousePosition()));
	const bool clicked = focused && GetMouseLeftButton();

	if (clicked) {
		switch (orientation) {
		case GUI_VERTICAL :
			weight = float(GetMousePosition().y - SEPARATOR / 2 - absoluteBounds.y) / float(height - SEPARATOR);
			break;
		case GUI_HORIZONTAL:
			weight = float(GetMousePosition().x - SEPARATOR / 2 - absoluteBounds.x) / float(width - SEPARATOR);
			break;
		}
		weight = std::clamp<float>(weight, THRESHOLD_MIN, THRESHOLD_MAX);
	}
	// guiDrawQuad(separatorBounds, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	uint32_t separatorFlags = GUI_VISIBLE | GUI_BACKGROUND | GUI_OUTLINE;
	if (focused) {
		separatorFlags |= GUI_FOCUSED;
	}
	if (clicked) {
		separatorFlags |= GUI_CLICKED;
	}
	drawButton(separatorBounds, 0, separatorFlags);

	return ans;
}


// ================ Button ================
using ButtonDrawProc = void(*)(const ivec4& bounds, const char* text, uint32_t flags);

inline void DefaultDrawButton(const ivec4& bounds, const char* text, uint32_t flags) {
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			if (flags & GUI_CLICKED) {
				guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_ACTIVE]);
			} else if (flags & GUI_FOCUSED) {
				guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_FOCUSED]);
			} else {
				guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
			}
		}

		if (flags & GUI_OUTLINE) {
			guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		}

		if (flags & GUI_FOREGROUND) {
			guiDrawText(text, bounds, __imgui_context->skin.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
		}
	}
}

template<ButtonDrawProc draw = DefaultDrawButton>
bool button(const char* text, uint32_t flags = GUI_FLAGS_BUTTON) {
	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);

	if ((absoluteBounds.contains(GetMousePosition()) && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED)) {
		flags |= (GetMouseLeftButton() ? GUI_CLICKED : GUI_FOCUSED);
	}

	draw(absoluteBounds, text, flags);

	return ((flags & GUI_FOCUSED) && !GetMouseLeftButton() && GetLastMouseLeftButton());
}

// ================ CheckBox ================
using CheckBoxDrawProc = void (*) (const ivec4&, uint32_t);

inline void DefaultDrawCheckBox(const ivec4& bounds, uint32_t flags) {
	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[(flags & GUI_FOCUSED) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}

	if ((flags & GUI_FOREGROUND) && (flags & GUI_CLICKED)) {
		guiDrawIcon(GUI_ICON_CHECK, bounds, __imgui_context->skin.colors[GUI_COLOR_ACTIVE], flags); // {bounds.x + 1, bounds.y + 1, bounds.z - 1, bounds.w - 1}
	}
}

template <CheckBoxDrawProc drawCheckBox = DefaultDrawCheckBox>
bool checkBox(bool &checked, uint32_t flags = GUI_FLAGS_CHECKBOX) {
	bool ans = false;

	__imgui_context->layout.max = {24, 24};
	const ivec4 bounds = guiGetAbsoluteBounds(true);
	__imgui_context->layout.max = {0x1FFFFFFF, 0x1FFFFFFF};

	if (flags & GUI_ENABLED) {
		if (bounds.contains(GetMousePosition())) {
			flags |= GUI_FOCUSED;

			if (!GetMouseLeftButton() && GetLastMouseLeftButton()) {
				checked = !checked;
				ans = true;
			}
		}
	}

	if (checked) {
		flags |= GUI_CLICKED;
	}

	drawCheckBox(bounds, flags);

	return ans;
}

// ================ Toggle ================
using ToggleDrawProc = void (*)(const ivec4&, uint32_t);

inline void DefaultDrawToggle(const ivec4& bounds, uint32_t flags) {
	static const int32_t PADDING = 2;

	const int32_t halfWidth = (bounds.z - bounds.x) / 2;

	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[(flags & GUI_FOCUSED) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}

	if (flags & GUI_FOREGROUND) {
		bool clicked = (flags & GUI_CLICKED);
		const ivec4 buttonBounds = {
			bounds.x + halfWidth *  clicked + PADDING * !clicked, 
			bounds.y + PADDING, 
			bounds.z - halfWidth * !clicked - PADDING *  clicked, 
			bounds.w - PADDING
		};

		guiDrawQuad(buttonBounds, __imgui_context->skin.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_PANE]);
		guiDrawBorder(buttonBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template <ToggleDrawProc drawToggle = DefaultDrawToggle>
bool toggle(bool& clicked, uint32_t flags = GUI_FLAGS_CHECKBOX) {
	__imgui_context->layout.max = {40, 22};
	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);
	__imgui_context->layout.max = {0x1FFFFFFF, 0x1FFFFFFF};

	bool ans = false;

	if (flags & GUI_ENABLED) {
		if (absoluteBounds.contains(GetMousePosition())) {
			flags |= GUI_FOCUSED;

			if (!GetMouseLeftButton() && GetLastMouseLeftButton()) {
				clicked = !clicked;
				ans = true;
			}
		}
	}

	if (clicked) {
		flags |= GUI_CLICKED;
	}

	drawToggle(absoluteBounds, flags);

	return ans;
}

// ================ ProgressBar ================
using ProgressBarDrawProc = void (*)(const ivec4&, float, int32_t, uint32_t);

inline void DefaultDrawProgressBar(const ivec4& bounds, float progress, int32_t padding, uint32_t flags) {
	const int32_t width  = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;

	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad(bounds, __imgui_context->skin.colors[GUI_COLOR_PANE]);
	}

	if ((flags & GUI_FOREGROUND) && (width > padding) && (height > padding)) {
		const ivec4 barBounds = {
			bounds.x + padding,
			bounds.y + padding,
			bounds.x + padding + int32_t((float)(width - padding * 2) * progress),
			bounds.w - padding,
		};

		guiDrawQuad(barBounds, __imgui_context->skin.colors[GUI_COLOR_ACTIVE]);
		guiDrawBorder(barBounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template <ProgressBarDrawProc drawProgressBar = DefaultDrawProgressBar>
void progressBar(float progress, int32_t padding = 2, uint32_t flags = GUI_FLAGS_PROGRESSBAR) {
	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);
	
	drawProgressBar(absoluteBounds, progress, padding, flags);
}

// ================ Spinner ================
template<IconButtonDrawProc buttonDraw = DefaultDrawIconButton, LabelDrawProc labelDraw = DefaultDrawLabel>
bool spinnerInternal(int& value, const char* text, int step, uint32_t flags) {
	static const uint32_t LABEL_FLAG_MASK  = GUI_VISIBLE | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_MASK;
	static const uint32_t BUTTON_FLAG_MASK = GUI_VISIBLE | GUI_ENABLED;
	static const uint32_t BUTTON_FLAGS = GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
	static const uint32_t button_flags = BUTTON_FLAGS | (flags & BUTTON_FLAG_MASK); // Customizable label flags

	const ivec4 bounds = guiGetAbsoluteBounds(false);
	const int32_t height = bounds.w - bounds.y;
	// const int32_t buttonWidth = std::min((bounds.z - bounds.x) / 2, (int)__imgui_context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	const float buttonWidthProc = std::min<float>(float(height)/ (float)(bounds.z - bounds.x), 0.5f);
	const int oldValue = value;

	const bool focused = bounds.contains(GetMousePosition());// && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED));
	if (focused) {
		int32_t mouseWheelDelta = 0;
		if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
			value -= mouseWheelDelta * step;
		}
	}
	// __imgui_context->layout.max.y = 24;
	PANEL(BorderLayout(GUI_HORIZONTAL, buttonWidthProc, buttonWidthProc, 0)) {
		value -= button<buttonDraw>(GUI_ICON_ARROW_LEFT,  button_flags); // GUI_ICON_ARROW_LEFT
		label<labelDraw>(text, flags & LABEL_FLAG_MASK);
		value += button<buttonDraw>(GUI_ICON_ARROW_RIGHT, button_flags); // GUI_ICON_ARROW_RIGHT
	}
	// __imgui_context->layout.max.y = 0x1FFFFFFF;

	return (oldValue != value);
}

template<IconButtonDrawProc buttonDraw = DefaultDrawIconButton, LabelDrawProc labelDraw = DefaultDrawLabel>
bool spinner(int& value, int step = 1, uint32_t flags = GUI_FLAGS_SPINNER) {
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "%d", value);

	return spinnerInternal<buttonDraw, labelDraw>(value, tmp, step, flags);
}

template<IconButtonDrawProc buttonDraw = DefaultDrawIconButton, LabelDrawProc labelDraw = DefaultDrawLabel>
bool spinner(int& value, const char** labels, uint32_t count, int step = 1, uint32_t flags = GUI_FLAGS_SPINNER) {
	if (spinnerInternal<buttonDraw, labelDraw>(value, labels[std::clamp<int>(value, 0, count - 1)], step, flags)) {
		value = std::clamp<int>(value, 0, count - 1);
		return true;
	}

	return false;
}

// ================ TextBox ================
using TextBoxDrawProc = void (*) (const ivec4&, const char*, const ivec2&, uint32_t, uint32_t);

inline void DefaultDrawTextBox(const ivec4& absoluteBounds, const char* text, const ivec2& carrotPosition, uint32_t time, uint32_t flags) {
	const int32_t CARROT_WIDTH = 1;
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
		guiDrawQuad(bounds, __imgui_context->skin.colors[(flags & GUI_FOCUSED) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}

	if (flags & GUI_FOREGROUND) {
		if (flags & GUI_HIDDEN) {
			// TODO: DrawIcon(GUI_ICON_CLOSE) ?
		} else {
			guiDrawText(text, bounds, __imgui_context->skin.colors[GUI_COLOR_TEXT], (flags & (GUI_MULTILINE | GUI_ALIGN_CENTER)));
		}

		if ((flags & GUI_CLICKED) && (time % 2 == 0)) {
			guiDrawQuad({carrotPosition.x, carrotPosition.y, carrotPosition.x + CARROT_WIDTH, carrotPosition.y + carrotHeight}, __imgui_context->skin.colors[GUI_COLOR_TEXT]);
		}
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(bounds, __imgui_context->skin.colors[(flags & GUI_CLICKED) ? GUI_COLOR_ACTIVE : (flags & GUI_FOCUSED) ? GUI_COLOR_FOCUSED : GUI_COLOR_BORDER]);
	}

}

template <TextBoxDrawProc drawTextBox = DefaultDrawTextBox>
bool textBox(char* text, const uint32_t max_length, int& carrot, uint32_t flags = GUI_FLAGS_TEXTBOX, uint32_t padding = 2) {
	// Pan the viewport until the carrot is within the visible viewport
	// Returns the string offset required
	auto panViewport = [](const char* text, uint32_t carrot, uint32_t width) -> int {
		int offset = 0;
		uint32_t positionX = guiGetTextSize(text, carrot).x;

		while (positionX > width) {
			positionX -= guiGetCharSize(text[offset++]).x; // positionX < 0 ?
		}

		return offset;
	};

	// Process ket events and manipulate the text or carrot possion.
	// Returns the type of event processed
	auto processKeyEvent = [](char* text, uint32_t max_length, int32_t length, uint32_t flags, int32_t& carrot, const auto& event) -> int {
		enum KeyEventResponse {
			KEY_NONE,
			KEY_MOVE,
			KEY_CHAR,
		};

		int ans = KEY_NONE;

		switch (event.type) {
		case GUI_EVENT_KEY_DOWN :
			if (event.value == __imgui_context->keyMap[GUI_KEY_HOME]) {
				carrot = MoveCursor(text, carrot, CUR_POS_BEGIN);
				ans = KEY_MOVE;
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_END]) {
				carrot = MoveCursor(text, carrot, CUR_POS_END);
				ans = KEY_MOVE;
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_UP]) {
				carrot = MoveCursor(text, carrot, CUR_POS_PREV_LINE);
				ans = KEY_MOVE;
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_DOWN]) {
				carrot = MoveCursor(text, carrot, CUR_POS_NEXT_LINE);
				ans = KEY_MOVE;
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_LEFT]) {
				carrot = std::max(carrot - 1, 0);
				ans = KEY_MOVE;
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_RIGHT]) {
				carrot = std::min(carrot + 1, length);
				ans = KEY_MOVE;
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_BACK]) {
				if (carrot > 0) {
					for (int32_t index = --carrot; index < length - 1; ++index) {
						text[index] = text[index + 1];
					}
					text[--length] = '\0';
					ans = KEY_CHAR;
				}
			} else if (event.value == __imgui_context->keyMap[GUI_KEY_DELETE]) {
				if (carrot < length) {
					for (int32_t index = carrot; index < length - 1; ++index) {
						text[index] = text[index + 1];
					}
					text[--length] = '\0';
					ans = KEY_CHAR;
				}
			}
			break;
		case GUI_EVENT_CHAR :
			switch (event.value) {
			case '\n' : 
				if ((flags & GUI_MULTILINE) == 0) {
					break;
				}
				[[fallthrough]];
			default :
				if (length < (int)max_length - 1) {
					for (int32_t index = ++length; index >= carrot; --index) {
						text[index] = text[index - 1];
					}
					text[carrot++] = (char)event.value;
					ans = KEY_CHAR;
				}
			}
			break;
		}

		return ans;
	};

	static time_t lastBlinkTime = 0;

	const ivec4 absoluteBounds = guiGetAbsoluteBounds(true);
	const int32_t width = absoluteBounds.z - absoluteBounds.x;
	const int32_t lineHeight = guiGetCharSize(' ').y;
	const int32_t offsetY = GetLineYOffset(text, absoluteBounds, flags);

	int32_t offsetX = 0;
	ivec2 carrotPosition = {0, 0};
	int32_t length = strlen(text);
	int32_t offset = 0;
	bool ans = false;

	// Offset text to keep carrot in viewport
	if (carrot > 0) {
		offset = panViewport(text, carrot, width);
	}

	// Process mouse events
	if (flags & GUI_ENABLED) {
		if (absoluteBounds.contains(GetMousePosition(true))) {
			flags |= GUI_FOCUSED;
		}

		if (GetLastMouseLeftButton(true) && (GetMouseLeftButton(true) == false)) {
			if (flags & GUI_FOCUSED) {
				// Jump lines
				int jumpLineCount = (GetMousePosition().y - offsetY) / lineHeight;
				for (char* ptr = text; (*ptr != '\0') && (jumpLineCount > 0); ++ptr, ++offset) {
					switch (*ptr) {
					case '\n' :
						--jumpLineCount;
						break;
					}
				}
				// Jump chars
				offsetX = ((flags & GUI_MULTILINE) ? GetLineXOffset(text + offset, absoluteBounds, flags) : absoluteBounds.x);
				int32_t pos_x = 0;
				for (carrot = offset; (carrot < length && text[carrot] != '\n'); ++carrot){ 
					int32_t charWidth = guiGetCharSize(text[carrot]).x;
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

	if (carrot >= 0) {
		flags |= GUI_CLICKED;

		// Process key events
		for (uint32_t index = 0; index < __imgui_context->keyEventCount; ++index) {
			if (int state = processKeyEvent(text, max_length, length, flags, carrot, __imgui_context->keyEvents[index]) > 0) {
				lastBlinkTime = time(NULL);
				ans = (state == 2);
			}
		}

		// Calculate carrot relative position in viewport
		for (int index = 0; index < std::max(carrot - offset, 0) && (text[index] != '\0'); ++index) {
			switch (text[index]) {
			case '\n' :
				carrotPosition.x = 0;
				carrotPosition.y += 16;
				break;
			default :
				carrotPosition.x += guiGetCharSize(text[index]).x;
				break;
			}
		}
		offsetX = ((flags & GUI_MULTILINE) ? GetLineXOffset(text + MoveCursor(text, carrot, CUR_POS_BEGIN), absoluteBounds, flags) : absoluteBounds.x);
	}

	drawTextBox(absoluteBounds, text + offset, {carrotPosition.x + offsetX, carrotPosition.y + offsetY}, time(NULL) - lastBlinkTime, flags);

	return ans;
}

// ================ Horizontal Slider ================
using SliderDrawProc = void (*)(const ivec4&, const ivec4&, float, uint32_t);

inline void DefaultDrawHorizontalSlider(const ivec4& bounds, const ivec4& box, float prog, uint32_t flags) {
	static const int32_t DEFAULT_THICKNESS = 4;

	const int32_t height = (bounds.w - bounds.y);
	const int32_t middle = (bounds.w + bounds.y) / 2;
	const int32_t halfThickness = std::min(height, DEFAULT_THICKNESS) / 2;

	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		guiDrawQuad({bounds.x, middle - halfThickness, box.x, middle + halfThickness}, __imgui_context->skin.colors[(flags & GUI_CLICKED) ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
		guiDrawQuad({box.z, middle - halfThickness, bounds.z, middle + halfThickness}, __imgui_context->skin.colors[GUI_COLOR_PANE]);
	}

	if (flags & GUI_FOREGROUND) {
		guiDrawQuad(box, __imgui_context->skin.colors[(flags & GUI_CLICKED) ? GUI_COLOR_ACTIVE : (flags & GUI_FOCUSED) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(box, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template <SliderDrawProc drawHorizontalSlider = DefaultDrawHorizontalSlider>
bool horizontalSlider(float& proc, float step = 0.1f, float boxProc = 0, uint32_t flags = GUI_FLAGS_SLIDER) {
	const ivec4 bounds = guiGetAbsoluteBounds(true);
	const int32_t width  = bounds.z - bounds.x;
	const int32_t boxLength = (boxProc != 0) ? boxProc * width : __imgui_context->skin.values[GUI_VALUE_SLIDER_WIDTH];
	const int32_t k = (float)(width - boxLength) * proc;
	const ivec4 boxBounds = {
		bounds.x + k, 
		bounds.y, 
		bounds.x + k + boxLength, 
		bounds.w
	};

	bool ans = false;

	if (flags & GUI_ENABLED) {
		// Focused
		if (bounds.contains(GetMousePosition()) || bounds.contains(GetLastMousePosition())) {
			flags |= GUI_FOCUSED;

			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				proc = std::clamp(proc - step * (float)mouseWheelDelta, 0.0f, 1.0f);
				ans = true;
			}

			// Clicked
			if (GetMouseLeftButton()) {
				flags |= GUI_CLICKED;

				const float newValue = std::clamp((float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength), 0.0f, 1.0f);
				if (newValue != proc) { // CHANGE_THRESHOLD
					proc = newValue;
					ans = true;
				}
			}
		}
	}

	drawHorizontalSlider(bounds, boxBounds, proc, flags);

	return ans;
}

// ================ Vertical Slider ================
inline void DefaultDrawVerticalSlider(const ivec4& bounds, const ivec4& box, float prog, uint32_t flags) {
	static const int32_t DEFAULT_THICKNESS = 4;

	const int32_t width  = (bounds.z - bounds.x);
	const int32_t middle = (bounds.z + bounds.x) / 2;
	const int32_t halfThickness = std::min(width, DEFAULT_THICKNESS) / 2;

	if ((flags & GUI_VISIBLE) == 0) {
		return;
	}

	if (flags & GUI_BACKGROUND) {
		// Draw active bar
		guiDrawQuad({middle - halfThickness, box.w, middle + halfThickness, bounds.w}, __imgui_context->skin.colors[(flags & GUI_CLICKED) ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
		// Draw passive/inactive bar
		guiDrawQuad({middle - halfThickness, bounds.y, middle + halfThickness, box.y}, __imgui_context->skin.colors[GUI_COLOR_PANE]);
	}

	if (flags & GUI_FOREGROUND) { // Draw active box
		guiDrawQuad(box, __imgui_context->skin.colors[(flags & GUI_CLICKED) ? GUI_COLOR_ACTIVE : (flags & GUI_FOCUSED) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}

	if (flags & GUI_OUTLINE) {
		guiDrawBorder(box, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
	}
}

template <SliderDrawProc drawVerticalSlider = DefaultDrawVerticalSlider>
bool verticalSlider(float& proc, float step = 0.1f, float boxProc = 0, uint32_t flags = GUI_FLAGS_SLIDER) {
	const ivec4 bounds = guiGetAbsoluteBounds(true);
	const int32_t length  = bounds.w - bounds.y;
	const int32_t boxLength = (boxProc != 0) ? boxProc * length : __imgui_context->skin.values[GUI_VALUE_SLIDER_WIDTH];

	const int32_t k = float(length - boxLength) * proc;
	const ivec4 boxBounds = {
		bounds.x,
		bounds.w - k - boxLength, 
		bounds.z, 
		bounds.w - k,
	};

	bool ans = false;

	if (flags & GUI_ENABLED) {
		// Focused
		if (bounds.contains(GetMousePosition()) || bounds.contains(GetLastMousePosition())) {
			flags |= GUI_FOCUSED;

			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				proc = std::clamp(proc - step * (float)mouseWheelDelta, 0.0f, 1.0f);
				ans = true;
			}

			// Clicked
			if (GetMouseLeftButton()) {
				flags |= GUI_CLICKED;

				const float newValue = std::clamp(1.0f - (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(length - boxLength), 0.0f, 1.0f);
				if (newValue != proc) { // CHANGE_THRESHOLD
					proc = newValue;
					ans = true;
				}
			}
		}
	}

	drawVerticalSlider(bounds, boxBounds, proc, flags);

	return ans;
}

// ================ Slider ================
template <SliderDrawProc horizontalSliderDraw = DefaultDrawHorizontalSlider, SliderDrawProc verticalSliderDraw = DefaultDrawVerticalSlider>
inline bool slider(float& proc, uint8_t orientation = GUI_HORIZONTAL, float step = 0.1f, float boxProc = 0, uint32_t flags = GUI_FLAGS_SLIDER) {
	switch (orientation) {
	case GUI_HORIZONTAL : 
		return horizontalSlider<horizontalSliderDraw>(proc, step, boxProc, flags);
	case GUI_VERTICAL : 
		return verticalSlider<verticalSliderDraw>(proc, step, boxProc, flags);
	}
	return false;
}

// ================ Horizontal ScrollBar ================
template <IconButtonDrawProc buttonDraw = DefaultDrawIconButton, SliderDrawProc horizontalSliderDraw = DefaultDrawHorizontalSlider>
bool horizontalScrollBar(float& value, float barProc, float step = 0.1f, uint32_t flags = GUI_FLAGS_SLIDER) {
	const ivec4 bounds = guiGetBounds(false);
	const int width = bounds.z - bounds.x;
	const int height = bounds.w - bounds.y;
	const float buttonProc = std::min<float>(float(height) / (float)width, 0.5f);

	bool ans = false;

	PANEL(BorderLayout(GUI_HORIZONTAL, buttonProc, buttonProc, 0), 0, GUI_FLAGS_PANEL) {
		if (button<buttonDraw>(GUI_ICON_ARROW_LEFT)) {
			value = std::max<float>(value - step, 0.0f);
			ans = true;
		}
		if (horizontalSlider<horizontalSliderDraw>(value, step, barProc, flags)) {
			ans = true;
		}
		if (button<buttonDraw>(GUI_ICON_ARROW_RIGHT)) {
			value = std::min<float>(value + step, 1.0f);
			ans = true;
		}
	}

	return ans;
}

// ================ Vertical ScrollBar ================
template <IconButtonDrawProc buttonDraw = DefaultDrawIconButton, SliderDrawProc verticalSliderDraw = DefaultDrawVerticalSlider>
bool verticalScrollBar(float& value, float barProc, float step = 0.1f, uint32_t flags = GUI_FLAGS_SLIDER) {
	const ivec4 bounds = guiGetBounds(false);
	const int width = bounds.z - bounds.x;
	const int height = bounds.w - bounds.y;
	const float buttonProc = std::min<float>(float(width) / (float)height, 0.5f);

	bool ans = false;

	PANEL(BorderLayout(GUI_VERTICAL, buttonProc, buttonProc, 0), 0, GUI_FLAGS_PANEL) {
		if (button<buttonDraw>(GUI_ICON_ARROW_UP)) {
			value = std::max<float>(value - step, 0.0f);
			ans = true;
		}
		value = 1.0f - value;
		if (verticalSlider<verticalSliderDraw>(value, step, barProc, flags)) {
			ans = true;
		}
		value = 1.0f - value;
		if (button<buttonDraw>(GUI_ICON_ARROW_DOWN)) {
			value = std::min<float>(value + step, 1.0f);
			ans = true;
		}
	}

	return ans;
}

// ================ ScrollBar ================
template <IconButtonDrawProc buttonDraw = DefaultDrawIconButton, SliderDrawProc horizontalSliderDraw = DefaultDrawHorizontalSlider, SliderDrawProc verticalSliderDraw = DefaultDrawVerticalSlider>
inline bool scrollBar(float& value, float barProc, uint8_t orientation = GUI_HORIZONTAL, float step = 0.1f) {
	switch (orientation) {
	case GUI_HORIZONTAL :
		return horizontalScrollBar<buttonDraw, horizontalSliderDraw>(value, barProc, step);
	case GUI_VERTICAL :
		return verticalScrollBar<buttonDraw, verticalSliderDraw>(value, barProc, step);
	}

	return false;
}

// ================ ScrollPanel ================
template <IconButtonDrawProc drawButton = DefaultDrawIconButton, SliderDrawProc drawSlider = DefaultDrawHorizontalSlider>
inline void scrollPanelHorizontal(ivec4& clientBounds, int32_t width, int* offset) {
	static const int HEIGHT = 16;

	const ivec4 bounds = guiGetBounds(false);
	const float visibleWidth = bounds.z - bounds.x;
	const float diff = width - visibleWidth;

	if ((diff > HEIGHT) && (offset != NULL)) {
		PANEL(FixSplitLayout(GUI_VERTICAL, -HEIGHT, 0, 0)) {
			clientBounds = guiGetAbsoluteBounds(true);

			float progress = (float)*offset / diff;
			if (horizontalScrollBar<drawButton, drawSlider>(progress, diff / (float)width)) {
				*offset = progress * diff;
			}
		}
	} else {
		clientBounds = guiGetAbsoluteBounds(true);
	}
}

template <IconButtonDrawProc drawButton = DefaultDrawIconButton, SliderDrawProc drawHorizontalSlider = DefaultDrawHorizontalSlider, SliderDrawProc drawVerticalSlider = DefaultDrawVerticalSlider>
inline void scrollPanelVertical(ivec4& clientBounds, int32_t width, int32_t height, int* offsetX, int* offsetY) {
	static const int WIDTH = 16;

	const ivec4 bounds = guiGetBounds(false);
	const float visibleHeight = bounds.w - bounds.y;
	const float diff = height - visibleHeight;

	if ((diff > WIDTH) && (offsetY != NULL)) {
		PANEL(FixSplitLayout(GUI_HORIZONTAL, -WIDTH, 0, 0)) {
			scrollPanelHorizontal<drawButton, drawHorizontalSlider>(clientBounds, width, offsetX);

			PANEL(FixSplitLayout(GUI_VERTICAL, -WIDTH, 0, 0)) { //
				float progress = (float)*offsetY / diff;
				if (verticalScrollBar<drawButton, drawVerticalSlider>(progress, diff / (float)height)) {
					*offsetY = (progress) * diff;
				}
			}
		}

		if (clientBounds.contains(GetMousePosition())) {
			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				// TODO: Check ALT to update offsetX ?
				*offsetY = std::clamp<int32_t>(*offsetY + (diff / 10) * mouseWheelDelta, 0, diff);
			}
		}
	} else {
		scrollPanelHorizontal<drawButton, drawHorizontalSlider>(clientBounds, width, offsetX);

		if (offsetX && clientBounds.contains(GetMousePosition())) {
			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				const float visibleWidth = bounds.z - bounds.x;
				const float diff = width - visibleWidth;
				*offsetX = std::clamp<int32_t>(*offsetX + (diff / 10) * mouseWheelDelta, 0, diff);
			}
		}
	}
}

template <
	DrawPanelProc drawPanel = DefaultDrawPanel,
	IconButtonDrawProc drawButton = DefaultDrawIconButton, 
	SliderDrawProc drawHorizontalSlider = DefaultDrawHorizontalSlider, 
	SliderDrawProc drawVerticalSlider = DefaultDrawVerticalSlider>
Layout beginScrollPanel(int width, int height, int* offsetX, int* offsetY, uint32_t margin, uint32_t flags) {
	Layout ans = beginPanel<drawPanel>();
	const ivec4 bounds = guiGetBounds(false);
	ivec4 clientBounds = {};

	scrollPanelVertical<drawButton, drawHorizontalSlider, drawVerticalSlider>(clientBounds, width, height, offsetX, offsetY);

	RectClip(clientBounds, __imgui_context->clip);
	__imgui_context->clip = clientBounds;

	__imgui_context->viewport.x = clientBounds.x - (offsetX ? *offsetX : 0);
	__imgui_context->viewport.y = clientBounds.y - (offsetY ? *offsetY : 0);
	__imgui_context->viewport.z = __imgui_context->viewport.x + (width == 0 ? bounds.z - bounds.x : width);
	__imgui_context->viewport.w = __imgui_context->viewport.y + (height == 0 ? bounds.w - bounds.y : height);

	return ans;
}

// ================ TabPanel ================
template <DrawPanelProc drawPanel = DefaultDrawPanel, ButtonDrawProc drawButton = DefaultDrawButton>
Layout beginTabPanel(const char* names, int& selected, uint32_t margin, uint32_t flags) {
	uint32_t count = (*names != '\0');
	for(const char* ptr = names, *begin = ptr; *ptr !='\0'; ++ptr) {
		if (*ptr == ',') {
			if (ptr - begin) {
				count += 1;
			}
			begin = ptr + 1;
		}
	}

	ivec4 absoluteRectangle = guiGetBounds(false);
	const int32_t height = absoluteRectangle.w - absoluteRectangle.y;
	const int32_t tabHeight = std::min(absoluteRectangle.w - absoluteRectangle.y, (int)__imgui_context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	const float tabHeightProc = (float)tabHeight / (float)height;
	Layout ans = beginPanel<drawPanel>(SplitLayout(GUI_VERTICAL, tabHeightProc, 0, 0));
	if ((count == 0) || (ans.type == GUI_LAYOUT_INVALID)) {
		return ans;
	}

	PANEL(GridLayout(count, 1, GUI_HORIZONTAL, 0)) {
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
					if (button<drawButton>(tmp, GUI_FLAGS_BUTTON | ((index == selected) ? GUI_CLICKED | GUI_FOCUSED : GUI_NONE))) {
						selected = index;
					}
				}
				++index;
				begin = ptr + 1;
				break;
			}
		} while (*ptr++ != '\0');
	}

	absoluteRectangle = guiGetAbsoluteBounds(false);
	drawPanel(absoluteRectangle, flags);

	return ans;
}

// ================ Window ================
template <DrawPanelProc drawPanel = DefaultDrawPanel, LabelDrawProc drawLabel = DefaultDrawLabel, IconButtonDrawProc drawButton = DefaultDrawIconButton>
Layout beginWindow(ivec4* rbounds, const char* title, const char* footer, uint32_t margin, uint32_t* flags) {
	assert(__imgui_context != NULL);

	const int32_t TitlebarHeight = __imgui_context->skin.values[GUI_VALUE_TITLEBAR_HEIGHT];
	ivec4 bounds = *rbounds;
	ivec4 contentBounds = bounds;

	uint32_t defaultFlags = GUI_FLAGS_WINDOW  & ~GUI_WINDOW_CLOSE;
	__imgui_context->viewport = bounds;

	if (flags == NULL) {
		flags = & defaultFlags;
	}

	Layout ans = beginPanel<drawPanel>(AbsoluteLayout(), 0, *flags);

	if ((*flags & GUI_VISIBLE) == 0) {
		guiEndPanel(&ans);
		return ans;
	}

	if (*flags & GUI_WINDOW_TITLEBAR) {
		const int height = bounds.w - bounds.y;
		const int width  = bounds.z - bounds.x;

		PANEL(FixSplitLayout(GUI_VERTICAL, TitlebarHeight, 0, 0)) {
			// Top element
			PANEL(FixSplitLayout(GUI_HORIZONTAL, -TitlebarHeight, 0, 0)) {
				// Left element
				ivec4 titlebarBounds = guiGetAbsoluteBounds(false);
				if (Movable(titlebarBounds)) {
					bounds = {titlebarBounds.x, titlebarBounds.y, titlebarBounds.x + width, titlebarBounds.y + height};
				}

				label<drawLabel>(title, GUI_VISIBLE | GUI_BACKGROUND | GUI_FOREGROUND | GUI_ALIGN_CENTER);

				// Right element
				if ((*flags & GUI_WINDOW_CLOSE) && (button<drawButton>(GUI_ICON_CLOSE))) {
					*flags &= ~GUI_VISIBLE;
				}
			}

			// Bottom element
			contentBounds = guiGetAbsoluteBounds(true);
		}
	} else {
		// TODO
	}
	__imgui_context->viewport = contentBounds;

	if (*flags & GUI_WINDOW_SIZE) {
		const int32_t width  = __imgui_context->viewport.z - __imgui_context->viewport.x;
		const int32_t height = __imgui_context->viewport.w - __imgui_context->viewport.y;
		PANEL(FixSplitLayout(GUI_VERTICAL, height - TitlebarHeight, 0, 0)) {
			// Top element
			contentBounds = guiGetAbsoluteBounds(true);

			// Bottom element
			PANEL(FixSplitLayout(GUI_HORIZONTAL, width - TitlebarHeight, 0)) {
				label<drawLabel>(footer == nullptr ? "" : footer);

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

				label<DefaultDrawIconLabel>(GUI_ICON_SIZE);

				// if (RECT_CONTAINS_POINT(sizeBounds, GetMousePosition())) {
				// 	*flags |= GUI_FOCUSED;
				// 	if (GetMouseLeftButton()) {
				// 		*flags |= GUI_CLICKED;
				// 	}
				// }
			}
		}
		__imgui_context->viewport = contentBounds;
	}

	if (*flags & GUI_OUTLINE) {
		// guiDrawBorder(bounds, __imgui_context->skin.colors[GUI_COLOR_BORDER]);
		__imgui_context->viewport.x += 1;
		__imgui_context->viewport.y += 1;
		__imgui_context->viewport.z -= 1;
		__imgui_context->viewport.w -= 1;
	}
	
	// drawWindow(bounds, title, footer, *flags);

	*rbounds = bounds;
	return ans;
}

/******************* TEMPLATE TEST END *******************/

#if defined (IMGUI_EXT_WINDOW_MANAGER)

void WMBringIndexToFront(int32_t selected) {
	if (selected >= 0 && selected < (int32_t)__imgui_context->windowManager.count) {
		const auto tmp = __imgui_context->windowManager.windows[selected];
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
			((*__imgui_context->windowManager.windows[index].bounds).contains(__imgui_context->mousePosition))&&
			((*__imgui_context->windowManager.windows[index].bounds).contains(__imgui_context->lastMousePosition))) {
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
				if (!(*__imgui_context->windowManager.windows[index].bounds).contains(__imgui_context->mousePosition)) {
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
			&& ((*__imgui_context->windowManager.windows[index].bounds).contains(__imgui_context->mousePosition))) {
			__imgui_context->windowManager.windows[index].receiveEvents = receiveEvents;
			receiveEvents = false;
		} else {
			__imgui_context->windowManager.windows[index].receiveEvents = false;
		}
	}
	
	for (int32_t index = __imgui_context->windowManager.count - 1; index >= 0; --index) {
		if (((*__imgui_context->windowManager.windows[index].flags) & GUI_VISIBLE) && ((*__imgui_context->windowManager.windows[index].bounds).contains(__imgui_context->mousePosition))) {
			WMBringIndexToFront(index);
			break;
		}
	}
}
#endif // IMGUI_EXT_WINDOW_MANAGER

#endif // IMGUI_IMPLEMENTATION

#define IMPLEMENT_FOR_EACH
#define IMPLEMENT_RUN_WITH

#ifdef IMPLEMENT_FOR_EACH
/*
	FOR_EACH(ACTION, a, 11, b, 22, c, 33)
	(GET_MACRO(a, 11, b, 22, c, 33, FE_8, FE_7, FE_6, FE_5, FE_4, FE_3, FE_2, FE_1)) (ACTION, a, 11, b, 22, c, 33) {
		_1 = a
		_2 = 11
		_3 = b
		_4 = 22
		_5 = c
		_6 = 33
		_7 = FE_8
		_8 = FE_7
		MACRO_NAME = FE_6
	}
	FE_6(ACTION, a, 11, b, 22, c, 33)
	ACTION(a, 11)   ACTION(b, 22)   ACTION(c, 33)
*/
#define FE_2(ACTION, X, Y     ) ACTION(X, Y) 
#define FE_4(ACTION, X, Y, ...) ACTION(X, Y)  FE_2(ACTION, __VA_ARGS__)
#define FE_6(ACTION, X, Y, ...) ACTION(X, Y)  FE_4(ACTION, __VA_ARGS__)
#define FE_8(ACTION, X, Y, ...) ACTION(X, Y)  FE_6(ACTION, __VA_ARGS__)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, MACRO_NAME, ...) MACRO_NAME // returns the first parameter after the __VA_ARGS__
#define FOR_EACH(ACTION, ...) GET_MACRO(__VA_ARGS__, FE_8, FE_7, FE_6, FE_5, FE_4, FE_3, FE_2, FE_1)(ACTION, __VA_ARGS__)

#if defined (IMPLEMENT_RUN_WITH)
// Store the even variables on the stack, overwrite with the odd values, restore to initial value
// Restrictions: 
//		even number of parameters that reprezent a lvalue, rvalue tuple
//		4 tuples max(8 params)
// Ex:	int a = 1, b = 2, c = 3, d = 4
//		RUN_WITH(a, 11, b, 22, c, 33, d, 44) { // backup and update
//		} // restore 
#define FE_BACKUP(M, V)  , backup_##M = M
#define FE_UPDATE(M, V)  , (M = V)
#define FE_RESTORE(M, V) , M = backup_##M

inline bool FETrue(...) { return true; }

#define RUN_WITH(...) for (auto run = 1         FOR_EACH(FE_BACKUP,  __VA_ARGS__);  \
	                            run && FETrue(0 FOR_EACH(FE_UPDATE,  __VA_ARGS__)); \
	                            run = 0         FOR_EACH(FE_RESTORE, __VA_ARGS__))

// #define LAYOUT_WITH(M, V) RUN_WITH(guiGetLayout()->M, V)

#endif // IMPLEMENT_RUN_WITH

#endif // IMPLEMENT_FOR_EACH

#endif /* __GUI_H__ */
