#include "imgui.h"

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>    // time(NULL)

static GUIContext* context = NULL;

#define CUR_POS_BEGIN      0
#define CUR_POS_END        1
#define CUR_POS_PREV_LINE  2
#define CUR_POS_NEXT_LINE  3

int GetCursorPosition(const char* text, int cursor) {
	int ans = 0;

	for (; ans < cursor; ++ans) {
		if (text[cursor - ans - 1] == '\n') {
			break;
		}
	}

	return ans;
}

int MoveCursor(const char* text, int cursor, int location) {
	int leftCount = 0;
	int max = 0;

	switch (location) {
	case CUR_POS_BEGIN :
		while ((cursor > 0) && (text[cursor - 1] != '\n')) {
			--cursor;
		}
		break;
	case CUR_POS_END :
		while (text[cursor] != '\0' && text[cursor] != '\n') {
			++cursor;
		}
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

#define RECT_CONTAINS_POINT(RECT, POINT) (((POINT).x >= (RECT).x) && ((POINT).x < (RECT).z) && ((POINT).y >= (RECT).y) && ((POINT).y < (RECT).w))

ivec4 min(const ivec4& A, const ivec4& B) {
	return {
		std::max(A.x, B.x),
		std::max(A.y, B.y),
		std::min(A.z, B.z),
		std::min(A.w, B.w),
	};
}

#if defined (IMGUI_INCLUDE_WINDOW_MANAGER)
void WMBringIndexToFront(int32_t selected) {
	if (selected >= 0 && selected < (int32_t)context->count) {
		const WindowInfo tmp = context->windows[selected];
		for (int32_t index = selected; index < (int32_t)context->count - 1; ++index) {
			context->windows[index] = context->windows[index + 1];
		}
		context->windows[context->count - 1] = tmp;
	}
}

int32_t WMGetIndexByID(int32_t id) {
	for (uint32_t index = 0; index < context->count; ++index) {
		if (context->windows[index].id == id) {
			return index;
		}
	}
	return -1;
}

bool WMRegister(ivec4* bounds, uint32_t* flags, int32_t id) {
	if (context != nullptr && context->count < GUIContext::MAX_WINDOW_COUNT) {
		context->windows[context->count].bounds = bounds;
		context->windows[context->count].flags = flags;
		context->windows[context->count].id = (id == -1) ? context->count : id;
		context->count += 1;
		return true;
	}
	return false;
}

bool WMUnregister(GUIContext* context, int32_t id) {
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


void WMBringIDToFront(int32_t id) {
	WMBringIndexToFront(WMGetIndexByID(id));
}

void WMOnCursorEvent(int32_t x, int32_t y) {
	bool receiveEvents = true;
	if (!context->mouseButtonLeft || !context->lastMouseButtonLeft) 
	for (int32_t index = context->count - 1; index >= 0; --index) {
		if (((*context->windows[index].flags) & GUI_VISIBLE) && 
			(RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition))&&
			(RECT_CONTAINS_POINT(*context->windows[index].bounds, context->lastMousePosition))) {
			context->windows[index].receiveEvents = receiveEvents;
			receiveEvents = false;
		} else {
			context->windows[index].receiveEvents = false;
		}
	}
	GUIOnCursorEvent(x, y);
}

void WMOnButtonEvent(int32_t button, int32_t value) {
	if ((value == GUI_BUTTON_PRESSED)) {
		if (context->modal >= 0) {
			const int32_t index = WMGetIndexByID(context->modal);
			if (index >= 0 && index <= (int32_t)context->count) {
				if ((*context->windows[index].flags) & GUI_VISIBLE) {
					if (!RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition)) {
						printf("\a");
						return;
					}
				} else {
					context->modal = -1;
				}
			}
		}
		bool receiveEvents = true;
		for (int32_t index = context->count - 1; index >= 0; --index) {
			if (((*context->windows[index].flags) & GUI_VISIBLE) 
				&& (RECT_CONTAINS_POINT(*context->windows[index].bounds, context->mousePosition))) {
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

void GUIDrawChar(char c, const ivec2& position, const color_t& color) {
	if (context == NULL) {
		return;
	}
	context->drawChar(context, c, position.x, position.y, color);
}

void GUIDrawText(const char* text, const ivec4& rect, const color_t& color, uint32_t flags) {
	if (context == NULL) {
		return;
	}
	// Clipping is done in the renderer
	context->drawText(context, text, rect, color, flags);
}

void GUIDrawLine(const ivec2& begin, const ivec2& end, const color_t& color) {
	if (context == NULL) {
		return;
	}
	context->drawLine(context, begin, end, color);
}

void GUIDrawQuad(const ivec4& rect, const color_t& color) {
	ivec4 bounds = rect;
	if (context == NULL) {
		return;
	}
	if (RectGetArea(context->clip) > 0) {
		RectClip(bounds, context->clip);
	}
	context->drawQuad(context, bounds, color);
}

void GUIDrawIcon(int32_t id, const ivec4& rect, const color_t& color) {
	ivec4 bounds = rect;
	if (context == NULL) {
		return;
	}
	if (RectGetArea(context->clip) > 0) {
		RectClip(bounds, context->clip);
	}
	context->drawIcon(context, id, bounds, color);
}

void GUIDrawBorder(const ivec4& rect, const color_t& color) {
	ivec4 bounds = rect;
	if (context == NULL) {
		return;
	}
	if (RectGetArea(context->clip) > 0) {
		RectClip(bounds, context->clip);
	}
	context->drawBorder(context, bounds, color);
}

void DrawBorder(GUIContext* context, const ivec4& bounds, const color_t& color) {
	if (((bounds.w - bounds.y) <= 0) || ((bounds.z - bounds.x) <= 0)) {
		return;
	}
	GUIDrawLine({bounds.x, bounds.y}, {bounds.x, bounds.w}, color);
	GUIDrawLine({bounds.x, bounds.w - 1}, {bounds.z, bounds.w - 1}, color);
	GUIDrawLine({bounds.z - 1, bounds.y}, {bounds.z - 1, bounds.w}, color);
	GUIDrawLine({bounds.x, bounds.y}, {bounds.z, bounds.y}, color);
}

ivec2 GetTextSize(GUIContext* guiContext, const char* text, uint32_t length) {
	int maxX = 0;
	ivec2 ans = {0, 0};
	for (uint32_t index = 0; (index < length) && (text[index] != '\0'); ++index) {
		switch (text[index]) {
		case '\n' :
			ans.x = 0;
			ans.y += 16;
			break;
		default :
			const ivec2 charSize = context->getCharSize(guiContext, text[index]);
			ans.x += charSize.x;
			ans.y = std::max(ans.y, charSize.y);
			maxX = std::max(maxX, ans.x);
			break;
		}
	}
	ans.x = std::max(ans.x, maxX);
	return ans;
}

int GetLineXOffset(const char* text, const ivec4& bounds, uint32_t flags) {
	const int lineSize = context->getTextSize(context, text, MoveCursor(text, 0, CUR_POS_END)).x;
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

	const int textSize = context->getTextSize(context, text, strlen(text)).y;
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
			const int charWidth = context->getCharSize(context, *text).x;
			if ((pos_x + charWidth <= bounds.z && pos_y <= bounds.w)) { // seems to draw 1 more char 
				context->drawChar(context, *text, pos_x, pos_y, color);
				pos_x += charWidth;
			}
			break;
		}
		++text;
	}
}

void GUISetActiveContext(GUIContext* ncontext) {
	context = ncontext;
	
	assert(context != NULL);
	assert(context->drawLine != NULL);
	assert(context->drawQuad != NULL);
	assert(context->drawChar != NULL);
	assert(context->drawText != NULL);
	assert(context->drawIcon != NULL);
	assert(context->getCharSize != NULL);
	assert(context->getTextSize != NULL);
}

GUIContext* GUIGetActiveContext() {
	return context;
}

bool GetMouseWheelDelta(int32_t *delta, bool force) {
	if ((context == NULL) || (context->mouse_wheel_delta == 0) || (force == false && context->events_enabled == false) 
#if defined IMGUI_INCLUDE_WINDOW_MANAGER
		|| (context->modal != -1)
#endif
	)  {
		return false;
	}
	*delta = context->mouse_wheel_delta;
	return true;
}

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

void GUIContextInit(GUIContext* context) {
	context->layout = GridLayout(1, 1, 0);
		
	context->drawLine = NULL;
	context->drawQuad = NULL;
	context->drawChar = NULL;
	context->drawIcon = NULL;
	context->getCharSize = NULL;
	context->opaqueData = NULL;
	
	context->drawBorder = DrawBorder;
	context->drawText = DrawText;
	context->getTextSize = GetTextSize;
	
	context->lastMousePosition = {-1, -1};
	context->mousePosition = {-1, -1};
	context->lastMouseButtonLeft = false;
	context->mouseButtonLeft = false;
	context->keyEventCount = 0;
	context->mouse_wheel_delta = 0;
	context->events_enabled = true;
	
	context->style.colors[GUI_COLOR_TEXT]          = {230, 230, 230, 255};
	context->style.colors[GUI_COLOR_TEXT_DISABLED] = {150, 150, 150, 255};
	context->style.colors[GUI_COLOR_TITLEBAR]      = { 25,  25,  25, 200};
	context->style.colors[GUI_COLOR_PANEL]         = { 50,  50,  50, 255};
	context->style.colors[GUI_COLOR_PANE]          = { 75,  75,  75, 255};
	context->style.colors[GUI_COLOR_FOCUSED]       = {110, 110, 120, 255};
	context->style.colors[GUI_COLOR_ACTIVE]        = { 75,  75, 255, 255};
	context->style.colors[GUI_COLOR_BORDER]        = { 25,  25,  25, 255};
	
	context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] = 20;
	context->style.values[GUI_VALUE_SLIDER_WIDTH]    =  7;

#if defined IMGUI_INCLUDE_WINDOW_MANAGER
	context->count = 0;
	context->modal = -1;
#endif // IMGUI_INCLUDE_WINDOW_MANAGER
	context->clip = {0, 0, 0, 0};
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
	(void)status;
	context->keyEvents[context->keyEventCount++] = {key, GUI_EVENT_KEY_DOWN};
}

void GUIOnCharEvent(char c) {
	if (context->keyEventCount >= GUI_MAX_KEY_EVENT_COUNT) {
		return;
	}
	if ((c >= 32 && c < 128) || (c == '\n')) {
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

ivec4 LayoutGetBounds(bool advance) {
	ivec4 ans;

	switch (context->layout.type) {
	case GUI_LAYOUT_ABSOLUTE :
		ans.x = 0;
		ans.y = 0;
		ans.z = context->viewport.z - context->viewport.x;
		ans.w = context->viewport.w - context->viewport.y;
		break;
	case GUI_LAYOUT_SPLIT :
		{
		const int32_t width  = context->viewport.z - context->viewport.x;
		const int32_t height = context->viewport.w - context->viewport.y;
		const int32_t index = context->layout.elementIndex % 2;
		switch (context->layout.split.orientation) {
		case GUI_VERTICAL :
			{
			const int32_t topHeight = (float)(height - context->layout.split.separator) * context->layout.split.weight + 0.5f;
			const int32_t botHeight = height - context->layout.split.separator - topHeight;
			ans.x = 0;
			ans.y =             (topHeight + context->layout.split.separator) * index;
			ans.z = width;
			ans.w = topHeight + (context->layout.split.separator + botHeight) * index;
			}
			break;
		case GUI_HORIZONTAL :
			{
			const int32_t leftWidth  = (float)(width - context->layout.split.separator) * context->layout.split.weight + 0.5f;
			const int32_t rightWidth =  width - context->layout.split.separator - leftWidth;
			ans.x =             (leftWidth + context->layout.split.separator) * index;
			ans.y = 0;
			ans.z = leftWidth + (rightWidth + context->layout.split.separator) * index;
			ans.w = height;
			} 
			break;
		}
		{
			const float midX = (float)(ans.x + ans.z) / 2.0f;
			const float midY = (ans.w + ans.y) / 2;
			const float width  = std::min<float>((ans.z - ans.x) / 2.0f + 0.5f, context->layout.max.x / 2);
			const float height = std::min<float>((ans.w - ans.y) / 2.0f + 0.5f, context->layout.max.y / 2);
			ans.x = midX - width;
			ans.y = midY - height;
			ans.z = midX + width;
			ans.w = midY + height;
		}
		}
		break;
	case GUI_LAYOUT_BORDER :
		{
		const float width  = context->viewport.z - context->viewport.x;
		const float height = context->viewport.w - context->viewport.y;
		const int vertical = (context->layout.border.orientation == GUI_VERTICAL);
		switch(context->layout.elementIndex % 3) {
		case 0 : // header
			ans.x = 0;
			ans.y = 0;
			ans.z = vertical ? width : ((float)width * context->layout.border.header + 0.5f);
			ans.w = vertical ? ((float)height * context->layout.border.header + 0.5f) : height;
			break;
		case 1 : // body
			ans.x = vertical ? 0 : ((float)width * context->layout.border.header + 0.5f);
			ans.y = vertical ? ((float)height * context->layout.border.header + 0.5f) : 0;
			ans.z = vertical ? width : ans.x + ((float)width * context->layout.border.body + 0.5f);
			ans.w = vertical ? ans.y + ((float)height * context->layout.border.body + 0.5f) : height;
			break;
		case 2 : // footer
			ans.x = vertical ? 0 : width - ((float)width * context->layout.border.footer + 0.5f);
			ans.y = vertical ? height - ((float)height * context->layout.border.footer + 0.5f) : 0;
			ans.z = width;
			ans.w = height;
			break;
		}
		}
		{ // TMP: clamp max Y size
			const int32_t midX = (ans.x + ans.z) / 2;
			const int32_t midY = (ans.w + ans.y) / 2;
			int32_t width  = std::min<int>(ans.z - ans.x, context->layout.max.x);
			int32_t height = std::min<int>(ans.w - ans.y, context->layout.max.y);
			ans.x = midX - width / 2;
			ans.y = midY - height / 2;
			ans.z = midX + width / 2;
			ans.w = midY + height / 2;
		}
		break;
	case GUI_LAYOUT_GRID:
		{
		const int32_t indexX = (context->layout.elementIndex % context->layout.grid.count.x) % context->layout.grid.count.x;
		const int32_t indexY = (context->layout.elementIndex / context->layout.grid.count.x) % context->layout.grid.count.y;
		ans.x = (indexX + 0) * context->layout.grid.cellSize.x,
		ans.y = (indexY + 0) * context->layout.grid.cellSize.y,
		ans.z = (indexX + 1) * context->layout.grid.cellSize.x,
		ans.w = (indexY + 1) * context->layout.grid.cellSize.y;

		const float width  = std::min(ans.z - ans.x, (int)context->layout.max.x);
		const float height = std::min(ans.w - ans.y, (int)context->layout.max.y);
		const float midX = (ans.z + ans.x) / 2;
		const float midY = (ans.w + ans.y) / 2;
		ans.x = midX - width  / 2.0f;
		ans.y = midY - height / 2.0f;
		ans.z = midX + width  / 2.0f;
		ans.w = midY + height / 2.0f;
		}
		break;
	default :
		assert(!"Invalid branch!");
	}

	ans.x += context->layout.margin;
	ans.y += context->layout.margin;
	ans.z -= context->layout.margin;
	ans.w -= context->layout.margin;
	context->layout.elementIndex += advance;

	return ans;
}

ivec4 LayoutGetAbsoluteBounds(bool advance) {
	ivec4 ans = LayoutGetBounds(advance);
	ans.x += context->viewport.x;
	ans.y += context->viewport.y;
	ans.z += context->viewport.x;
	ans.w += context->viewport.y;
	return ans;
}

Layout AbsoluteLayout(uint32_t margin) {
	Layout ans = {0};
	ans.type = GUI_LAYOUT_ABSOLUTE;
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
	ans.grid.cellSize.x = 1;
	ans.grid.cellSize.y = 1;
	ans.margin = margin;
	ans.grid.count.x = std::max<int>(x, 1);
	ans.grid.count.y = std::max<int>(y, 1);
	return ans;
}

Layout SplitLayout(GUIOrientation orientation, float weight, uint32_t separator, uint32_t margin) {
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

Layout FixSplitLayout(GUIOrientation orientation, int32_t size, uint32_t separator, uint32_t margin) {
	const ivec4 bounds = LayoutGetBounds(false);
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

Layout BorderLayout(GUIOrientation orientation, float headerWeight, float footerWeight, uint32_t margin) {
	Layout ans = {};
	ans.type = GUI_LAYOUT_BORDER;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.border.header = headerWeight;
	ans.border.footer = footerWeight;
	ans.border.orientation = orientation;
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

void Label(const char* text, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);

	LabelInternal(flags, absoluteBounds);
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawText(text, absoluteBounds, context->style.colors[GUI_COLOR_TEXT], (flags & GUI_ALIGN_CENTER));
	}
}

void Label(int32_t id, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);

	LabelInternal(flags, absoluteBounds);
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawIcon(id, absoluteBounds, context->style.colors[GUI_COLOR_TEXT]);
	}
}

bool ButtonInternal(uint32_t flags, const ivec4& bounds) {
	ivec4 cbounds = bounds;
	if (RectGetArea(context->clip) > 0) {
		RectClip(cbounds, context->clip);
	}
	
	const bool focused = ((RECT_CONTAINS_POINT(cbounds, GetMousePosition()) && (flags & GUI_ENABLED)) || (flags & GUI_FOCUSED));
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

bool Button(int32_t icon, const char* text, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	bool ans = false;

	if (ButtonInternal(flags, absoluteBounds)) {
		ans = true;
	}
	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		const int width = absoluteBounds.z - absoluteBounds.x;
		const int height = absoluteBounds.w - absoluteBounds.y;
		const int min = width < height ? width : height;
		GUIDrawIcon(icon, {absoluteBounds.x, absoluteBounds.y, absoluteBounds.x + min, absoluteBounds.y + min}, context->style.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED]);
		GUIDrawText(text, {absoluteBounds.x + min, absoluteBounds.y, absoluteBounds.z, absoluteBounds.w}, context->style.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
	}

	return ans;
}

bool Button(const char* text, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	const bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawText(text, absoluteBounds, context->style.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED], (flags & GUI_ALIGN_CENTER));
	}

	return ans;
}

bool Button(int32_t icon, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	const bool ans = ButtonInternal(flags, absoluteBounds);

	if ((flags & GUI_VISIBLE) && (flags & GUI_FOREGROUND)) {
		GUIDrawIcon(icon, absoluteBounds, context->style.colors[(flags & GUI_ENABLED) ? GUI_COLOR_TEXT : GUI_COLOR_TEXT_DISABLED]/*, (flags & GUI_ALIGN_CENTER) */);
	}

	return ans;
}

bool Button(int32_t icon, uint32_t* flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
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

bool CheckBox(bool &checked, uint32_t flags) {
	context->layout.max = {22, 22};
	ivec4 rect = LayoutGetAbsoluteBounds(true);
	context->layout.max = {0x1FFFFFFF, 0x1FFFFFFF};

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

bool Toggle(bool &enabled, uint32_t flags) {
	static const int32_t PADDING = 2;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	const int32_t halfWidth = (absoluteBounds.z - absoluteBounds.x) / 2;
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

void ProgressBar(float proc, uint32_t flags) {
	static const int32_t K = 2;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
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
			GUIDrawQuad(absoluteBounds, context->style.colors[GUI_COLOR_PANE]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(absoluteBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
		if (flags & GUI_FOREGROUND) {
			GUIDrawQuad(barBounds, context->style.colors[GUI_COLOR_ACTIVE]);
			GUIDrawBorder(barBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}
}

bool SliderInternal(float& proc, float step, int32_t boxLength, GUIOrientation orientation, uint32_t flags, const ivec4& bounds) {
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
				GUIDrawQuad({begin.x - 0, begin.y,     end.x + 1, boxBounds.y}, context->style.colors[GUI_COLOR_PANE]);
				GUIDrawQuad({begin.x - 1, boxBounds.w, end.x + 2, end.y      }, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				break;
			case GUI_HORIZONTAL:
				GUIDrawQuad({begin.x,     begin.y - 1, boxBounds.x, end.y + 2}, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				GUIDrawQuad({boxBounds.z, begin.y - 0, end.x, end.y + 1      }, context->style.colors[GUI_COLOR_PANE]);
				break;
			}
		}
		if (flags & GUI_FOREGROUND) {
			GUIDrawQuad(boxBounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
			GUIDrawBorder(boxBounds, context->style.colors[GUI_COLOR_BORDER]);
		}
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(bounds, context->style.colors[GUI_COLOR_BORDER]);
		}
	}

	return ans;
}

bool Slider(float& proc, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	const int32_t width = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;

	return SliderInternal(proc, 0.1f, context->style.values[GUI_VALUE_SLIDER_WIDTH], (height > width) ? GUI_VERTICAL : GUI_HORIZONTAL, flags, absoluteBounds);
}

bool Slider(float& proc, uint32_t boxLength, GUIOrientation orientation, uint32_t flags) {
	return SliderInternal(proc, 0.1f, boxLength, orientation, flags, LayoutGetAbsoluteBounds(true));
}

bool RangeSliderInternal(float& procMin, float& procMax, float step, int32_t boxLength, GUIOrientation orientation, uint32_t flags, const ivec4& bounds) {
	const int32_t width  = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;
	ivec4 boxBounds[2];
	ivec2 begin;
	ivec2 end;
	bool ans = false;
	bool selectedLeft;

	switch (orientation) {
	case GUI_VERTICAL:
		{
		const int32_t mid = (bounds.x + bounds.z) / 2;
		begin = {mid, bounds.y};
		end = {mid, bounds.w};
		int32_t k0 = (float)(height - boxLength) * procMin;
		int32_t k1 = (float)(height - boxLength) * procMax;
		boxBounds[0] = {bounds.x, bounds.w - k0 - boxLength, bounds.z, bounds.w - k0};
		boxBounds[1] = {bounds.x, bounds.w - k1 - boxLength, bounds.z, bounds.w - k1};
		selectedLeft = abs(GetMousePosition().y - boxBounds[0].w) < abs(GetMousePosition().y - boxBounds[1].y);
		}
		break;
	case GUI_HORIZONTAL:
		{
		const int32_t mid = (bounds.w + bounds.y) / 2;
		begin = {bounds.x, mid};
		end = {bounds.z, mid};
		int32_t k0 = (float)(width - boxLength) * procMin;
		int32_t k1 = (float)(width - boxLength) * procMax;
		boxBounds[0] = {bounds.x + k0, bounds.y, bounds.x + k0 + boxLength, bounds.w};
		boxBounds[1] = {bounds.x + k1, bounds.y, bounds.x + k1 + boxLength, bounds.w};
		selectedLeft = abs(GetMousePosition().x - boxBounds[0].z) < abs(GetMousePosition().x - boxBounds[1].x);
		}
		break;
	default:
		assert(false);
		break;
	}
	const bool focused = (flags & GUI_ENABLED) && (RECT_CONTAINS_POINT(bounds, GetMousePosition()) || RECT_CONTAINS_POINT(bounds, GetLastMousePosition()));
	const bool clicked = (focused && GetMouseLeftButton());

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
	if (clicked) {
		float newValue;
		switch (orientation) {
		case GUI_VERTICAL :
			if (GetMousePosition().y <= boxBounds[0].w) {
				newValue = (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(height - boxLength);
				procMin = std::clamp(newValue, 0.0f, procMax);
				ans = true;
			} else if (GetMousePosition().y >= boxBounds[1].y) {
				newValue = (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(height - boxLength);
				procMax = std::clamp(newValue, procMin, 1.0f);
				ans = true;
			} else {
				if (abs(GetMousePosition().y - boxBounds[0].w) < abs(GetMousePosition().y - boxBounds[1].y)) {
					newValue = (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(height - boxLength);
					procMin = std::clamp(newValue, 0.0f, procMax);
					ans = true;
				} else {
					newValue = (float)(GetMousePosition().y - boxLength / 2 - bounds.y) / (float)(height - boxLength);
					procMax = std::clamp(newValue, procMin, 1.0f);
					ans = true;
				}
			}
			break;
		case GUI_HORIZONTAL :
			if (GetMousePosition().x <= boxBounds[0].z) {
				newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
				procMin = std::clamp(newValue, 0.0f, procMax);
				ans = true;
			} else if (GetMousePosition().x >= boxBounds[1].x) {
				newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
				procMax = std::clamp(newValue, procMin, 1.0f);
				ans = true;
			} else {
				if (abs(GetMousePosition().x - boxBounds[0].z) < abs(GetMousePosition().x - boxBounds[1].x)) {
					newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
					procMin = std::clamp(newValue, 0.0f, procMax);
					ans = true;
				} else {
					newValue = (float)(GetMousePosition().x - boxLength / 2 - bounds.x) / (float)(width - boxLength);
					procMax = std::clamp(newValue, procMin, 1.0f);
					ans = true;
				}
			}
			break;
		}
	}
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_BACKGROUND) {
			switch (orientation) {
			case GUI_VERTICAL:
				GUIDrawQuad({begin.x,     begin.y, boxBounds[0].x, end.y + 1}, context->style.colors[GUI_COLOR_PANE]);
				GUIDrawQuad({boxBounds[0].z, begin.y - 1, boxBounds[1].x, end.y + 2}, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				GUIDrawQuad({boxBounds[1].z, begin.y, end.x, end.y + 1}, context->style.colors[GUI_COLOR_PANE]);
				break;
			case GUI_HORIZONTAL:
				GUIDrawQuad({begin.x,     begin.y, boxBounds[0].x, end.y + 1}, context->style.colors[GUI_COLOR_PANE]);
				GUIDrawQuad({boxBounds[0].z, begin.y - 1, boxBounds[1].x, end.y + 2}, context->style.colors[clicked ? GUI_COLOR_ACTIVE : GUI_COLOR_FOCUSED]);
				GUIDrawQuad({boxBounds[1].z, begin.y, end.x, end.y + 1}, context->style.colors[GUI_COLOR_PANE]);
				break;
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

bool RangeSlider(float& procMin, float& procMax, GUIOrientation orientation, uint32_t flags) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	
	return RangeSliderInternal(procMin, procMax, 0.1f, context->style.values[GUI_VALUE_SLIDER_WIDTH], orientation, flags, absoluteBounds);
}

bool TextBox(char* text, const uint32_t max_length, int& carrot, uint32_t flags, uint32_t padding) {
	const int32_t CARROT_WIDTH = 1;
	const int32_t FONT_HEIGHT = 16;

	static time_t lastBlinkTime = 0;

	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(true);
	const ivec4 textBounds = {absoluteBounds.x + (int)padding, absoluteBounds.y + (int)padding, absoluteBounds.z - (int)padding, absoluteBounds.w - (int)padding};
	const int32_t width = textBounds.z - textBounds.x;
	const int32_t height = textBounds.w - textBounds.y;
	const bool focused = RECT_CONTAINS_POINT(textBounds, GetMousePosition(true)) && context->events_enabled;
	const int offsetY = GetLineYOffset(text, textBounds, flags);
	int offsetX = 0;
	
	int32_t length = strlen(text);
	int32_t offset = 0;
	bool ans = false;

	// Offset text to keep carrot in viewport
	if (carrot > 0) {
		ivec2 pos = context->getTextSize(context, text, carrot);
		while (pos.x > width) {
			pos.x -= context->getTextSize(context, text, 1).x;
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
					int32_t charWidth = context->getTextSize(context, text + carrot, 1).x;
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
		for (uint32_t index = 0; index < context->keyEventCount; ++index) {
			switch (context->keyEvents[index].type) {
			case GUI_EVENT_KEY_DOWN :
				if (context->keyEvents[index].value == context->keyMap[GUI_KEY_HOME]) {
					carrot = MoveCursor(text, carrot, CUR_POS_BEGIN);
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_END]) {
					carrot = MoveCursor(text, carrot, CUR_POS_END);
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_UP]) {
					carrot = MoveCursor(text, carrot, CUR_POS_PREV_LINE);
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_DOWN]) {
					carrot = MoveCursor(text, carrot, CUR_POS_NEXT_LINE);
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_LEFT]) {
					carrot = std::max(carrot - 1, 0);
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_RIGHT]) {
					carrot = std::min(carrot + 1, length);
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_BACK]) {
					if (carrot > 0) {
						for (int32_t index = --carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						ans = true;
					}
					lastBlinkTime = time(NULL);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_DELETE]) {
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
				switch (context->keyEvents[index].value) {
				case '\n' : if ((flags & GUI_MULTILINE) == 0) break;
				default :
					if (length < (int)max_length - 1) {
						for (int32_t index = ++length; index >= carrot; --index) {
							text[index] = text[index - 1];
						}
						text[carrot++] = (char)context->keyEvents[index].value;
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
			GUIDrawQuad(absoluteBounds, context->style.colors[(carrot >= 0) ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
		}

		if (flags & GUI_FOREGROUND) {
			if (height >= FONT_HEIGHT) {
				if (flags & GUI_HIDDEN) {
					// TODO: DrawIcon(GUI_ICON_CLOSE) ?
				} else {
					GUIDrawText(text + (offset > 0) * offset, textBounds, context->style.colors[GUI_COLOR_TEXT], (flags & (GUI_MULTILINE | GUI_ALIGN_CENTER)));
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
						carrotPosition.x += context->getCharSize(context, text[index]).x;
						break;
					}
				}
				int carrotHeight = std::min(height, FONT_HEIGHT);
				
				
				offsetX = ((flags & GUI_MULTILINE) ? GetLineXOffset(text + MoveCursor(text, carrot, CUR_POS_BEGIN), textBounds, flags) : textBounds.x);
				GUIDrawQuad({offsetX + carrotPosition.x, offsetY + carrotPosition.y, offsetX + carrotPosition.x + CARROT_WIDTH, offsetY + carrotPosition.y + carrotHeight}, context->style.colors[GUI_COLOR_TEXT]);
			}
		}

		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(absoluteBounds, context->style.colors[(carrot >= 0) ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_BORDER]);
		}
	}

	return ans;
}

bool TextArea(char* text, const uint32_t max_length, int& carrot, uint32_t flags, uint32_t padding) {
	bool ans = false;
	static int offsetX = 0;
	static int offsetY = 0;
	
	ivec2 size = context->getTextSize(context, text, strlen(text));
	size.x += padding * 2;
	size.y += padding * 2;
	
	ScrollPanel(size.x, size.y, &offsetX, &offsetY, 0, GUI_FLAGS_PANEL) {
		ans = TextBox(text, max_length, carrot, flags);
	}
	
	return ans;
}

bool SpinnerInternal(int& value, const char* text, uint32_t flags) {
	static const uint32_t LABEL_FLAG_MASK = GUI_VISIBLE | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_MASK;
	static const uint32_t BUTTON_FLAG_MASK = GUI_VISIBLE | GUI_ENABLED;
	static const uint32_t BUTTON_FLAGS = GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER;
	static const uint32_t button_flags = BUTTON_FLAGS | (flags & BUTTON_FLAG_MASK);

	const ivec4 bounds = LayoutGetBounds(false);
	const int32_t buttonWidth = std::min(bounds.z - bounds.x, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	const float buttonWidthProc = (float)buttonWidth/ (float)(bounds.z - bounds.x);
	const int oldValue = value;

	Panel(BorderLayout(GUI_HORIZONTAL, buttonWidthProc, buttonWidthProc, 0)) {
		value -= Button(GUI_ICON_ARROW_LEFT, button_flags);
		Label(text, flags & LABEL_FLAG_MASK);
		value += Button(GUI_ICON_ARROW_RIGHT, button_flags);
	}

	return (oldValue != value);
}


bool Spinner(int& value, uint32_t flags) {
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "%d", value);

	return SpinnerInternal(value, tmp, flags);
}

bool Spinner(int& value, const char** labels, uint32_t count, uint32_t flags) {
	if (SpinnerInternal(value, labels[std::clamp<int>(value, 0, count - 1)], flags)) {
		value = std::clamp<int>(value, 0, count - 1);
		return true;
	}
	return false;
}

bool Scrollbar(float& value, float barProc, GUIOrientation orientation, float step) {
	const ivec4 rbounds = LayoutGetBounds(false);
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
	const float buttonProc = 16.0f / (float)length;

	Panel(BorderLayout(orientation, buttonProc, buttonProc, 0), 0, GUI_FLAGS_PANEL) {
		if (Button(buttonIcons[0])) {
			value += dir ? step : - step;
			ans = true;
		}
		if (Slider(value, barLength * barProc, orientation, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE)) {
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

Layout BeginPanel(const Layout& layout, uint32_t padding, uint32_t flags) {
	context->layout.backup_viewport = context->viewport;
	context->layout.backup_clip = context->clip;

	context->viewport = LayoutGetAbsoluteBounds(true);
	const int32_t width = context->viewport.z - context->viewport.x;
	const int32_t height = context->viewport.w - context->viewport.y;

	Layout ans = context->layout;
	ans.run_statement = (width >= (int32_t)padding * 2) && (height >= (int32_t)padding * 2);
	if (ans.run_statement == false) {
		return ans;
	}
	
	if (padding) {
		context->viewport.x = context->viewport.x + padding;
		context->viewport.y = context->viewport.y + padding;
		context->viewport.z = context->viewport.z - padding;
		context->viewport.w = context->viewport.w - padding;
	}

	if (RectGetArea(context->clip) > 0) {
		context->clip = {
			std::max(context->clip.x, context->viewport.x),
			std::max(context->clip.y, context->viewport.y),
			std::min(context->clip.z, context->viewport.z),
			std::min(context->clip.w, context->viewport.w)
		};
	} else {
		context->clip = context->viewport;
	}
	
	SetLayout(layout);

	return ans;
}

void EndPanel(Layout* bkp_layout) {
	bkp_layout->run_statement = false;

	context->layout = *bkp_layout;
	context->viewport = context->layout.backup_viewport;
	context->clip = context->layout.backup_clip;
}

Layout BeginSplitPanel(GUIOrientation orientation, float& weight, uint32_t padding, uint32_t flags) {
	static const int32_t SEPARATOR = 4;
	const Layout ans = BeginPanel(SplitLayout(orientation, weight, SEPARATOR), padding, flags);
	const ivec4 absoluteBounds = context->viewport; // DAFUQ ?!
	const int32_t width  = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;
	const int32_t halfPadding = SEPARATOR / 2;
	ivec4 separatorBounds = absoluteBounds;

	switch (orientation) {
	case GUI_VERTICAL :
		separatorBounds.y += (float)(height - SEPARATOR) * weight;
		separatorBounds.w = separatorBounds.y + SEPARATOR;
		break;
	case GUI_HORIZONTAL :
		separatorBounds.x += (float)(width - SEPARATOR) * weight;
		separatorBounds.z = separatorBounds.x + SEPARATOR;
		break;
	default:
		assert(false);
	}

	const bool focused = (RECT_CONTAINS_POINT(separatorBounds, GetMousePosition()) || RECT_CONTAINS_POINT(separatorBounds, GetLastMousePosition()));
	const bool clicked = focused && GetMouseLeftButton();

	if (clicked) {
		switch (orientation) {
		case GUI_VERTICAL :
			weight = (float)(GetMousePosition().y - halfPadding - absoluteBounds.y) / (float)(height - SEPARATOR);
			break;
		case GUI_HORIZONTAL:
			weight = (float)(GetMousePosition().x - halfPadding - absoluteBounds.x) / (float)(width - SEPARATOR);
			
			break;
		}
		weight = std::clamp(weight, 0.01f, 0.99f);
	}
	GUIDrawQuad(separatorBounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);

	return ans;
}

// ScrollPanel components
inline void ScrollPanelClient(ivec4& bounds) {
	bounds = LayoutGetAbsoluteBounds(true);
}

inline void ScrollPanelHorizontal(ivec4& clientBounds, int32_t width, int* offset) {
	static const int HEIGHT = 18;

	const ivec4 bounds = LayoutGetBounds(false);
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

	const ivec4 bounds = LayoutGetBounds(false);
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

Layout BeginScrollPanel(int width, int height, int* offsetX, int* offsetY, uint32_t margin, uint32_t flags) {
	Layout ans = BeginPanel();
	const ivec4 bounds = LayoutGetBounds(false);
	ivec4 clientBounds;

	ScrollPanelVertical(clientBounds, width, height, offsetX, offsetY);

	RectClip(clientBounds, context->clip);
	context->clip = clientBounds;

	context->viewport.x = clientBounds.x - (offsetX ? *offsetX : 0);
	context->viewport.y = clientBounds.y - (offsetY ? *offsetY : 0);
	context->viewport.z = context->viewport.x + (width == 0 ? bounds.z - bounds.x : width);
	context->viewport.w = context->viewport.y + (height == 0 ? bounds.w - bounds.y : height);

	return ans;
}

Layout BeginTabPanel(const char* names, int& selected, uint32_t margin, uint32_t flags) {
	uint32_t count = (*names != '\0');
	for(const char* ptr = names, *begin = ptr; *ptr !='\0'; ++ptr) {
		if (*ptr == ',') {
			if (ptr - begin) {
				count += 1;
			}
			begin = ptr + 1;
		}
	}

	ivec4 absoluteRectangle = LayoutGetBounds(false);
	const int32_t height = absoluteRectangle.w - absoluteRectangle.y;
	const int32_t tabHeight = std::min(absoluteRectangle.w - absoluteRectangle.y, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);
	const float tabHeightProc = (float)tabHeight / (float)height;
	Layout ans = BeginPanel(SplitLayout(GUI_VERTICAL, tabHeightProc, 0, 0));
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

	absoluteRectangle = LayoutGetAbsoluteBounds(false);
	GUIDrawBorder(absoluteRectangle, context->style.colors[GUI_COLOR_BORDER]);
	GUIDrawQuad(absoluteRectangle, context->style.colors[GUI_COLOR_PANEL]);

	return ans;
}

Layout BeginWindow(ivec4* rbounds, const char* title, uint32_t margin, uint32_t* flags) {
	assert(context != NULL);

	const int32_t TitlebarHeight = context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
	// ivec4 bounds = LayoutGetAbsoluteBounds(*rbounds, true);
	ivec4 bounds = *rbounds;
	ivec4 contentBounds = bounds;

	uint32_t defaultFlags = GUI_FLAGS_WINDOW  & ~GUI_WINDOW_CLOSE;
	Layout ans = BeginPanel();
	context->viewport = bounds;

	if (flags == NULL) {
		flags = & defaultFlags;
	} else if ((*flags & GUI_VISIBLE) == 0) {
		ans.run_statement = false;
		EndPanel(&ans);
		return ans;
	}

	if (*flags & GUI_BACKGROUND) {
		GUIDrawQuad(bounds, context->style.colors[GUI_COLOR_PANEL]);
	}

	if (*flags & GUI_WINDOW_TITLEBAR) {
		const int height = bounds.w - bounds.y;
		const int width  = bounds.z - bounds.x;

		Panel(FixSplitLayout(GUI_VERTICAL, TitlebarHeight, 0, 0)) {
			// Top element
			Panel(FixSplitLayout(GUI_HORIZONTAL, -TitlebarHeight, 0, 0)) {
				// Left element
				ivec4 titlebarBounds = LayoutGetAbsoluteBounds(false);
				GUIDrawQuad(titlebarBounds, context->style.colors[GUI_COLOR_TITLEBAR]);
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
			contentBounds = LayoutGetAbsoluteBounds(true);
		}
	} else {
		// TODO
	}
	context->viewport = contentBounds;

	if (*flags & GUI_WINDOW_SIZE) {
		const int32_t width  = context->viewport.z - context->viewport.x;
		const int32_t height = context->viewport.w - context->viewport.y;
		Panel(FixSplitLayout(GUI_VERTICAL, height - TitlebarHeight, 0, 0)) {
			// Top element
			contentBounds = LayoutGetAbsoluteBounds(true);

			// Bottom element
			Panel(FixSplitLayout(GUI_HORIZONTAL, width - TitlebarHeight, 0)) {
				//DummyElement();
				Label("Some status text goes here");
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
				GUIDrawIcon(GUI_ICON_SIZE, sizeBounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
			}
		}
		context->viewport = contentBounds;
	}

	if (*flags & GUI_OUTLINE) {
		GUIDrawBorder(bounds, context->style.colors[GUI_COLOR_BORDER]);
		context->viewport.x += 1;
		context->viewport.y += 1;
		context->viewport.z -= 1;
		context->viewport.w -= 1;
	}

	*rbounds = bounds;
	return ans;
}

void SetLayout(const Layout& layout) {
	context->layout = layout;
	context->layout.elementIndex = 0;
	switch (context->layout.type) {
	case GUI_LAYOUT_BORDER :
		if (context->layout.border.header + context->layout.border.body > 1.0f) {
			assert(context->layout.border.header + context->layout.border.body <= 1.0f);
		}
		context->layout.border.body = 1.0f - (context->layout.border.header + context->layout.border.footer);
		break;
	case GUI_LAYOUT_GRID:
		context->layout.grid.cellSize.x = (context->viewport.z - context->viewport.x) / context->layout.grid.count.x;
		context->layout.grid.cellSize.y = (context->viewport.w - context->viewport.y) / context->layout.grid.count.y;
		break;
	case GUI_LAYOUT_SPLIT :
		if (context->layout.split.weight < 0) {
			context->layout.split.weight = 1.0f + context->layout.split.weight;
			//context->layout.elementIndex += 1;
		}
		break;
	}
}

Layout* GetLayout() {
	return &context->layout;
}

#undef MIN
#undef MIN2
#undef MIN3
#undef RECT_CONTAINS_POINT
