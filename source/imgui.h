/*
TODO:
	- remove 'bounds' parameter
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

+---------------------------+
|          Margin           |
|  +---------------------+  |
|  |       Border        |  |
|  |  +---------------+  |  |
|  |  |    Padding    |  |  |
|  |  |  +---------+  |  |  |
|  |  |  | Content |  |  |  |
|  |  |  +---------+  |  |  |
|  |  |               |  |  |
|  |  +---------------+  |  |
|  |                     |  |
|  +---------------------+  |
|                           |
+---------------------------+
*/

#ifndef __GUI_H__
#define __GUI_H__

#define IMGUI_INCLUDE_WINDOW_MANAGER

#include <stdint.h>
#include <stdbool.h>

enum GUIOrientation : uint8_t {
	GUI_VERTICAL,
	GUI_HORIZONTAL,
};

#define GUI_LAYOUT_ABSOLUTE 0
#define GUI_LAYOUT_GRID     1
#define GUI_LAYOUT_SPLIT    2
#define GUI_LAYOUT_BORDER   3

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
#define GUI_KEY_UP     6
#define GUI_KEY_DOWN   7
#define GUI_KEY_COUNT  8

#define GUI_ICON_CLOSE        0
#define GUI_ICON_ARROW_LEFT   1
#define GUI_ICON_ARROW_RIGHT  2
#define GUI_ICON_ARROW_DOWN   3
#define GUI_ICON_ARROW_UP     4
#define GUI_ICON_SIZE         5
#define GUI_ICON_CHECK        6
#define GUI_ICON_CUSTOM       (GUI_ICON_CHECK + 1)

#define GUI_MAX_KEY_EVENT_COUNT 4

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
#define GUI_VALUE_TITLEBAR_HEIGHT 0
#define GUI_VALUE_SLIDER_WIDTH    1
#define GUI_VALUE_COUNT           2

const uint32_t GUI_NONE                = 0;
const uint32_t GUI_VISIBLE             = 1 << 0;
const uint32_t GUI_ENABLED             = 1 << 1;
const uint32_t GUI_BACKGROUND          = 1 << 2;
const uint32_t GUI_FOREGROUND          = 1 << 3;
const uint32_t GUI_OUTLINE             = 1 << 4;
const uint32_t GUI_FOCUSED             = 1 << 5;
const uint32_t GUI_CLICKED             = 1 << 6;

const uint32_t GUI_WINDOW_TITLEBAR     = 1 << 7;
const uint32_t GUI_WINDOW_CLOSE        = 1 << 8;
const uint32_t GUI_WINDOW_MOVE         = 1 << 9;
const uint32_t GUI_WINDOW_SIZE         = 1 << 10;
const uint32_t GUI_WINDOW_DECORATION   = GUI_WINDOW_TITLEBAR | GUI_WINDOW_CLOSE;

const uint32_t GUI_HIDDEN              = 1 << 11;
const uint32_t GUI_MULTILINE           = 1 << 12;

const uint32_t GUI_ALIGN_LEFT          = 1 << 13;
const uint32_t GUI_ALIGN_BOTTOM        = 1 << 14;
const uint32_t GUI_ALIGN_RIGHT         = 1 << 15;
const uint32_t GUI_ALIGN_TOP           = 1 << 16;

const uint32_t GUI_ALIGN_LEFT_TOP      = GUI_ALIGN_TOP | GUI_ALIGN_LEFT;
const uint32_t GUI_ALIGN_LEFT_CENTER   = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT;
const uint32_t GUI_ALIGN_LEFT_BOTTOM   = GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT;
const uint32_t GUI_ALIGN_BOTTOM_CENTER = GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_BOTTOM_RIGHT  = GUI_ALIGN_BOTTOM | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_CENTER_RIGHT  = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_TOP_RIGHT     = GUI_ALIGN_TOP | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_TOP_CENTER    = GUI_ALIGN_TOP | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;
const uint32_t GUI_ALIGN_CENTER        = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;

const uint32_t GUI_ALIGN_MASK          = GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT;

const uint32_t GUI_FLAGS_LABEL         = GUI_VISIBLE |               GUI_FOREGROUND |                                GUI_ALIGN_CENTER;
const uint32_t GUI_FLAGS_BUTTON        = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
const uint32_t GUI_FLAGS_SPINNER       = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND |               GUI_ALIGN_CENTER;
const uint32_t GUI_FLAGS_CHECKBOX      = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;
const uint32_t GUI_FLAGS_PROGRESSBAR   = GUI_VISIBLE |               GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;
const uint32_t GUI_FLAGS_SLIDER        = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND;
const uint32_t GUI_FLAGS_TEXTBOX       = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_LEFT_TOP;
const uint32_t GUI_FLAGS_PANEL         = GUI_VISIBLE;
const uint32_t GUI_FLAGS_WINDOW        = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE  | GUI_WINDOW_DECORATION;

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
			ivec2 cellSize;  // cached data
		} grid;
		struct {
			float weight;
			uint8_t separator;
			GUIOrientation orientation;
		} split;
		struct {
			float header;
			float body;
			float footer;
			GUIOrientation orientation;
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
} GUIStyle;

typedef struct {
	uint32_t value;
	uint8_t type;
} KeyEvent;

struct GUIContext;
typedef void (*DrawLineProc)(GUIContext*, const ivec2&, const ivec2&, const color_t&);
typedef void (*DrawQuadProc)(GUIContext*, const ivec4&, const color_t&);
typedef void (*DrawCharProc)(GUIContext*, char c, float pos_x, float pos_y, const color_t& color);
typedef void (*DrawTextProc)(GUIContext*, const char*, const ivec4&, const color_t&, uint32_t);
typedef void (*DrawIconProc)(GUIContext*, int32_t, const ivec4&, const color_t&);
typedef void (*DrawBorderProc)(GUIContext*, const ivec4&, const color_t&);
typedef ivec2 (*GetCharSizeProc)(GUIContext*, char);
typedef ivec2 (*GetTextSizeProc)(GUIContext*, const char*, uint32_t);

#if defined IMGUI_INCLUDE_WINDOW_MANAGER
typedef struct WindowInfo {
	ivec4* bounds;
	uint32_t* flags;
	int32_t id;
	bool receiveEvents;
} WindowInfo;
#endif
typedef struct GUIContext {
	Layout layout;
	GUIStyle style;
	uint32_t keyMap[GUI_KEY_COUNT];

	void* opaqueData;	
	DrawLineProc drawLine;
	DrawQuadProc drawQuad;
	DrawCharProc drawChar;
	DrawTextProc drawText;
	DrawIconProc drawIcon;
	DrawBorderProc drawBorder;
	GetCharSizeProc getCharSize;
	GetTextSizeProc getTextSize;

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
#if defined IMGUI_INCLUDE_WINDOW_MANAGER
	static const uint32_t MAX_WINDOW_COUNT = 8;

	WindowInfo windows[MAX_WINDOW_COUNT];
	uint32_t count;
	int32_t modal;
#endif
} GUIContext;

void GUIDrawChar(char c, const ivec2& position, const color_t& color);

void GUIDrawText(const char* text, const ivec4& bounds, const color_t& color, uint32_t flags = 0);

void GUIDrawLine(const ivec2& begin, const ivec2& end, const color_t& color);

void GUIDrawQuad(const ivec4& bounds, const color_t& color);

void GUIDrawIcon(int32_t id, const ivec4& bounds, const color_t& color);

void GUIDrawBorder(const ivec4& rect, const color_t& color);


void GUIOnCursorEvent(int32_t x, int32_t y);

void GUIOnMouseWheelEvent(int32_t delta);

void GUIOnButtonEvent(uint32_t button, bool status);

void GUIOnKeyEvent(uint32_t key, bool status);

void GUIOnCharEvent(char c);


int32_t GUIBeginFrame();

void GUIEndFrame();


void GUIContextInit(GUIContext* context);

void GUISetActiveContext(GUIContext*);

GUIContext* GUIGetActiveContext();


Layout AbsoluteLayout(uint32_t margin = 0);

Layout SplitLayout(GUIOrientation orientation, float weight = 0.5f, uint32_t separator = 0, uint32_t margin = 0);

Layout FixSplitLayout(GUIOrientation orientation, int32_t size, uint32_t separator = 0, uint32_t margin = 0);

Layout BorderLayout(GUIOrientation orientation, float headerWeight = 0.33f, float footerWeight = 0.33f, uint32_t margin = 0);

Layout GridLayout(uint32_t x, uint32_t y, uint32_t margin = 2);

void SetLayout(const Layout& layout);

Layout* GetLayout();

ivec4 LayoutGetAbsoluteBounds(bool advance = true);


void DummyElement(uint32_t count = 1);

void Label(const char* text, uint32_t flags = GUI_FLAGS_LABEL);

void Label(int32_t iconID, uint32_t flags = GUI_FLAGS_LABEL);

bool Button(const char* text, uint32_t flags = GUI_FLAGS_BUTTON);

bool Button(int32_t iconID, uint32_t flags = GUI_FLAGS_BUTTON);

bool Button(int32_t iconID, uint32_t* flags);

bool Button(int32_t inconID, const char* text, uint32_t flags = GUI_FLAGS_BUTTON);

bool CheckBox(bool& state, uint32_t flags = GUI_FLAGS_CHECKBOX);

bool Toggle(bool& state, uint32_t flags = GUI_FLAGS_CHECKBOX);

bool Spinner(int& value, uint32_t flags = GUI_FLAGS_SPINNER);

bool Spinner(int& value, const char** textValues, uint32_t textCount, uint32_t flags = GUI_FLAGS_SPINNER);

void ProgressBar(float progress, uint32_t flags = GUI_FLAGS_PROGRESSBAR);

bool Slider(float& value, uint32_t flags = GUI_FLAGS_SLIDER);

bool Slider(float& value, uint32_t boxLength, GUIOrientation orientation, uint32_t flags = GUI_FLAGS_SLIDER);

bool RangeSlider(float& minValue, float& maxValue, GUIOrientation = GUI_VERTICAL, uint32_t flags = GUI_FLAGS_SLIDER);

bool TextBox(char* text, uint32_t textLength, int32_t& carrot, uint32_t flags = GUI_FLAGS_TEXTBOX, uint32_t padding = 3);

bool TextArea(char*, uint32_t, int32_t&, uint32_t = GUI_FLAGS_TEXTBOX, uint32_t padding = 2);

bool Scrollbar(float& progress, float barProc = 0.1f, GUIOrientation orientation = GUI_VERTICAL, float step = 0.1f);

Layout BeginWindow(ivec4* bounds, const char* title = nullptr, uint32_t padding = 0, uint32_t* flags = nullptr);

Layout BeginPanel(const Layout& layout = AbsoluteLayout(), uint32_t padding = 0, uint32_t flags = GUI_FLAGS_PANEL);

Layout BeginSplitPanel(GUIOrientation orientation, float& , uint32_t padding = 0, uint32_t flags = GUI_FLAGS_PANEL);

Layout BeginTabPanel(const char* names, int& selectedTab, uint32_t padding = 0, uint32_t flags = 0);

Layout BeginScrollPanel(int, int, int* = nullptr, int* = nullptr, uint32_t padding = 0, uint32_t flags = 0);

void EndPanel(Layout* backup);

#define GUIFrame(...)  for(int32_t ___tmp = GUIBeginFrame(__VA_ARGS__); !___tmp; (___tmp += 1), GUIEndFrame())

#define Panel(...)  for(Layout ___tmp = BeginPanel(__VA_ARGS__); ___tmp.run_statement; EndPanel(&___tmp))

#define SplitPanel(...)  for(Layout ___tmp = BeginSplitPanel(__VA_ARGS__); ___tmp.run_statement; EndPanel(&___tmp))

#define TabPanel(...)  for(Layout ___tmp = BeginTabPanel(__VA_ARGS__); ___tmp.run_statement; EndPanel(&___tmp))

#define ScrollPanel(...)  for(Layout ___tmp = BeginScrollPanel(__VA_ARGS__); ___tmp.run_statement; EndPanel(&___tmp))

#define Window(...)  for(Layout ___tmp = BeginWindow(__VA_ARGS__); ___tmp.run_statement; EndPanel(&___tmp))

#if defined (IMGUI_INCLUDE_WINDOW_MANAGER)
void WMBringIndexToFront(int32_t selected);

int32_t WMGetIndexByID(int32_t id);

bool WMRegister(ivec4* bounds, uint32_t* flags, int32_t id = -1);

bool WMUnregister(GUIContext* context, int32_t id);

void WMBringIDToFront(int32_t id);

void WMOnCursorEvent(int32_t x, int32_t y);

void WMOnButtonEvent(int32_t button, int32_t value);
#else
#define WMOnCursorEvent GUIOnCursorEvent
#define WMOnButtonEvent GUIOnButtonEvent
#endif // IMGUI_INCLUDE_WINDOW_MANAGER

#endif /* __GUI_H__ */
