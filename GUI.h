#ifndef __GUI_H__
#define __GUI_H__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GUI_LAYOUT_ABSOLUTE 0
#define GUI_LAYOUT_GRID     1
#define GUI_LAYOUT_SPLIT    2

#define GUI_ORIENTATION_VERTICAL   0
#define GUI_ORIENTATION_HORIZONTAL 1
#define GUI_ORIENTATION_AUTO       2

#define GUI_DIRECTION_LEFT   0
#define GUI_DIRECTION_BOTTOM 1
#define GUI_DIRECTION_RIGHT  2
#define GUI_DIRECTION_TOP    3

#define GUI_EVENT_KEY_DOWN 0
#define GUI_EVENT_KEY_UP   1
#define GUI_EVENT_CHAR     2

#define GUI_BUTTON_LEFT   0
#define GUI_BUTTON_MIDDLE 1
#define GUI_BUTTON_RIGHT  2

#define GUI_BUTTON_RELEASED 0
#define GUI_BUTTON_PRESSED  1

#define GUI_KEY_BACK   0
#define GUI_KEY_DELETE 1
#define GUI_KEY_HOME   2
#define GUI_KEY_END    3
#define GUI_KEY_LEFT   4
#define GUI_KEY_RIGHT  5
#define GUI_KEY_COUNT  6

#define GUI_ICON_CLOSE        0
#define GUI_ICON_ARROW_LEFT   1
#define GUI_ICON_ARROW_RIGHT  2
#define GUI_ICON_ARROW_DOWN   3
#define GUI_ICON_ARROW_UP     4
#define GUI_ICON_SIZE         5
#define GUI_ICON_CHECK        6
#define GUI_ICON_CUSTOM       (GUI_ICON_CHECK + 1)

#define GUI_MAX_KEY_EVENT_COUNT 8

// Style colors
#define GUI_COLOR_TEXT            0
#define GUI_COLOR_TEXT_DISABLED   1
#define GUI_COLOR_BORDER          2
#define GUI_COLOR_PANEL           3
#define GUI_COLOR_TITLEBAR        4
#define GUI_COLOR_PANE            5
#define GUI_COLOR_FOCUSED         6
#define GUI_COLOR_ACTIVE          7
#define GUI_COLOR_COUNT           8

// Style variables
#define GUI_VALUE_TEXT_SCALE      0
#define GUI_VALUE_TITLEBAR_HEIGHT 1
#define GUI_VALUE_SCROLLBAR_WIDTH 2
#define GUI_VALUE_SLIDER_WIDTH    3
#define GUI_VALUE_BUTTON_WIDTH    4
#define GUI_VALUE_GROUP_OFFSET    5
#define GUI_VALUE_WINDOW_PADDING  6
#define GUI_VALUE_COUNT           8

#define MIN2(A, B) ((A) < (B) ? (A) : (B))
#define MIN3(A, B, C) MIN2(MIN2((A), (B)), (C))
#define MIN(A, B) MIN2((A), (B))
#define MAX2(A, B) ((A) > (B) ? (A) : (B))
#define MAX3(A, B, C) MAX2(MAX2((A), (B)), (C))
#define MAX(A, B) MAX2((A), (B))
#define CLAMP(VALUE, MIN_VALUE, MAX_VALUE) MIN(MAX((VALUE), (MIN_VALUE)), (MAX_VALUE))
#define RECT_CONTAINS_POINT(RECT, POINT) (((POINT).x >= (RECT).x) && ((POINT).x <= (RECT).z) && ((POINT).y >= (RECT).y) && ((POINT).y <= (RECT).w))

const uint32_t GUI_NONE        = 0;
const uint32_t GUI_VISIBLE     = 1 << 0;
const uint32_t GUI_ENABLED     = 1 << 1;
const uint32_t GUI_BACKGROUND  = 1 << 2;
const uint32_t GUI_FOREGROUND  = 1 << 3;
const uint32_t GUI_OUTLINE     = 1 << 4;
const uint32_t GUI_FOCUSED     = 1 << 5;
const uint32_t GUI_CLICKED     = 1 << 15;

const uint32_t GUI_WINDOW_TITLEBAR  = 1 << 6;
const uint32_t GUI_WINDOW_CLOSE     = 1 << 7;
const uint32_t GUI_WINDOW_MOVE      = 1 << 8;
const uint32_t GUI_WINDOW_SIZE      = 1 << 9;
const uint32_t GUI_WINDOW_DECORATION = GUI_WINDOW_TITLEBAR | GUI_WINDOW_CLOSE;

const uint32_t GUI_HIDDEN           = 1 << 10;

const uint32_t GUI_ALIGN_LEFT   = 1 << 11;
const uint32_t GUI_ALIGN_BOTTOM = 1 << 12;
const uint32_t GUI_ALIGN_RIGHT  = 1 << 13;
const uint32_t GUI_ALIGN_TOP    = 1 << 14;

const uint32_t GUI_ALIGN_LEFT_TOP      = GUI_ALIGN_TOP | GUI_ALIGN_LEFT;
const uint32_t GUI_ALIGN_LEFT_CENTER   = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT;
const uint32_t GUI_ALIGN_LEFT_BOTTOM   = GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT;
const uint32_t GUI_ALIGN_BOTTOM_CENTER = GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_BOTTOM_RIGHT  = GUI_ALIGN_BOTTOM | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_CENTER_RIGHT  = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_TOP_RIGHT     = GUI_ALIGN_TOP | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_TOP_CENTER    = GUI_ALIGN_TOP | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_CENTER        = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;

const uint32_t GUI_ALIGN_MASK = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;

const uint32_t GUI_DEFAULT_FLAGS_LABEL       = GUI_VISIBLE |               GUI_FOREGROUND |                                GUI_ALIGN_CENTER;
const uint32_t GUI_DEFAULT_FLAGS_BUTTON      = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
const uint32_t GUI_DEFAULT_FLAGS_SPINNER     = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND |               GUI_ALIGN_CENTER;
const uint32_t GUI_DEFAULT_FLAGS_CHECKBOX    = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;
const uint32_t GUI_DEFAULT_FLAGS_PROGRESSBAR = GUI_VISIBLE |               GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;
const uint32_t GUI_DEFAULT_FLAGS_SLIDER      = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;
const uint32_t GUI_DEFAULT_FLAGS_TEXTBOX     = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
const uint32_t GUI_DEFAULT_FLAGS_WINDOW      = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_WINDOW_MOVE  | GUI_WINDOW_DECORATION;

typedef struct {
	int32_t x;
	int32_t y;
} ivec2;

typedef struct {
	uint32_t x;
	uint32_t y;
} uvec2;

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t w;
} ivec4;

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t w;
} color;

typedef struct Layout {
	union {
		struct {
			ivec2 count;
			ivec2 cellSize;  // cached data
		} grid;
		struct {
			float weight;
			uint8_t separator;
			bool vertical;
		} split;
	};
	ivec4 parent;
	uvec2 max;
	uint8_t padding[4];
	int32_t elementIndex;
	uint8_t type;
	bool run_statement;
} Layout;

ivec4 LayoutGetCellBounds(const ivec4& bounds, bool advance);
ivec4 LayoutGetAbsoluteBounds(const ivec4& bounds, bool advance);

typedef struct {
	uint32_t value;
	uint8_t type;
} KeyEvent;

struct GUIContext;
typedef void (*DrawLineProc)(GUIContext*, const ivec2&, const ivec2&, const color&);
typedef void (*DrawQuadProc)(GUIContext*, const ivec4&, const color&);
typedef void (*DrawTextProc)(GUIContext*, const char*, const ivec4&, const color&, uint32_t);
typedef void (*DrawIconProc)(GUIContext*, int32_t, const ivec4&, const color&);
typedef void (*DrawBorderProc)(GUIContext*, const ivec4&, const color&);
typedef ivec2 (*GetTextSizeProc)(GUIContext*, const char*, uint32_t);

typedef struct {
	color colors[GUI_COLOR_COUNT];
	uint8_t values[GUI_VALUE_COUNT];
} GUIStyle;

#if defined IMGUI_INCLUDE_WINDOW_MANAGER
typedef struct WindowInfo {
	ivec4* bounds;
	uint32_t* flags;
	int32_t id;
	bool receiveEvents;
} WindowInfo;
#endif // IMGUI_INCLUDE_WINDOW_MANAGER

typedef struct GUIContext {
	GUIStyle style;
	
	void* opaqueData;
	
	ivec2 mousePosition;
	ivec2 lastMousePosition;
	bool mouseButtonLeft;
	bool lastMouseButtonLeft;
	int32_t mouse_wheel_delta;
	bool events_enabled;
	KeyEvent keyEvents[GUI_MAX_KEY_EVENT_COUNT];
	uint32_t keyEventCount;
	
	Layout layout;
	ivec4 viewport;
	
	uint64_t textBoxActivationTime;
	
	DrawLineProc drawLine;
	DrawQuadProc drawQuad;
	DrawTextProc drawText;
	DrawIconProc drawIcon;
	DrawBorderProc drawBorder;
	GetTextSizeProc getTextSize;
	
	uint32_t keyMap[GUI_KEY_COUNT];
	
#if defined IMGUI_INCLUDE_WINDOW_MANAGER
	static const uint32_t MAX_WINDOW_COUNT = 8;
	typedef void (*ModalAlertProc)(void*, const ivec4&);
	
	WindowInfo windows[MAX_WINDOW_COUNT];
	ModalAlertProc modalAlertProc;
	void* modalUserData;
	uint32_t count;
	int32_t modal;
#endif
} GUIContext;

void GUIDrawText(const char* text, const ivec4& bounds, color& color, uint32_t flags);

void GUIDrawLine(const ivec2& begin, const ivec2& end, const color& color);

void GUIDrawQuad(const ivec4& bounds, const color& color);

void GUIDrawIcon(int32_t id, const ivec4& bounds, const color& color);

void GUIContextInit(GUIContext* context);

void GUIOnCursorEvent(int32_t x, int32_t y);

void GUIOnMouseWheelEvent(int32_t delta);

void GUIOnButtonEvent(uint32_t button, bool status);

void GUIOnKeyEvent(uint32_t key, bool status);

void GUIOnCharEvent(char c);

int32_t GUIBeginFrame();

void GUIEndFrame();

void GUISetActiveContext(GUIContext*);

GUIContext* GUIGetActiveContext();

Layout AbsoluteLayout();

Layout SplitLayout(bool vertical, float weight = 0.5f, uint8_t separator = 0);

Layout GridLayout(uint32_t x, uint32_t y);

void SetLayout(const Layout& layout);

Layout* GetLayout();

void DummyElement(uint32_t count = 1);

void Label(const char*, uint32_t = GUI_DEFAULT_FLAGS_LABEL, const ivec4& = {});

void Label(int32_t, uint32_t = GUI_DEFAULT_FLAGS_LABEL, const ivec4& = {});

bool Button(const char*, uint32_t = GUI_DEFAULT_FLAGS_BUTTON, const ivec4& = {});

bool Button(const char*, uint32_t*, const ivec4& = {});

bool Button(int32_t, uint32_t = GUI_DEFAULT_FLAGS_BUTTON, const ivec4& = {});

bool Button(int32_t, uint32_t*, const ivec4& = {});

bool CheckBox(bool&, uint32_t = GUI_DEFAULT_FLAGS_CHECKBOX, const ivec4& = {});

bool Toggle(bool&, uint32_t = GUI_DEFAULT_FLAGS_CHECKBOX, const ivec4& = {});

bool Spinner(int&, uint32_t = GUI_DEFAULT_FLAGS_SPINNER, const ivec4& = {});

bool Spinner(int&, const char**, uint32_t, uint32_t = GUI_DEFAULT_FLAGS_SPINNER, const ivec4& = {});

void ProgressBar(float, uint32_t = GUI_DEFAULT_FLAGS_PROGRESSBAR, const ivec4& = {});

bool Slider(float&, uint32_t = GUI_DEFAULT_FLAGS_SLIDER, const ivec4& = {});

bool RangeSlider(float&, float&, bool = false, uint32_t = GUI_DEFAULT_FLAGS_SLIDER, const ivec4& = {});

bool TextBox(char*, uint32_t, int32_t&, uint32_t = GUI_DEFAULT_FLAGS_TEXTBOX, const ivec4& = {});

bool ListBox(int&, int&, const char**, uint32_t, uint32_t = 0, const ivec4& = {});

bool ScrollBar(int&, int, bool, float = 1.0f, const ivec4& = {});

Layout BeginWindow(ivec4*, const char* = nullptr, uint32_t* = nullptr);

Layout BeginPanel(const Layout& = Layout());

Layout BeginSplitPanel(bool, float&, const ivec4& = ivec4());

Layout BeginTabPanel(const char**, uint32_t, int&, const ivec4& = ivec4());

Layout BeginGroup(const char*, const ivec4& = ivec4());

void EndPanel(Layout*);

#define EndWindow EndPanel

#define EndSplitPanel EndPanel

#define EndTabPanel EndPanel

#define EndGroup EndPanel

#define GUIFrame(context) \
	for(int32_t ___tmp = GUIBeginFrame(context); \
		!___tmp; (___tmp += 1), \
		GUIEndFrame())

#define Panel(new_layout) \
	for(Layout ___tmp = BeginPanel(new_layout); \
		___tmp.run_statement; \
		EndPanel(&___tmp))

#define SplitPanel(vertical, weight) \
	for(Layout ___tmp = BeginSplitPanel(vertical, weight); \
		___tmp.run_statement; \
		EndPanel(&___tmp))

#define TabPanel(names, count, selected) \
	for(Layout ___tmp = BeginTabPanel(names, count, selected); \
		___tmp.run_statement; \
		EndPanel(&___tmp))

#define Group(text) \
	for(Layout ___tmp = BeginGroup(text); \
		___tmp.run_statement; \
		EndPanel(&___tmp))

#define Window(bounds, title, flags) \
	for(Layout ___tmp = BeginWindow(bounds, title, flags); \
		___tmp.run_statement; \
		EndPanel(&___tmp))

static GUIContext* context = NULL;

#if defined (IMGUI_INCLUDE_WINDOW_MANAGER)
static void WMBringIndexToFront(int32_t selected) {
	if (selected >= 0 && selected < context->count) {
		const WindowInfo tmp = context->windows[selected];
		for (int32_t index = selected; index < context->count - 1; ++index) {
			context->windows[index] = context->windows[index + 1];
		}
		context->windows[context->count - 1] = tmp;
	}
}

static int32_t WMGetIndexByID(int32_t id) {
	for (uint32_t index = 0; index < context->count; ++index) {
		if (context->windows[index].id == id) {
			return index;
		}
	}
	return -1;
}

static bool WMRegister(ivec4* bounds, uint32_t* flags, int32_t id = -1) {
	if (context != nullptr && context->count < GUIContext::MAX_WINDOW_COUNT) {
		context->windows[context->count].bounds = bounds;
		context->windows[context->count].flags = flags;
		context->windows[context->count].id = (id == -1) ? context->count : id;
		context->count += 1;
		return true;
	}
	return false;
}

static bool WMUnregister(GUIContext* context, int32_t id) {
	int32_t windowIndex = WMGetIndexByID(id);
	if (windowIndex < 0) {
		return false;
	}
	for (uint32_t index = windowIndex; index < context->count - 1; ++index) {
		context->windows[index] = context->windows[index + 1];
	}
	context->count--;
	return true;
}


static void WMBringIDToFront(int32_t id) {
	WMBringIndexToFront(WMGetIndexByID(id));
}

static void WMOnCursorEvent(int32_t x, int32_t y) {
	bool receiveEvents = true;
	for (int32_t index = context->count - 1; index >= 0; --index) {
		if (((*context->windows[index].flags) & GUI_VISIBLE) && 
			(RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition))) {
			context->windows[index].receiveEvents = receiveEvents;
			receiveEvents = false;
		} else {
			context->windows[index].receiveEvents = false;
		}
	}
	GUIOnCursorEvent(x, y);
}

static void WMOnButtonEvent(int32_t button, int32_t value) {
	if ((button == GUI_BUTTON_LEFT) && (value == GUI_BUTTON_PRESSED)) {
		if (context->modal >= 0) {
			const int32_t index = WMGetIndexByID(context->modal);
			if (index >= 0 && index <= context->count) {
				if ((*context->windows[index].flags) & GUI_VISIBLE) {
					if (!RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition)) {
						if (context->modalAlertProc) {
							context->modalAlertProc(context->modalUserData, *context->windows[index].bounds);
						}
						return;
					}
				} else {
					context->modal = -1;
				}
			}
		}
		bool receiveEvents = true;
		for (int32_t index = context->count - 1; index >= 0; --index) {
			if (((*context->windows[index].flags) & GUI_VISIBLE) && (RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition))) {
				context->windows[index].receiveEvents = receiveEvents;
				receiveEvents = false;
			} else {
				context->windows[index].receiveEvents = false;
			}
		}
		
		for (int32_t index = context->count - 1; index >= 0; --index) {
			if (((*context->windows[index].flags) & GUI_VISIBLE) && (RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition))) {
				WMBringIndexToFront(index);
				break;
			}
		}
	}
	GUIOnButtonEvent(button, value);
}
#endif // IMGUI_INCLUDE_WINDOW_MANAGER

/************************************** IMPLEMENTATION ******************************/
#if defined (IMGUI_IMPLEMENTATION)

void GUIDrawText(const char* text, const ivec4& bounds, color& color, uint32_t flags) {
	if (context != NULL) {
			context->drawText(context, text, bounds, color, flags);
	}
}

void GUIDrawLine(const ivec2& begin, const ivec2& end, const color& color) {
	if (context != NULL) {
		context->drawLine(context, begin, end, color);
	}
}

void GUIDrawQuad(const ivec4& bounds, const color& color) {
	if (context != NULL) {
		context->drawQuad(context, bounds, color);
	}
}

void GUIDrawIcon(int32_t id, const ivec4& bounds, const color& color) {
	if (context != NULL) {
		context->drawIcon(context, id, bounds, color);
	}
}

void GUIDrawBorder(const ivec4& bounds, const color& color) {
	if (context != NULL) {
		context->drawBorder(context, bounds, color);
	}
}

void DrawBorder(GUIContext* context, const ivec4& bounds, const color& color) {
	if (((bounds.w - bounds.y) <= 0) || ((bounds.z - bounds.x) <= 0)) {
		return;
	}
	GUIDrawLine({bounds.x, bounds.y}, {bounds.x, bounds.w}, color);
	GUIDrawLine({bounds.x, bounds.w - 1}, {bounds.z, bounds.w - 1}, color);
	GUIDrawLine({bounds.z - 1, bounds.y}, {bounds.z - 1, bounds.w}, color);
	GUIDrawLine({bounds.x, bounds.y}, {bounds.z, bounds.y}, color);
}

void GUISetActiveContext(GUIContext* ncontext) {
	context = ncontext;
	
	assert(context != NULL);
	assert(context->drawLine != NULL);
	assert(context->drawQuad != NULL);
	assert(context->drawText != NULL);
	assert(context->drawIcon != NULL);
	assert(context->getTextSize != NULL);
}

GUIContext* GUIGetActiveContext() {
	return context;
}

bool GetMouseWheelDelta(int32_t *delta, bool force) {
	if ((context == NULL) || (context->mouse_wheel_delta == 0) || (force == false && context->events_enabled == false) || (context->modal != -1))  {
		return false;
	}
	*delta = context->mouse_wheel_delta;
	return true;
}
#if 0
bool GetMousePosition(ivec2* position, bool force) {
	if (context == NULL || !context->events_enabled  || position == NULL) {
		return false;
	}
	*position = context->mousePosition;
	return true;
}

bool GetLastMousePosition(ivec2& position, bool force) {
	if (context == NULL || !context->events_enabled  || position == NULL) {
		return false;
	}
	*position = context->lastMousePosition;
	return true;
}
bool GetMouseLeftButton(bool force) {
	return (context != NULL && (context->events_enabled || force) && context->mouseButtonLeft);
}

bool GetLastMouseLeftButton(bool force) {
	return (context != NULL && (context->events_enabled || force) && context->lastMouseButtonLeft);
}
#else
ivec2 GetMousePosition(bool force = false) {
	if (context != NULL && (context->events_enabled || force)) {
		return context->mousePosition;
	}
	return {-1, -1};
}
ivec2 GetLastMousePosition(bool force = false) {
	if (context != NULL && (context->events_enabled || force)) {
		return context->lastMousePosition;
	}
	return {-2, -2};
}

bool GetMouseLeftButton(bool force = false) {
	return (context != NULL && (context->events_enabled || force) && context->mouseButtonLeft);
}

bool GetLastMouseLeftButton(bool force = false) {
	return (context != NULL && (context->events_enabled || force) && context->lastMouseButtonLeft);
}
#endif
void GUIContextInit(GUIContext* context) {
	context->layout = AbsoluteLayout();
		
	context->opaqueData = NULL;
	context->drawLine = NULL;
	context->drawQuad = NULL;
	context->drawText = NULL;
	context->drawIcon = NULL;
	context->getTextSize = NULL;
	
	context->drawBorder = DrawBorder;
	
	context->lastMousePosition = {-1, -1};
	context->mousePosition = {-1, -1};
	context->lastMouseButtonLeft = false;
	context->mouseButtonLeft = false;
	context->keyEventCount = 0;
	context->mouse_wheel_delta = 0;
	context->events_enabled = true;
	
	context->textBoxActivationTime = 0;
	
	context->style.colors[GUI_COLOR_TEXT]          = {230, 230, 230, 255};
	context->style.colors[GUI_COLOR_TEXT_DISABLED] = {150, 150, 150, 255};
	context->style.colors[GUI_COLOR_TITLEBAR]      = { 25,  25,  25, 200};
	context->style.colors[GUI_COLOR_PANEL]         = { 50,  50,  50, 255};
	context->style.colors[GUI_COLOR_PANE]          = { 75,  75,  75, 255};
	context->style.colors[GUI_COLOR_FOCUSED]       = {110, 110, 120, 255};
	context->style.colors[GUI_COLOR_ACTIVE]        = { 75, 75, 255, 255};
	context->style.colors[GUI_COLOR_BORDER]        = { 25,  25,  25, 255};
	
	context->style.values[GUI_VALUE_TEXT_SCALE] = 2;
	context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] = context->style.values[GUI_VALUE_TEXT_SCALE] * 10;
	context->style.values[GUI_VALUE_SCROLLBAR_WIDTH] = 16; //context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
	context->style.values[GUI_VALUE_SLIDER_WIDTH] = 7; //context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] / 3;
	context->style.values[GUI_VALUE_GROUP_OFFSET] = 127; // centered
	context->style.values[GUI_VALUE_WINDOW_PADDING] = 4;

#if defined IMGUI_INCLUDE_WINDOW_MANAGER
	context->count = 0;
	context->modal = -1;
	context->modalAlertProc = nullptr;
	context->modalUserData = nullptr;
#endif // IMGUI_INCLUDE_WINDOW_MANAGER
}

void GUIOnCursorEvent(int32_t x, int32_t y) {
	context->mousePosition.x = x;
	context->mousePosition.y = y;
}

void GUIOnMouseWheelEvent(int32_t delta) {
	context->mouse_wheel_delta += delta;
}

void GUIOnButtonEvent(uint32_t button, bool status) {
	if (button == 0) {
		context->mouseButtonLeft = status;
	}
}

void GUIOnKeyEvent(uint32_t key, bool status) {
	if (context->keyEventCount >= GUI_MAX_KEY_EVENT_COUNT) {
		return;
	}
	context->keyEvents[context->keyEventCount++] = {key, GUI_EVENT_KEY_DOWN}; //  : GUI_EVENT_KEY_UP)
}

void GUIOnCharEvent(char c) {
	if (context->keyEventCount >= GUI_MAX_KEY_EVENT_COUNT) {
		return;
	}
	if (c >= 32 && c < 128) {
		context->keyEvents[context->keyEventCount++] = {(uint32_t)c, GUI_EVENT_CHAR};
	}
}

int32_t GUIBeginFrame() {
	/* Empty for now */
	return 0;
}

void GUIEndFrame() {
	context->lastMouseButtonLeft = context->mouseButtonLeft;
	context->lastMousePosition = context->mousePosition;
	context->mouse_wheel_delta = 0;
	context->keyEventCount = 0;
}

ivec4 LayoutGetCellBounds(const ivec4& bounds, bool advance) {
	ivec4 ans;
	switch (context->layout.type) {
	case GUI_LAYOUT_ABSOLUTE :
		if ((bounds.w - bounds.y + bounds.z - bounds.x) != 0) {
			ans = bounds;
		} else {
			ans.x = 0;
			ans.y = 0;
			ans.z = context->viewport.z - context->viewport.x;
			ans.w = context->viewport.w - context->viewport.y;
		}
		break;
	case GUI_LAYOUT_SPLIT :
		{
		const int32_t width  = context->viewport.z - context->viewport.x;
		const int32_t height = context->viewport.w - context->viewport.y;
		const int32_t index = context->layout.elementIndex % 2;
		if (context->layout.split.vertical) {
			const int32_t topCellWidth = (width - context->layout.split.separator) * context->layout.split.weight;
			const int32_t botCellWidth =  width - context->layout.split.separator - topCellWidth;
			ans.x =                (topCellWidth + context->layout.split.separator) * index;
			ans.y = 0;
			ans.z = topCellWidth + (botCellWidth + context->layout.split.separator) * index;
			ans.w = height;
		} else {
			const int32_t topCellHeight = (height - context->layout.split.separator) * context->layout.split.weight;
			const int32_t botCellHeight = height - context->layout.split.separator - topCellHeight;
			ans.x = 0;
			ans.y =                 (topCellHeight + context->layout.split.separator) * index;
			ans.z = width;
			ans.w = topCellHeight + (context->layout.split.separator + botCellHeight) * index;
		}
		if (ans.w < ans.y) {
			ans.w = ans.y;
		}
		{ // TMP: clamp max Y size
			const int32_t mid = (ans.w + ans.y) / 2;
			int32_t height = ans.w - ans.y;
			if (height > context->layout.max.y) {
				height = context->layout.max.y;
			}
			ans.y = mid - height / 2;
			ans.w = mid + height / 2;
		}
		ans.x += context->layout.padding[GUI_DIRECTION_LEFT];
		ans.y += context->layout.padding[GUI_DIRECTION_RIGHT];
		ans.z -= context->layout.padding[GUI_DIRECTION_TOP];
		ans.w -= context->layout.padding[GUI_DIRECTION_BOTTOM];
		}
		break;
	case GUI_LAYOUT_GRID:
		{
		const int32_t indexX = (context->layout.elementIndex % context->layout.grid.count.x) % context->layout.grid.count.x;
		const int32_t indexY = (context->layout.elementIndex / context->layout.grid.count.x) % context->layout.grid.count.y;
		ans.x = indexX       * context->layout.grid.cellSize.x,
		ans.y = indexY       * context->layout.grid.cellSize.y,
		ans.z = (indexX + 1) * context->layout.grid.cellSize.x,
		ans.w = (indexY + 1) * context->layout.grid.cellSize.y;

		const int32_t width  = MIN(ans.z - ans.x, context->layout.max.x);
		const int32_t height = MIN(ans.w - ans.y, context->layout.max.y);
		const int32_t midX = (ans.z + ans.x) / 2;
		const int32_t midY = (ans.w + ans.y) / 2;
		ans.x = midX - width  / 2 + context->layout.padding[GUI_DIRECTION_LEFT];
		ans.z = midX + width  / 2 - context->layout.padding[GUI_DIRECTION_RIGHT];
		ans.y = midY - height / 2 + context->layout.padding[GUI_DIRECTION_TOP];
		ans.w = midY + height / 2 - context->layout.padding[GUI_DIRECTION_BOTTOM];
		}
		break;
	default :
		assert(!"Invalid branch!");
	}
	
	context->layout.elementIndex += advance;

	return ans;
}

ivec4 LayoutGetAbsoluteBounds(const ivec4& bounds, bool advance) {
	ivec4 ans = LayoutGetCellBounds(bounds, advance);
	ans.x += context->viewport.x;
	ans.y += context->viewport.y;
	ans.z += context->viewport.x;
	ans.w += context->viewport.y;
	return ans;
}

Layout AbsoluteLayout() {
	Layout ans = {0};
	ans.type = GUI_LAYOUT_ABSOLUTE;
	ans.max.x = 0xFFFFFFFF;
	ans.max.y = 0xFFFFFFFF;
	ans.grid.cellSize.x = 1;
	ans.grid.cellSize.y = 1;
	ans.padding[0] = 2;
	ans.padding[1] = 2;
	ans.padding[2] = 2;
	ans.padding[3] = 2;
	
	return ans;
}

Layout GridLayout(uint32_t x, uint32_t y) {
	Layout ans = {0};
	ans.type = GUI_LAYOUT_GRID;
	ans.max.x = 0xFFFFFFFF;
	ans.max.y = 0xFFFFFFFF;
	ans.grid.cellSize.x = 1;
	ans.grid.cellSize.y = 1;
	ans.padding[0] = 2;
	ans.padding[1] = 2;
	ans.padding[2] = 2;
	ans.padding[3] = 2;
	ans.grid.count.x = x;
	ans.grid.count.y = y;
	return ans;
}

Layout SplitLayout(bool vertical, float weight, uint8_t separator) {
	Layout ans;
	ans.type = GUI_LAYOUT_SPLIT;
	ans.max.x = 0xFFFFFFFF;
	ans.max.y = 0xFFFFFFFF;
	ans.grid.cellSize.x = 1;
	ans.grid.cellSize.y = 1;
	ans.padding[0] = 2;
	ans.padding[1] = 2;
	ans.padding[2] = 2;
	ans.padding[3] = 2;
	ans.split.vertical = vertical;
	ans.split.separator = separator;
	ans.split.weight = weight;
	return ans;
}

void DummyElement(uint32_t count) {
	context->layout.elementIndex += count;
}

void LabelInternal(uint32_t flags, const ivec4& bounds) {
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			GUIDrawQuad(bounds, context->style.colors[GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(bounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}
}

void Label(const char* text, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);

	LabelInternal(flags, absoluteBounds);
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawText(text, absoluteBounds, context->style.colors[GUI_COLOR_TEXT], (flags & GUI_ALIGN_CENTER));
	}
}

void Label(int32_t id, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);

	LabelInternal(flags, absoluteBounds);
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawIcon(id, absoluteBounds, context->style.colors[GUI_COLOR_TEXT]);
	}
}

bool ButtonInternal(uint32_t flags, const ivec4& bounds) {
	const bool focused = ((RECT_CONTAINS_POINT(bounds, GetMousePosition()) && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED));
	const bool clicked = ((focused && GetMouseLeftButton()) || (flags & GUI_CLICKED));

	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			GUIDrawQuad(bounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(bounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}

	return (focused && !GetMouseLeftButton() && GetLastMouseLeftButton());
}

bool Button(const char* text, uint32_t* flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	bool ans = false;

	if (ButtonInternal(*flags, absoluteBounds)) {
		*flags ^= GUI_CLICKED;
		ans = true;
	}
	if ((*flags & GUI_VISIBLE) && (*flags & GUI_FOREGROUND)) {
		GUIDrawText(text, absoluteBounds, context->style.colors[(*flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (*flags & GUI_ALIGN_CENTER));
	}

	return ans;
}

bool Button(const char* text, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	const bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawText(text, absoluteBounds, context->style.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
	}

	return ans;
}

bool Button(int32_t icon, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	const bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawIcon(icon, absoluteBounds, context->style.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED]/*, (flags & GUI_ALIGN_CENTER) */);
	}

	return ans;
}

bool Button(int32_t icon, uint32_t* flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	bool ans = false;

	if (ButtonInternal(*flags, absoluteBounds)) {
		*flags ^= GUI_CLICKED;
		ans = true;
	}
	if ((*flags & GUI_VISIBLE) && (*flags & GUI_FOREGROUND)) {
		GUIDrawIcon(icon, absoluteBounds, context->style.colors[(*flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED]/*, (flags & GUI_ALIGN_CENTER) */);
	}

	return ans;
}

bool CheckBox(bool &checked, uint32_t flags, const ivec4& nbounds) {
	ivec4 rect = LayoutGetAbsoluteBounds(nbounds, true);
	const int32_t centerX = (rect.z - rect.x) / 2 - 7;
	const int32_t centerY = (rect.w - rect.y) / 2 - 7;
	const int32_t width  = rect.z - rect.x;
	const int32_t height = rect.w - rect.y;
	int32_t min = MIN3(width, height, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	rect.z = rect.x + min;
	rect.w = rect.y + min;
	rect.x += centerX;
	rect.z += centerX;
	rect.y += centerY;
	rect.w += centerY;
	const bool focused = RECT_CONTAINS_POINT(rect, GetMousePosition());
	bool ans = false;
	
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			GUIDrawQuad(rect, context->style.colors[focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(rect, context->style.colors[GUI_COLOR_BORDER]);
		}
		if ((flags & GUI_FOREGROUND) && checked) {
			GUIDrawIcon(GUI_ICON_CHECK, 
						{rect.x + 1, rect.y + 1, rect.z - 1, rect.w - 1}, 
						context->style.colors[GUI_COLOR_ACTIVE]);
		}
	}
	if ((flags & GUI_ENABLED) && focused && !GetMouseLeftButton() && GetLastMouseLeftButton()) {
		checked = !checked;
		ans = true;
	}

	return ans;
}

bool Toggle(bool &enabled, uint32_t flags, const ivec4& nbounds) {
	static const int32_t PADDING = 3;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(nbounds, true);
	const int32_t width = absoluteBounds.z - absoluteBounds.x;
	const int32_t halfWidth = width / 2;
	const bool focused = RECT_CONTAINS_POINT(absoluteBounds, GetMousePosition());
	bool ans = false;

	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			GUIDrawQuad(absoluteBounds, context->style.colors[focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_FOREGROUND) {
			const ivec4 buttonBounds = {absoluteBounds.x + halfWidth * enabled + PADDING * !enabled, absoluteBounds.y + PADDING, absoluteBounds.z - halfWidth * !enabled - PADDING * enabled, absoluteBounds.w - PADDING};
			GUIDrawQuad(buttonBounds, 
						context->style.colors[enabled ? GUI_COLOR_ACTIVE : GUI_COLOR_PANE]);
			GUIDrawBorder(buttonBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(absoluteBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}
	if ((flags & GUI_ENABLED) && focused && !GetMouseLeftButton() && GetLastMouseLeftButton()) {
		enabled = !enabled;
		ans = true;
	}

	return ans;
}

void ProgressBar(float proc, uint32_t flags, const ivec4& bounds) {
	static const uint32_t K = 4;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	const int32_t width = absoluteBounds.z - absoluteBounds.x - K * 2;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;

	if ((width <= 0) || (height <= 0)) {
		return;
	}
	if (flags & GUI_VISIBLE) {
		ivec4 barBounds;
		barBounds.x = absoluteBounds.x + K;
		barBounds.y = absoluteBounds.y + K;
		barBounds.z = absoluteBounds.x - K + width * proc;
		barBounds.w = absoluteBounds.w - K;
		
		if (flags & GUI_BACKGROUND) {
			GUIDrawQuad(absoluteBounds, context->style.colors[GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(absoluteBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
		if (flags & GUI_FOREGROUND) {
			GUIDrawQuad(barBounds, context->style.colors[GUI_COLOR_PANEL]);
			GUIDrawBorder(barBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}
}

bool SliderInternal(float& proc, float step, int32_t boxLength, bool vertical, uint32_t flags, const ivec4& bounds) {
	const int32_t width  = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;
	ivec4 boxBounds;
	ivec2 begin;
	ivec2 end;
	bool ans = false;
	
	if (vertical) {
		const int32_t mid = (bounds.x + bounds.z) / 2;
		begin = {mid, bounds.y};
		end = {mid, bounds.w};
		int32_t k = (float)(height - boxLength) * proc;
		boxBounds = {bounds.x, bounds.w - k - boxLength, bounds.z, bounds.w - k};
	} else {
		const int32_t mid = (bounds.w + bounds.y) / 2;
		begin = {bounds.x, mid};
		end = {bounds.z, mid};
		int32_t k = (float)(width - boxLength) * proc;
		boxBounds = {bounds.x + k, bounds.y, bounds.x + k + boxLength, bounds.w};
	}
	const bool focused = (flags & GUI_ENABLED) && (RECT_CONTAINS_POINT(bounds, GetMousePosition()) || RECT_CONTAINS_POINT(bounds, GetLastMousePosition()));
	const bool clicked = (focused && GetMouseLeftButton());

	if (focused) {
		int32_t mouseWheelDelta = 0;
		if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
			proc -= step * (float)mouseWheelDelta;
			proc = CLAMP(proc, 0.0f, 1.0f);
			ans = true;
		}
	}
	if (clicked) {
		float newValue;
		
		if (vertical) {
			newValue = 1.0f - (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(height - boxLength);
			newValue = CLAMP(newValue, 0.0f, 1.0f);
		} else {
			newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
			newValue = CLAMP(newValue, 0.0f, 1.0f);
		}
		if (newValue != proc) {
			proc = newValue;
			ans = true;
		}
	}
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			if (vertical) {
				GUIDrawQuad({begin.x - 0, begin.y,     end.x + 1, boxBounds.y}, context->style.colors[GUI_COLOR_PANE]);
				GUIDrawQuad({begin.x - 1, boxBounds.w, end.x + 2, end.y      }, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
			} else {
				GUIDrawQuad({begin.x,     begin.y - 1, boxBounds.x, end.y + 2}, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				GUIDrawQuad({boxBounds.z, begin.y - 0, end.x, end.y + 1      }, context->style.colors[GUI_COLOR_PANE]);
			}
		}
		if (flags & GUI_FOREGROUND) {
			GUIDrawQuad(boxBounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(boxBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}

	return ans;
}

bool Slider(float& proc, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	const int32_t width = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;

	return SliderInternal(proc, 0.1f, context->style.values[GUI_VALUE_SLIDER_WIDTH], height > width, flags, absoluteBounds);
}

bool RangeSliderInternal(float& procMin, float& procMax, float step, int32_t boxLength, bool vertical, uint32_t flags, const ivec4& bounds) {
	const int32_t width  = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;
	ivec4 boxBounds[2];
	ivec2 begin;
	ivec2 end;
	bool ans = false;

	if (vertical) {
		const int32_t mid = (bounds.x + bounds.z) / 2;
		begin = {mid, bounds.y};
		end = {mid, bounds.w};
		int32_t k0 = (float)(height - boxLength) * procMin;
		boxBounds[0] = {bounds.x, bounds.w - k0 - boxLength, bounds.z, bounds.w - k0};
	} else {
		const int32_t mid = (bounds.w + bounds.y) / 2;
		begin = {bounds.x, mid};
		end = {bounds.z, mid};
		int32_t k0 = (float)(width - boxLength) * procMin;
		int32_t k1 = (float)(width - boxLength) * procMax;
		boxBounds[0] = {bounds.x + k0, bounds.y, bounds.x + k0 + boxLength, bounds.w};
		boxBounds[1] = {bounds.x + k1, bounds.y, bounds.x + k1 + boxLength, bounds.w};
	}
	const bool focused = (flags & GUI_ENABLED) && (RECT_CONTAINS_POINT(bounds, GetMousePosition()) || RECT_CONTAINS_POINT(bounds, GetLastMousePosition()));
	const bool clicked = (focused && GetMouseLeftButton());
	const bool selectedLeft = abs(GetMousePosition().x - boxBounds[0].z) < abs(GetMousePosition().x - boxBounds[1].x);
	if (focused) {
		int32_t mouseWheelDelta = 0;
		if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
			if (selectedLeft) {
				procMin -= step * (float)mouseWheelDelta;
				procMin = CLAMP(procMin, 0.0f, 1.0f);
			} else {
				procMax -= step * (float)mouseWheelDelta;
				procMax = CLAMP(procMax, 0.0f, 1.0f);
			}
		}
	}
	if (clicked) {
		float newValue;
		if (vertical) {
		} else {
			if (GetMousePosition().x <= boxBounds[0].z) {
				newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
				procMin = CLAMP(newValue, 0.0f, procMax);
				ans = true;
			} else if (GetMousePosition().x >= boxBounds[1].x) {
				newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
				procMax = CLAMP(newValue, procMin, 1.0f);
				ans = true;
			} else {
				if (abs(GetMousePosition().x - boxBounds[0].z) < abs(GetMousePosition().x - boxBounds[1].x)) {
					newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
					procMin = CLAMP(newValue, 0.0f, procMax);
					ans = true;
				} else {
					newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
					procMax = CLAMP(newValue, procMin, 1.0f);
					ans = true;
				}
			}
		}
	}
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			if (vertical) {
				// TODO
			} else {
				GUIDrawQuad({begin.x,     begin.y, boxBounds[0].x, end.y + 1}, context->style.colors[GUI_COLOR_PANE]);
				GUIDrawQuad({boxBounds[0].z, begin.y - 1, boxBounds[1].x, end.y + 2}, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				GUIDrawQuad({boxBounds[1].z, begin.y, end.x, end.y + 1}, context->style.colors[GUI_COLOR_PANE]);
			}
		}
		if (flags & GUI_FOREGROUND) {
			GUIDrawQuad(boxBounds[0], context->style.colors[(clicked && selectedLeft)  ? GUI_COLOR_ACTIVE : (focused && selectedLeft)  ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
			GUIDrawQuad(boxBounds[1], context->style.colors[(clicked && !selectedLeft) ? GUI_COLOR_ACTIVE : (focused && !selectedLeft) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(boxBounds[0], context->style.colors[GUI_COLOR_BORDER]);
			GUIDrawBorder(boxBounds[1], context->style.colors[GUI_COLOR_BORDER]);
		}
	}

	return ans;
}

bool RangeSlider(float& procMin, float& procMax, bool vertical, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	
	return RangeSliderInternal(procMin, procMax, 0.1f, context->style.values[GUI_VALUE_SLIDER_WIDTH], vertical, flags, absoluteBounds);
}

bool TextBox(char* text, const uint32_t max_length, int& carrot, uint32_t flags, const ivec4& bounds) {
	static const color CARROT_COLOR = {255, 255, 255, 255};
	static const int32_t CARROT_WIDTH = 1;
	static const int32_t FONT_HEIGHT = 16;

	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	const ivec4 textBounds = {absoluteBounds.x + 3, absoluteBounds.y + 2, absoluteBounds.z - 3, absoluteBounds.w - 2};
	const int32_t width = textBounds.z - textBounds.x;
	const int32_t height = textBounds.w - textBounds.y;
	const bool focused = RECT_CONTAINS_POINT(textBounds, GetMousePosition(true));
	int32_t length = strlen(text);
	int32_t offset = 0;
	bool ans = false;

	if (carrot > 0) {
		int32_t pos_x = context->getTextSize(context, text, carrot).x;
		while (pos_x > width) {
			pos_x -= context->getTextSize(context, text, 1).x;
			++offset;
		}
	}
	if (flags & GUI_BACKGROUND) {
		GUIDrawQuad(absoluteBounds, context->style.colors[(carrot >= 0) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}
	if (flags & GUI_OUTLINE) {
		GUIDrawBorder(absoluteBounds, context->style.colors[(carrot >= 0) ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_BORDER]);
	}
	if (flags & GUI_ENABLED) {
		if (GetLastMouseLeftButton(true) && (GetMouseLeftButton(true) == false)) {
			if (focused) {
				int32_t pos_x = 0;
				for (carrot = offset; carrot < length; ++carrot){ 
					int32_t k = context->getTextSize(context, text + carrot, 1).x;
					if (pos_x + k >= GetMousePosition().x - textBounds.x) {
						
						break;
					} else {
						pos_x += k;
					}
				}
				context->textBoxActivationTime = time(NULL);
			} else {
				carrot = -1;
			}
		}
	} else {
		carrot = -1;
	}
	if (carrot >= 0) {
		for (uint32_t index = 0; index < context->keyEventCount; ++index) {
			switch (context->keyEvents[index].type) {
			case GUI_EVENT_KEY_DOWN :
				if (context->keyEvents[index].value == context->keyMap[GUI_KEY_HOME]) {
					carrot = 0;
					context->textBoxActivationTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_END]) {
					carrot = length;
					context->textBoxActivationTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_LEFT]) {
					carrot = MAX(carrot - 1, 0);
					context->textBoxActivationTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_RIGHT]) {
					carrot = MIN(carrot + 1, length);
					context->textBoxActivationTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_BACK]) {
					if (carrot > 0) {
						for (int32_t index = --carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						context->textBoxActivationTime = time(NULL);
						ans = true;
					}
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_DELETE]) {
					if (carrot < length) {
						for (int32_t index = carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						carrot = MIN(carrot, length);
						context->textBoxActivationTime = time(NULL);
						ans = true;
					}
				}
				break;
			case GUI_EVENT_CHAR :
				if (length < max_length - 1) {
					length += 1;
					for (int32_t index = length; index >= carrot; --index) {
						text[index] = text[index - 1];
					}
					text[carrot++] = (char)context->keyEvents[index].value;
					ans = true;
				} else {
					printf("\a");
				}
				context->textBoxActivationTime = time(NULL);
				break;
			default :
				assert(!"?!");
			}
		}
		context->keyEventCount = 0;
		if ((flags & GUI_FOREGROUND) && ((time(NULL) - context->textBoxActivationTime) % 2 == 0)) {
			const int32_t carrotPosition = context->getTextSize(context, text + offset, MAX(carrot - offset, 0)).x;
			const int32_t carrotHeight = MIN(height, FONT_HEIGHT);
			GUIDrawQuad({textBounds.x + carrotPosition, textBounds.y, textBounds.x + carrotPosition + CARROT_WIDTH, textBounds.y + carrotHeight}, CARROT_COLOR);
		}
	}
	if ((flags & GUI_FOREGROUND) && (height >= FONT_HEIGHT)) {
		if (flags & GUI_HIDDEN) {
			// TODO
		} else {
			GUIDrawText(text + (offset > 0) * offset, textBounds, context->style.colors[GUI_COLOR_TEXT], 0);
		}
	}

	return ans;
}

bool SpinnerInternal(int& value, const char* text, uint32_t flags, const ivec4& nbounds) {
	static const uint32_t LABEL_FLAG_MASK = GUI_VISIBLE | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_MASK;
	static const uint32_t BUTTON_FLAG_MASK = GUI_VISIBLE | GUI_ENABLED;
	static const uint32_t BUTTON_FLAGS = GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
	static const uint32_t INT_MAX_CHARACTER_COUNT = 16;

	bool ans = false;

	Panel(AbsoluteLayout()){
		const ivec4 bounds = LayoutGetCellBounds(nbounds, true);
		const int32_t buttonWidth = MIN(bounds.z - bounds.x, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);
		uint32_t button_flags = BUTTON_FLAGS | (flags & BUTTON_FLAG_MASK);
		
		Label(text, flags & LABEL_FLAG_MASK, {bounds.x + buttonWidth + 1, bounds.y, bounds.z - buttonWidth - 1, bounds.w});
		if (Button(GUI_ICON_ARROW_LEFT, button_flags, {bounds.x, bounds.y, bounds.x + buttonWidth, bounds.w})) {
			value--;
			ans = true;
		}
		if (Button(GUI_ICON_ARROW_RIGHT, button_flags, {bounds.z - buttonWidth, bounds.y, bounds.z, bounds.w})) {
			value++;
			ans = true;
		}
	}

	return ans;
}


bool Spinner(int& value, uint32_t flags, const ivec4& bounds) {
	static const uint32_t INT_MAX_CHARACTER_COUNT = 16;
	char tmp[INT_MAX_CHARACTER_COUNT];
	snprintf(tmp, INT_MAX_CHARACTER_COUNT, "%d", value);

	return SpinnerInternal(value, tmp, flags, bounds);
}

bool Spinner(int& value, const char** labels, uint32_t count, uint32_t flags, const ivec4& bounds) {
	if (SpinnerInternal(value, labels[CLAMP(value, 0, count - 1)], flags, bounds)) {
		value = CLAMP(value, 0, count - 1);
		return true;
	}
	return false;
}

bool ListBox(int& selectedIndex, int& offset, const char** items, uint32_t count, uint32_t flags, const ivec4& bounds) {
	bool ans = false;

	Panel(AbsoluteLayout()) {
		const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
		const int32_t width = absoluteBounds.z - absoluteBounds.x;
		const int32_t height = absoluteBounds.w - absoluteBounds.y;
		const int32_t itemHeight = context->getTextSize(context, "A", 1).y;// 7 * context->style.values[GUI_VALUE_TEXT_SCALE];
		const int32_t visibleItemCount = height / itemHeight;
		const int32_t minCount = MIN(visibleItemCount, (int32_t)count);
		const int32_t diff = count - visibleItemCount;
		const ivec4 listBounds = {absoluteBounds.x, absoluteBounds.y, absoluteBounds.z - context->style.values[GUI_VALUE_SCROLLBAR_WIDTH] * (diff > 0), absoluteBounds.w};
		const bool focused = RECT_CONTAINS_POINT(listBounds, GetMousePosition());
		int32_t focusedIndex = -1;
		
		offset = MAX(MIN(diff, offset), 0);
		GUIDrawQuad(absoluteBounds, context->style.colors[GUI_COLOR_PANE]);
		GUIDrawBorder(absoluteBounds, context->style.colors[GUI_COLOR_BORDER]);
		if (focused) {
			focusedIndex = CLAMP((GetMousePosition().y - absoluteBounds.y) / itemHeight + offset, offset, (int)count - 1);
			if (GetMouseLeftButton() == false && GetLastMouseLeftButton() == true) {
				selectedIndex = focusedIndex;
				ans = true;
			}
			int32_t mouseWheelDelta = 0;
			if (GetMouseWheelDelta(&mouseWheelDelta, false)) {
				offset += mouseWheelDelta;
				offset = CLAMP(offset, 0, diff);
			}
		}
		for (uint32_t index = offset; index < MIN(offset + minCount, (int)count); ++index) {
			int32_t vOffset = (index - offset) * itemHeight;
			if (vOffset < 0) continue;
			const ivec4 itemBounds = {absoluteBounds.x, absoluteBounds.y + vOffset, absoluteBounds.z - context->style.values[GUI_VALUE_SCROLLBAR_WIDTH] * (diff > 0), absoluteBounds.y + vOffset  + itemHeight};
			if (selectedIndex == index) {
				GUIDrawQuad(itemBounds, context->style.colors[GUI_COLOR_ACTIVE]);
			} else if (focusedIndex == index) {
				GUIDrawQuad(itemBounds, context->style.colors[GUI_COLOR_FOCUSED]);
			}
			GUIDrawText(items[index], itemBounds, context->style.colors[GUI_COLOR_TEXT], flags & GUI_ALIGN_MASK);
		}
		if ((diff > 0) && (width >= context->style.values[GUI_VALUE_SCROLLBAR_WIDTH])) {
			ScrollBar(offset, diff, true, (float)visibleItemCount / (float)count, {width - context->style.values[GUI_VALUE_SCROLLBAR_WIDTH] - 1, 0, width - 1, height});
		}
	}

	return ans;
}

bool ScrollBar(int& offset, const int32_t max, bool vertical, float proc, const ivec4& relativeBounds) {
	bool ans = false;

	Panel(AbsoluteLayout()) {
		const ivec4 bounds = LayoutGetCellBounds(relativeBounds, true);
		const ivec4 absoluteBounds = {bounds.x + context->viewport.x, bounds.y + context->viewport.y, bounds.z + context->viewport.x, bounds.w + context->viewport.y};
		const int32_t width = absoluteBounds.z - absoluteBounds.x;
		const int32_t height = absoluteBounds.w - absoluteBounds.y;
		const int32_t buttonWidth  = MIN(bounds.z - bounds.x, (int)context->style.values[GUI_VALUE_SCROLLBAR_WIDTH]);
		const int32_t buttonHeight = MIN(bounds.w - bounds.y, (int)context->style.values[GUI_VALUE_SCROLLBAR_WIDTH]);
		ivec4 buttonBounds[2];

		if (vertical) {
			buttonBounds[0] = {bounds.x, bounds.y, bounds.z, bounds.y + buttonHeight};
			buttonBounds[1] = {bounds.x, bounds.w - buttonHeight, bounds.z, bounds.w - 1};
		} else {
			buttonBounds[0] = {bounds.x, bounds.y, bounds.x + buttonWidth, bounds.w};
			buttonBounds[1] = {bounds.z - buttonWidth, bounds.y, bounds.z, bounds.w};
		}
		if (Button(vertical ? GUI_ICON_ARROW_UP : GUI_ICON_ARROW_LEFT, GUI_DEFAULT_FLAGS_BUTTON, buttonBounds[0])) {
			offset = MAX(offset - 1, 0);
			ans = true;
		}
		{
		const ivec4 barBounds = {absoluteBounds.x, absoluteBounds.y + buttonWidth, absoluteBounds.z, absoluteBounds.w - buttonWidth};
		const int32_t barHeight = barBounds.w - barBounds.y;
		float proc = 1.0f - (float)offset / (float)(max + 0);
		float barLength = barHeight / (max + 1);
		float step = 0.1f;
		if (SliderInternal(proc, step, barLength, vertical, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE, barBounds)) {
			offset = (1.0f - proc) * (float)(max + 0);
		}
		}
		if (Button(vertical ? GUI_ICON_ARROW_DOWN : GUI_ICON_ARROW_RIGHT, GUI_DEFAULT_FLAGS_BUTTON, buttonBounds[1])) {
			offset = MIN(offset + 1, max);
			ans = true;
		}
	}

	return ans;
}

Layout BeginPanel(const Layout& layout) {
	context->layout.parent = context->viewport;
	context->viewport = LayoutGetAbsoluteBounds({0, 0, 0, 0}, true);
	Layout ans = context->layout;
	ans.run_statement = true;
	SetLayout(layout);

	return ans;
}

void EndPanel(Layout* bkp_layout) {
	context->layout = *bkp_layout;
	context->viewport = context->layout.parent;
	bkp_layout->run_statement = false;
}

Layout BeginGroup(const char* text, const ivec4& bounds) {
	const Layout ans = BeginPanel();
	const int32_t l = context->style.values[GUI_VALUE_TEXT_SCALE] * 7 / 2;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	const ivec4 absoluteBounds2 = {absoluteBounds.x, absoluteBounds.y + l, absoluteBounds.z, absoluteBounds.w};
	const int32_t width = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;
	
	GUIDrawBorder(absoluteBounds2, context->style.colors[GUI_COLOR_BORDER]);
	context->viewport.x = absoluteBounds2.x + l;
	context->viewport.y = absoluteBounds2.y + l * 2;
	context->viewport.z = absoluteBounds2.z - l;
	context->viewport.w = absoluteBounds2.w - l;
	if (height >= context->style.values[GUI_VALUE_TEXT_SCALE] * 7) {
		const ivec2 textSize = context->getTextSize(context, text, strlen(text));
		int32_t offset = (absoluteBounds.z - absoluteBounds.x - textSize.x) * ((float)context->style.values[GUI_VALUE_GROUP_OFFSET] / 255.0f);
		const ivec4 textBounds = {absoluteBounds.x + offset, absoluteBounds.y, absoluteBounds.x + textSize.x + offset, absoluteBounds.y + textSize.y};
		GUIDrawQuad(textBounds, context->style.colors[GUI_COLOR_PANEL]);
		GUIDrawText(text, textBounds, context->style.colors[GUI_COLOR_TEXT], GUI_ALIGN_CENTER);
	}
	
	return ans;
}

Layout BeginSplitPanel(bool vertical, float& weight, const ivec4& bounds) {
	static const int32_t SEPARATOR = 4;
	const Layout ans = BeginPanel(SplitLayout(vertical, weight, SEPARATOR));
	const ivec4 absoluteBounds = context->viewport; // DAFUQ ?!
	const int32_t width  = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;
	const int32_t halfPadding = SEPARATOR / 2;
	ivec4 separatorBounds;

	if (vertical) {
		const int32_t k = (float)(width - SEPARATOR) * weight;
		separatorBounds = {absoluteBounds.x + k, absoluteBounds.y, absoluteBounds.x + k + SEPARATOR, absoluteBounds.w};
	} else {
		const int32_t k = (float)(height - SEPARATOR) * weight;
		separatorBounds = {absoluteBounds.x, absoluteBounds.y + k, absoluteBounds.z, absoluteBounds.y + k + SEPARATOR};
	}

	const bool focused = (RECT_CONTAINS_POINT(separatorBounds, GetMousePosition()) || RECT_CONTAINS_POINT(separatorBounds, GetLastMousePosition()));
	const bool clicked = focused && GetMouseLeftButton();

	if (clicked) {
		if (vertical) {
			weight = (float)(GetMousePosition().x - halfPadding - absoluteBounds.x) / (float)(width - SEPARATOR);
		} else {
			weight = (float)(GetMousePosition().y - halfPadding - absoluteBounds.y) / (float)(height - SEPARATOR);
		}
		weight = CLAMP(weight, 0.01f, 0.99f);
	}
	GUIDrawQuad(separatorBounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);

	return ans;
}

Layout BeginTabPanel(const char** names, uint32_t count, int& selected, const ivec4& bounds) {
	const Layout ans = BeginPanel(AbsoluteLayout());
	const ivec4 absoluteRectangle = LayoutGetAbsoluteBounds(bounds, true);
	const int32_t tabWidth = (absoluteRectangle.z - absoluteRectangle.x) / count;
	const int32_t tabHeight = MIN(absoluteRectangle.w - absoluteRectangle.y, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);

	for (int32_t index = 0; index < count; ++index) {
		if (Button(names[index], GUI_DEFAULT_FLAGS_BUTTON | ((index == selected) ? GUI_CLICKED | GUI_FOCUSED : GUI_NONE), {index * tabWidth, 0, (index + 1) * tabWidth - 1, tabHeight})) {
			selected = index;
		}
	}

	context->viewport.x = absoluteRectangle.x;
	context->viewport.y = absoluteRectangle.y + tabHeight + 1;
	context->viewport.z = absoluteRectangle.z;
	context->viewport.w = absoluteRectangle.w;

	GUIDrawBorder(context->viewport, context->style.colors[GUI_COLOR_BORDER]);

	return ans;
}

Layout BeginWindow(ivec4* rbounds, const char* text, uint32_t* flags) {
	static const int32_t SEPARATOR = 2; // TODO: replace with Layout::padding
	static const int32_t CloseButtonSize = context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
	Layout ans = BeginPanel();
	ivec4 bounds = LayoutGetAbsoluteBounds(*rbounds, true);
	const int32_t width  = MAX(bounds.z - bounds.x, CloseButtonSize + SEPARATOR * 2);
	const int32_t height = MAX(bounds.w - bounds.y, CloseButtonSize + SEPARATOR * 2);
	const ivec4 titlebarRectangle = {bounds.x, bounds.y, bounds.z - CloseButtonSize, bounds.y + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]};
	uint32_t defaultFlags = GUI_DEFAULT_FLAGS_WINDOW  & ~GUI_WINDOW_CLOSE;

	if (flags == NULL) {
		flags = & defaultFlags;
	}
	if ((*flags & GUI_VISIBLE) == 0) {
		ans.run_statement = false;
		EndPanel(&ans);
		return ans;
	}

	const int32_t padding = ((*flags & GUI_WINDOW_SIZE ) ? SEPARATOR : 0);
	const int32_t content_offset_y = ((*flags & GUI_WINDOW_TITLEBAR ) ? context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] : 0);

	if (bounds.z < bounds.x + padding * 2) {
		bounds.z = bounds.x + padding * 2;
	}
	if (bounds.w < bounds.y + padding * 2 + content_offset_y) {
		bounds.w = bounds.y + padding * 2 + content_offset_y;
	}
	if ((*flags & GUI_WINDOW_TITLEBAR) && (*flags & GUI_WINDOW_MOVE)) {
		if (((    GetMouseLeftButton() == true && RECT_CONTAINS_POINT(titlebarRectangle,     GetMousePosition())) ||
			 (GetLastMouseLeftButton() == true && RECT_CONTAINS_POINT(titlebarRectangle, GetLastMousePosition())))) {
			const ivec2 mousePosition = GetMousePosition();
			const ivec2 lastMousePosition = GetLastMousePosition();
			ivec2 offset =  {mousePosition.x - lastMousePosition.x, mousePosition.y - lastMousePosition.y};
			bounds.x += offset.x;
			bounds.y += offset.y;
			bounds.z += offset.x;
			bounds.w += offset.y;
		}	
	}
	// Draw titlebar
	context->viewport = bounds;
	int32_t offsetY = 0;
	if (*flags & GUI_WINDOW_TITLEBAR) {
		GUIDrawQuad({bounds.x, bounds.y, bounds.z, bounds.y + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]}, context->style.colors[GUI_COLOR_TITLEBAR]);
		if (text) {
			GUIDrawText(text, {titlebarRectangle.x + 2, titlebarRectangle.y + 2, titlebarRectangle.z, titlebarRectangle.w}, context->style.colors[GUI_COLOR_TEXT], 0);
		}
		const int32_t width = bounds.z - bounds.x;
		if ((*flags & GUI_WINDOW_CLOSE)) {
			if (Button(GUI_ICON_CLOSE, GUI_DEFAULT_FLAGS_BUTTON, {width - context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] + 1, 1, width - 1, context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] - 1})) {
				*flags &= ~GUI_VISIBLE;
			}
		}
		offsetY += context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
	}

	context->viewport.x = bounds.x + context->style.values[GUI_VALUE_WINDOW_PADDING];
	context->viewport.y = bounds.y + context->style.values[GUI_VALUE_WINDOW_PADDING] + content_offset_y;
	context->viewport.z = bounds.z - context->style.values[GUI_VALUE_WINDOW_PADDING];
	context->viewport.w = bounds.w - context->style.values[GUI_VALUE_WINDOW_PADDING] - padding;
	SetLayout(GridLayout(1, 1));

	// Draw window body
	GUIDrawQuad({bounds.x, bounds.y + offsetY, bounds.z, bounds.w}, context->style.colors[GUI_COLOR_PANEL]);
	GUIDrawBorder(bounds, context->style.colors[GUI_COLOR_BORDER]);

	if (*flags & GUI_WINDOW_SIZE) {
		const bool isInside  = (GetMousePosition().x >= bounds.z - 15) && (GetMousePosition().x <= bounds.z) && (GetMousePosition().y >= bounds.w - 15) && (GetMousePosition().y <= bounds.w);
		const bool wasInside = (GetLastMousePosition().x >= bounds.z - 15) && (GetLastMousePosition().x <= bounds.z) && (GetLastMousePosition().y >= bounds.w - 15) && (GetLastMousePosition().y <= bounds.w);
		const bool focused = (isInside || wasInside);
		const bool clicked = (focused && GetMouseLeftButton());

		GUIDrawIcon(GUI_ICON_SIZE, {bounds.z - 15, bounds.w - 15, bounds.z, bounds.w}, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		if (clicked) {
			if (GetLastMouseLeftButton()) {
				const ivec2 mousePosition = GetMousePosition();
				const ivec2 lastMousePosition = GetLastMousePosition();
				ivec2 offset =  {mousePosition.x - lastMousePosition.x, mousePosition.y - lastMousePosition.y};
				bounds.z += offset.x;
				bounds.w += offset.y;
			}
		}
	}
	if (rbounds) {
		*rbounds = bounds;
	}
	
	return ans;
}

void SetLayout(const Layout& layout) {
	context->layout = layout;
	context->layout.elementIndex = 0;
	switch (context->layout.type) {
	case GUI_LAYOUT_GRID:
		context->layout.grid.cellSize.x = (context->viewport.z - context->viewport.x) / context->layout.grid.count.x;
		context->layout.grid.cellSize.y = (context->viewport.w - context->viewport.y) / context->layout.grid.count.y;
		break;
	}
}

Layout* GetLayout() {
	return &context->layout;
}

#endif /* IMGUI_IMPLEMENTATION */

#undef MIN
#undef MIN2
#undef MIN3
#undef MAX
#undef MAX2
#undef MAX3
#undef CLAMP
#undef RECT_CONTAINS_POINT

#endif /* __GUI_H__ */
