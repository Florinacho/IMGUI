#include "imgui.h"

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static GUIContext* context = NULL;

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
	if ((button == GUI_BUTTON_LEFT) && (value == GUI_BUTTON_PRESSED)) {
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

void GUIDrawText(const char* text, const ivec4& rect, const color_t& color, uint32_t flags) {
	ivec4 bounds = rect;
	if (context == NULL) {
		return;
	}
	// Clipping is done in the renderer
	context->drawText(context, text, bounds, color, flags);
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
	
	context->style.colors[GUI_COLOR_TEXT]          = {230, 230, 230, 255};
	context->style.colors[GUI_COLOR_TEXT_DISABLED] = {150, 150, 150, 255};
	context->style.colors[GUI_COLOR_TITLEBAR]      = { 25,  25,  25, 200};
	context->style.colors[GUI_COLOR_PANEL]         = { 50,  50,  50, 255};
	context->style.colors[GUI_COLOR_PANE]          = { 75,  75,  75, 255};
	context->style.colors[GUI_COLOR_FOCUSED]       = {110, 110, 120, 255};
	context->style.colors[GUI_COLOR_ACTIVE]        = { 75,  75, 255, 255};
	context->style.colors[GUI_COLOR_BORDER]        = { 25,  25,  25, 255};
	
	context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] = 20;
	context->style.values[GUI_VALUE_SCROLLBAR_WIDTH] = 16;
	context->style.values[GUI_VALUE_SLIDER_WIDTH]    =  7;
	context->style.values[GUI_VALUE_WINDOW_PADDING]  =  4;

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

ivec4 LayoutGetBounds(const ivec4& bounds, bool advance) {
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
		switch (context->layout.split.orientation) {
		case GUI_VERTICAL :
			{
			const int32_t topCellHeight = (height - context->layout.split.separator) * context->layout.split.weight;
			const int32_t botCellHeight = height - context->layout.split.separator - topCellHeight;
			ans.x = 0;
			ans.y =                 (topCellHeight + context->layout.split.separator) * index;
			ans.z = width;
			ans.w = topCellHeight + (context->layout.split.separator + botCellHeight) * index;
			}
			break;
		case GUI_HORIZONTAL :
			{
			const int32_t topCellWidth = (width - context->layout.split.separator) * context->layout.split.weight;
			const int32_t botCellWidth =  width - context->layout.split.separator - topCellWidth;
			ans.x =                (topCellWidth + context->layout.split.separator) * index;
			ans.y = 0;
			ans.z = topCellWidth + (botCellWidth + context->layout.split.separator) * index;
			ans.w = height;
			} 
			break;
		}
		if (ans.w < ans.y) {
			ans.w = ans.y;
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
		}
		break;
	case GUI_LAYOUT_BORDER :
		{
		const int32_t width  = context->viewport.z - context->viewport.x;
		const int32_t height = context->viewport.w - context->viewport.y;
		const int vertical = (context->layout.border.orientation == GUI_VERTICAL);
		switch(context->layout.elementIndex % 3) {
		case 0 : // header
			ans.x = 0;
			ans.y = 0;
			ans.z = vertical ? width : ((float)width * context->layout.border.header);
			ans.w = vertical ? ((float)height * context->layout.border.header) : height;
			break;
		case 1 : // body
			ans.x = vertical ? 0 : ((float)width * context->layout.border.header);
			ans.y = vertical ? ((float)height * context->layout.border.header) : 0;
			ans.z = vertical ? width : ans.x + ((float)width * context->layout.border.body);
			ans.w = vertical ? ans.y + ((float)height * context->layout.border.body) : height;
			break;
		case 2 : // footer
			ans.x = vertical ? 0 : width - ((float)width * context->layout.border.footer);
			ans.y = vertical ? height - ((float)height * context->layout.border.footer) : 0;
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

		const int32_t width  = std::min(ans.z - ans.x, (int)context->layout.max.x);
		const int32_t height = std::min(ans.w - ans.y, (int)context->layout.max.y);
		const int32_t midX = (ans.z + ans.x) / 2;
		const int32_t midY = (ans.w + ans.y) / 2;
		ans.x = midX - width  / 2;
		ans.y = midY - height / 2;
		ans.z = midX + width  / 2;
		ans.w = midY + height / 2;
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

ivec4 LayoutGetAbsoluteBounds(const ivec4& bounds, bool advance) {
	ivec4 ans = LayoutGetBounds(bounds, advance);
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
	const ivec4 bounds = LayoutGetBounds({}, false);
	const int32_t width = bounds.z - bounds.x;
	const int32_t height = bounds.w - bounds.y;
	
	Layout ans = {};
	ans.type = GUI_LAYOUT_SPLIT;
	ans.max.x = 0x1FFFFFFF;
	ans.max.y = 0x1FFFFFFF;
	ans.margin = margin;
	ans.split.orientation = orientation;
	ans.split.separator = separator;
	switch (orientation) {
	case GUI_VERTICAL :
		ans.split.weight = std::min<float>((float)size / (float)height,  1.0f);
		break;
	case GUI_HORIZONTAL :
		ans.split.weight = std::min<float>((float)size / (float)width, 1.0f);
		break;
	}
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

bool Button(int32_t icon, const char* text, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
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
#if 0
int Grid(uint32_t row, uint32_t collumn, bool square, uint32_t flags, const ivec4& bounds) {
	ivec4 absoluteBounds = bounds;//LayoutGetAbsoluteBounds(bounds, true);

	const int32_t width = ((absoluteBounds.z - absoluteBounds.x) / collumn) * collumn;
	const int32_t height = ((absoluteBounds.w - absoluteBounds.y) / row) * row;
	int32_t length = ((width < height) ? width : height);
	
	if (square) {
		const int32_t midX = (absoluteBounds.z + absoluteBounds.x) / 2;
		const int32_t midY = (absoluteBounds.w + absoluteBounds.y) / 2;
		absoluteBounds.x = midX - length / 2;
		absoluteBounds.y = midY - length / 2;
		absoluteBounds.z = midX + length / 2;
		absoluteBounds.w = midY + length / 2;
	}

	length = length / row;

	const bool focused = (RECT_CONTAINS_POINT(absoluteBounds, GetMousePosition()) && (flags & GUI_ENABLED));
	const bool clicked = (focused && GetMouseLeftButton());
	
	if (flags & GUI_VISIBLE) {
		if (flags & GUI_OUTLINE) {
			GUIDrawBorder(absoluteBounds, context->style.colors[GUI_COLOR_BORDER]);

			for (uint32_t y = 1; y < row; ++y) {
				int32_t ny = y * length;
				const ivec2 begin = {absoluteBounds.x, absoluteBounds.y + ny};
				const ivec2 end = {absoluteBounds.z, absoluteBounds.y + ny};
				GUIDrawLine(begin, end, context->style.colors[GUI_COLOR_BORDER]);
			}
			for (uint32_t x = 1; x < collumn; ++x) {
				int32_t nx = x * length;
				const ivec2 begin = {absoluteBounds.x + nx, absoluteBounds.y};
				const ivec2 end = {absoluteBounds.x + nx, absoluteBounds.w};
				GUIDrawLine(begin, end, context->style.colors[GUI_COLOR_BORDER]);
			}
		}
	}
	
	if (clicked) {
		const ivec2 pos = GetMousePosition();
		const int x = (pos.x - absoluteBounds.x) / length;
		const int y = (pos.y - absoluteBounds.y) / length;
		return y * collumn + x;
	}
	
	return -1;
}

int Canvas(uint32_t row, uint32_t collumn, const color_t* palette, const uint8_t* pattern, uint32_t flags, const ivec4& bounds) {
	ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);

	const int32_t width = ((absoluteBounds.z - absoluteBounds.x) / collumn) * collumn;
	const int32_t height = ((absoluteBounds.w - absoluteBounds.y) / row) * row;
	int32_t length = ((width < height) ? width : height);

	const int32_t midX = (absoluteBounds.z + absoluteBounds.x) / 2;
	const int32_t midY = (absoluteBounds.w + absoluteBounds.y) / 2;
	absoluteBounds.x = midX - length / 2;
	absoluteBounds.y = midY - length / 2;
	absoluteBounds.z = midX + length / 2;
	absoluteBounds.w = midY + length / 2;

	length /= row;

	for (int y = 0; y < (int)row; ++y) {
		for (int x = 0; x < (int)collumn; ++x) {
			const int index = y * collumn + x;
			const ivec4 c = palette[pattern[index]];
			GUIDrawQuad({absoluteBounds.x + x * length,
						 absoluteBounds.y + y * length,
						 absoluteBounds.x + (x + 1) * length, 
						 absoluteBounds.y + (y + 1) * length}, 
						 {c.x, c.y, c.z, 255});
		}
	}
	
	return Grid(row, collumn, false, flags, absoluteBounds);
}
#endif
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
	context->layout.max = {22, 22};
	ivec4 rect = LayoutGetAbsoluteBounds(nbounds, true);
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

bool Toggle(bool &enabled, uint32_t flags, const ivec4& nbounds) {
	static const int32_t PADDING = 2;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(nbounds, true);
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

void ProgressBar(float proc, uint32_t flags, const ivec4& bounds) {
	static const int32_t K = 2;
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
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

	return SliderInternal(proc, 0.1f, context->style.values[GUI_VALUE_SLIDER_WIDTH], (height > width) ? GUI_VERTICAL : GUI_HORIZONTAL, flags, absoluteBounds);
}

bool Slider(float& proc, uint32_t boxLength, GUIOrientation orientation, uint32_t flags, const ivec4& bounds) {
	return SliderInternal(proc, 0.1f, boxLength, orientation, flags, LayoutGetAbsoluteBounds(bounds, true));
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

bool RangeSlider(float& procMin, float& procMax, GUIOrientation orientation, uint32_t flags, const ivec4& bounds) {
	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);
	
	return RangeSliderInternal(procMin, procMax, 0.1f, context->style.values[GUI_VALUE_SLIDER_WIDTH], orientation, flags, absoluteBounds);
}

bool TextBox(char* text, const uint32_t max_length, int& carrot, uint32_t flags, const ivec4& bounds) {
	const int32_t CARROT_WIDTH = 1;
	const int32_t FONT_HEIGHT = 16;

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
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_END]) {
					carrot = length;
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_LEFT]) {
					carrot = std::max(carrot - 1, 0);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_RIGHT]) {
					carrot = std::min(carrot + 1, length);
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_BACK]) {
					if (carrot > 0) {
						for (int32_t index = --carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						ans = true;
					}
				} else if (context->keyEvents[index].value == context->keyMap[GUI_KEY_DELETE]) {
					if (carrot < length) {
						for (int32_t index = carrot; index < length - 1; ++index) {
							text[index] = text[index + 1];
						}
						text[--length] = '\0';
						carrot = std::min(carrot, length);
						ans = true;
					}
				}
				break;
			case GUI_EVENT_CHAR :
				if (length < (int)max_length - 1) {
					length += 1;
					for (int32_t index = length; index >= carrot; --index) {
						text[index] = text[index - 1];
					}
					text[carrot++] = (char)context->keyEvents[index].value;
					ans = true;
				} else {
					printf("\a");
				}
				break;
			default :
				assert(!"?!");
			}
		}
		context->keyEventCount = 0;
		if ((flags & GUI_FOREGROUND) && (time(NULL) % 2 == 0)) {
			const int32_t carrotPosition = context->getTextSize(context, text + offset, std::max(carrot - offset, 0)).x;
			const int32_t carrotHeight = std::min(height, FONT_HEIGHT);
			GUIDrawQuad({textBounds.x + carrotPosition, textBounds.y, textBounds.x + carrotPosition + CARROT_WIDTH, textBounds.y + carrotHeight}, context->style.colors[GUI_COLOR_TEXT]);
		}
	}
	if ((flags & GUI_FOREGROUND) && (height >= FONT_HEIGHT)) {
		if (flags & GUI_HIDDEN) {
			// TODO: DrawIcon(GUI_ICON_CLOSE) ?
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

	bool ans = false;

	Panel(AbsoluteLayout()){
		const ivec4 bounds = LayoutGetBounds(nbounds, true);
		const int32_t buttonWidth = std::min(bounds.z - bounds.x, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);
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
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "%d", value);

	return SpinnerInternal(value, tmp, flags, bounds);
}

bool Spinner(int& value, const char** labels, uint32_t count, uint32_t flags, const ivec4& bounds) {
	if (SpinnerInternal(value, labels[std::clamp<int>(value, 0, count - 1)], flags, bounds)) {
		value = std::clamp<int>(value, 0, count - 1);
		return true;
	}
	return false;
}

bool Scrollbar(float& value, float barProc, GUIOrientation orientation, float step, const ivec4& bounds) {
	bool ans = false;
	
	float buttonProc;
	uint8_t buttonIcons[2];
	int barLength;
	int length;
	const ivec4 rbounds = LayoutGetBounds(bounds, false);
	
	switch (orientation) {
	case GUI_VERTICAL :
		length = rbounds.w - rbounds.y;
		if (length < 32) {
			return false;
		}
		barLength = length - 32;
		buttonProc = 16.0f / (float)length;
		buttonIcons[0] = GUI_ICON_ARROW_UP;
		buttonIcons[1] = GUI_ICON_ARROW_DOWN;
		break;
	case GUI_HORIZONTAL :
		length = rbounds.z - rbounds.x;
		if (length < 32) {
			return false;
		}
		barLength = length - 32;
		buttonProc = 16.0f / (float)length;
		buttonIcons[0] = GUI_ICON_ARROW_LEFT;
		buttonIcons[1] = GUI_ICON_ARROW_RIGHT;
		break;
	default :
		assert(false);
	}

	Panel(BorderLayout(orientation, buttonProc, buttonProc, 0), 0, GUI_FLAGS_PANEL, bounds) {
		if (Button(buttonIcons[0])) {
			value = std::max(value - step, 0.0f);
			ans = true;
		}
		GUIDrawBorder(LayoutGetAbsoluteBounds({}, false), context->style.colors[GUI_COLOR_BORDER]);
		if (Slider(value, barLength * barProc, orientation, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE)) {
			ans = true;
		}
		if (Button(buttonIcons[1])) {
			value = std::min(value + step, 1.0f);
			ans = true;
		}
	}
	
	return ans;
}
#if 0
bool Palette(color& value, uint32_t flags, const ivec4& bounds) {
	static const uint32_t COLOR_COUNT_X = 16;
	static const uint32_t COLOR_COUNT_Y = 32;
	static const vec4 PRIME_COLORS[] = {
		{1.0f, 0.0f, 0.0f, 1.0f}, // RED
		{1.0f, 1.0f, 0.0f, 1.0f}, // YELLOW
		{0.0f, 1.0f, 0.0f, 1.0f}, // GREEN
		{0.0f, 1.0f, 1.0f, 1.0f}, // CYAN
		{0.0f, 0.0f, 1.0f, 1.0f}, // BLUE
		{1.0f, 0.0f, 1.0f, 1.0f}, // VIOLETE
		{1.0f, 0.0f, 0.0f, 1.0f}, // RED
	};
	static const uint32_t PrimeColorCount = (sizeof(PRIME_COLORS) / sizeof(PRIME_COLORS[0])) - 1;

	const ivec4 absoluteBounds = LayoutGetAbsoluteBounds(bounds, true);

	const int width = absoluteBounds.z - absoluteBounds.x;
	const int height = absoluteBounds.w - absoluteBounds.y;

	const int colorTileSizeX = (width  / COLOR_COUNT_X / PrimeColorCount) * 1;
	const int colorTileSizeY = (height / COLOR_COUNT_Y / PrimeColorCount) * 3;
	const int offsetX = (width  - colorTileSizeX * COLOR_COUNT_X * PrimeColorCount) / 2;
	const int offsetY = (height - colorTileSizeY * COLOR_COUNT_Y * 2) / 2;

	if (flags & GUI_VISIBLE) {
		for (uint32_t y = 0; y < COLOR_COUNT_Y * 2; ++y) {
			const int colorIndexY = y / COLOR_COUNT_Y;
			const float procY = (float)(y % COLOR_COUNT_Y) / (float)(COLOR_COUNT_Y);
			const float iprocY = 1.0f - procY;
			for (uint32_t x = 0; x < COLOR_COUNT_X * PrimeColorCount; ++x) {
				const int colorIndexX = x / COLOR_COUNT_X;
				float procX = (float)(x % COLOR_COUNT_X)/ (float)(COLOR_COUNT_X);
				const float iprocX = 1.0f - procX;
				const vec4 tmpca[] = {
					{1.0f, 1.0f, 1.0f, 1.0f},
					{
						PRIME_COLORS[colorIndexX].x * iprocX + PRIME_COLORS[colorIndexX + 1].x * procX,
						PRIME_COLORS[colorIndexX].y * iprocX + PRIME_COLORS[colorIndexX + 1].y * procX,
						PRIME_COLORS[colorIndexX].z * iprocX + PRIME_COLORS[colorIndexX + 1].z * procX,
						PRIME_COLORS[colorIndexX].w * iprocX + PRIME_COLORS[colorIndexX + 1].w * procX,
					},
					{0.0f, 0.0f, 0.0f, 1.0f}
				};
				vec4 k = {
					tmpca[colorIndexY].x * iprocY + tmpca[colorIndexY + 1].x * procY,
					tmpca[colorIndexY].y * iprocY + tmpca[colorIndexY + 1].y * procY,
					tmpca[colorIndexY].z * iprocY + tmpca[colorIndexY + 1].z * procY,
					tmpca[colorIndexY].w * iprocY + tmpca[colorIndexY + 1].w * procY,
				};
				GUIDrawQuad({
								absoluteBounds.x + offsetX + x * colorTileSizeX, 
				                absoluteBounds.y + offsetY + y * colorTileSizeY, 
				                absoluteBounds.x + offsetX + (x + 1) * colorTileSizeX, 
				                absoluteBounds.y + offsetY + (y + 1) * colorTileSizeY
							},
				            {k.x * 255.0f, k.y * 255.0f, k.z * 255.0f, k.w * 255.0f});
			}
		}
	}
/**/
	const ivec4 paletteRect = {absoluteBounds.x + offsetX, absoluteBounds.y + offsetY, absoluteBounds.x + offsetX + COLOR_COUNT_X * PrimeColorCount * colorTileSizeX, absoluteBounds.y + offsetY + COLOR_COUNT_Y * 2 * colorTileSizeY};
	const bool isInside = RECT_CONTAINS_POINT(paletteRect, GetMousePosition());
	const ivec2 relativeMousePosition = {
			GetMousePosition().x - paletteRect.x,
			GetMousePosition().y - paletteRect.y
		};

	if ((flags & GUI_ENABLED) && isInside && GetMouseLeftButton() == false && GetLastMouseLeftButton() == true) {
		const int colorIndexX = relativeMousePosition.x / (COLOR_COUNT_X * colorTileSizeX);
		const int colorIndexY = relativeMousePosition.y / (COLOR_COUNT_Y * colorTileSizeY);
		const int x = (relativeMousePosition.x / colorTileSizeX) % COLOR_COUNT_X;
		const int y = (relativeMousePosition.y / colorTileSizeY) % COLOR_COUNT_Y;
		const float procX = (float)(x) / (float)(COLOR_COUNT_X - 1);
		const float procY = (float)(y) / (float)(COLOR_COUNT_Y - 1);
		const vec4 tmpca[] = {
			{1.0f, 1.0f, 1.0f, 1.0f},
			{
				PRIME_COLORS[colorIndexX].x * (1.0f - procX) + PRIME_COLORS[colorIndexX + 1].x * procX,
				PRIME_COLORS[colorIndexX].y * (1.0f - procX) + PRIME_COLORS[colorIndexX + 1].y * procX,
				PRIME_COLORS[colorIndexX].z * (1.0f - procX) + PRIME_COLORS[colorIndexX + 1].z * procX,
				PRIME_COLORS[colorIndexX].w * (1.0f - procX) + PRIME_COLORS[colorIndexX + 1].w * procX,
			},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
		const vec4 outputColor = {
			tmpca[colorIndexY].x * (1.0f - procY) + tmpca[colorIndexY + 1].x * procY,
			tmpca[colorIndexY].y * (1.0f - procY) + tmpca[colorIndexY + 1].y * procY,
			tmpca[colorIndexY].z * (1.0f - procY) + tmpca[colorIndexY + 1].z * procY,
			tmpca[colorIndexY].w * (1.0f - procY) + tmpca[colorIndexY + 1].w * procY,
		};
		value = {
			outputColor.x * 255, 
			outputColor.y * 255, 
			outputColor.z * 255, 
			outputColor.w * 255};
		return true;
	}

	return false;
}
#endif
Layout BeginPanel(const Layout& layout, uint32_t padding, uint32_t flags, const ivec4& bounds) {
	context->layout.backup_viewport = context->viewport;
	context->layout.backup_clip = context->clip;

	context->viewport = LayoutGetAbsoluteBounds(bounds, true);
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

Layout BeginSplitPanel(GUIOrientation orientation, float& weight, uint32_t padding, uint32_t flags, const ivec4& bounds) {
	static const int32_t SEPARATOR = 4;
	const Layout ans = BeginPanel(SplitLayout(orientation, weight, SEPARATOR), padding, flags, bounds);
	const ivec4 absoluteBounds = context->viewport; // DAFUQ ?!
	const int32_t width  = absoluteBounds.z - absoluteBounds.x;
	const int32_t height = absoluteBounds.w - absoluteBounds.y;
	const int32_t halfPadding = SEPARATOR / 2;
	ivec4 separatorBounds;

	switch (orientation) {
	case GUI_VERTICAL :
		{
		const int32_t k = (float)(height - SEPARATOR) * weight;
		separatorBounds = {absoluteBounds.x, absoluteBounds.y + k, absoluteBounds.z, absoluteBounds.y + k + SEPARATOR};
		}
		break;
	case GUI_HORIZONTAL :
		{
		const int32_t k = (float)(width - SEPARATOR) * weight;
		separatorBounds = {absoluteBounds.x + k, absoluteBounds.y, absoluteBounds.x + k + SEPARATOR, absoluteBounds.w};
		}
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

Layout BeginScrollPanel(int width, int height, int* offsetX, int* offsetY, uint32_t margin, uint32_t flags, const ivec4& bounds) {
	Layout ans = BeginPanel(AbsoluteLayout());
	const ivec4 absoluteRectangle = LayoutGetAbsoluteBounds(bounds, false);
	const int visible_width = absoluteRectangle.z - absoluteRectangle.x;
	const int visible_height = absoluteRectangle.w - absoluteRectangle.y;
	const int WIDTH = context->style.values[GUI_VALUE_SCROLLBAR_WIDTH];
	const bool vBarVisible = ((visible_height < height) && (visible_height > WIDTH) && (offsetY != nullptr));
	const bool hBarVisible = ((visible_width  <  width) && (visible_width > WIDTH) && (offsetX != nullptr));
	
	if ((visible_width <= 0) || (visible_height <= 0)) {
		ans.run_statement = false;
		return ans;
	}

	if (hBarVisible == 0) {
		width = visible_width - vBarVisible * WIDTH;
	}
	if (vBarVisible == 0) {
		height = visible_height - hBarVisible * WIDTH;
	}

	if (vBarVisible) {
		if (visible_width < WIDTH) {
			ans.run_statement = false;
			return ans;
		}
		const float k = visible_height - (hBarVisible ? WIDTH : 0);
		const float diff = height - k;
		const float barProc = k / (float)height;
		float fv = 1.0f - (float)*offsetY / diff;
		if (Scrollbar(fv, barProc, GUI_VERTICAL, 0.1f, {visible_width - WIDTH, 0, visible_width, visible_height  - (hBarVisible? WIDTH : 0)})) {
			*offsetY = (1.0f - fv) * diff;
		}
		context->clip.z -= WIDTH;;
	} else {
		if (offsetY) {
			offsetY = 0;
		}
	}
	if (hBarVisible) {
		const float k = visible_width - (vBarVisible ? WIDTH : 0);
		const float diff = width - k;
		const float barProc = k / (float)width;
		float fv = (float)*offsetX / diff;
		if (Scrollbar(fv, barProc, GUI_HORIZONTAL, 0.1f, {0, visible_height - WIDTH, visible_width - (vBarVisible ? WIDTH : 0), visible_height})) {
			*offsetX = fv * diff;
		}
		context->clip.w -= WIDTH;
	} else {
		if (offsetX) {
			*offsetX = 0;
		}
	}

	context->viewport.x = absoluteRectangle.x;
	context->viewport.z = absoluteRectangle.x+ width;
	context->viewport.y = absoluteRectangle.y;
	context->viewport.w = absoluteRectangle.y + height;
	
	if (offsetX) {
		context->viewport.x -= *offsetX;
		context->viewport.z -= *offsetX;
	}
	
	if (offsetY) {
		context->viewport.y -= *offsetY;
		context->viewport.w -= *offsetY;
	}

	return ans;
}

Layout BeginTabPanel(const char* names, int& selected, uint32_t margin, uint32_t flags, const ivec4& bounds) {
	uint32_t count = (*names != '\0');
	for(const char* ptr = names, *begin = ptr; *ptr !='\0'; ++ptr) {
		if (*ptr == ',') {
			if (ptr - begin) {
				count += 1;
			}
			begin = ptr + 1;
		}
	}
	Layout ans = BeginPanel(AbsoluteLayout());
	if (count == 0) {
		ans.run_statement = false;
	}
	if (ans.run_statement == false) {
		return ans;
	}

	const ivec4 absoluteRectangle = LayoutGetAbsoluteBounds(bounds, true);
	const int32_t tabWidth = (absoluteRectangle.z - absoluteRectangle.x) / count;
	const int32_t tabHeight = std::min(absoluteRectangle.w - absoluteRectangle.y, (int)context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);

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
				if (Button(tmp, GUI_FLAGS_BUTTON | ((index == selected) ? GUI_CLICKED | GUI_FOCUSED : GUI_NONE), {index * tabWidth, 0, (index + 1) * tabWidth - 1, tabHeight})) {
					selected = index;
				}
			}
			++index;
			begin = ptr + 1;
			break;
		}
	} while (*ptr++ != '\0');

	context->viewport.x = absoluteRectangle.x;
	context->viewport.y = absoluteRectangle.y + tabHeight + 1;
	context->viewport.z = absoluteRectangle.z;
	context->viewport.w = absoluteRectangle.w;

	GUIDrawBorder(context->viewport, context->style.colors[GUI_COLOR_BORDER]);
	GUIDrawQuad(context->viewport, context->style.colors[GUI_COLOR_PANEL]);

	return ans;
}

Layout BeginWindow(ivec4* rbounds, const char* text, uint32_t margin, uint32_t* flags) {
	assert(context != NULL);

	static const int32_t CloseButtonSize = context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
	Layout ans = BeginPanel();
	ivec4 bounds = LayoutGetAbsoluteBounds(*rbounds, true);

	uint32_t defaultFlags = GUI_FLAGS_WINDOW  & ~GUI_WINDOW_CLOSE;

	if (flags == NULL) {
		flags = & defaultFlags;
	} else if ((*flags & GUI_VISIBLE) == 0) {
		ans.run_statement = false;
		EndPanel(&ans);
		return ans;
	}

	GUIDrawQuad(bounds, context->style.colors[GUI_COLOR_PANEL]);
	context->viewport = bounds;
	context->viewport.x += 1;
	context->viewport.y += 1;
	context->viewport.z -= 1;
	context->viewport.w -= 1;

	if (*flags & GUI_WINDOW_TITLEBAR) {
		bounds.w = std::max(bounds.w, bounds.y + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] + 15);

		ivec4 titlebarRectangle = {bounds.x, bounds.y, bounds.z - CloseButtonSize, bounds.y + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]};
		if (*flags & GUI_WINDOW_MOVE) {
			const int width = bounds.z - bounds.x;
			const int height = bounds.w - bounds.y;
			if (Movable(titlebarRectangle)) {
				bounds = {titlebarRectangle.x, titlebarRectangle.y, titlebarRectangle.x + width, titlebarRectangle.y + height};
			}
		}
		GUIDrawQuad({bounds.x, bounds.y, bounds.z, bounds.y + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]}, context->style.colors[GUI_COLOR_TITLEBAR]);
		if (text) {
			GUIDrawText(text, {bounds.x + 2, bounds.y + 2, bounds.z - CloseButtonSize, bounds.y + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]}, context->style.colors[GUI_COLOR_TEXT], 0);
		}
		const int32_t width = bounds.z - bounds.x;
		if ((*flags & GUI_WINDOW_CLOSE)) {
			bounds.z = std::max(bounds.z, bounds.x + context->style.values[GUI_VALUE_TITLEBAR_HEIGHT]);
			if (Button(GUI_ICON_CLOSE, GUI_FLAGS_BUTTON, {width - context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] + 1, 1, width - 1, context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] - 1})) {
				*flags &= ~GUI_VISIBLE;
			}
		}
		context->viewport.y += context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
	}

	if (*flags & GUI_WINDOW_SIZE) {
		bounds.z = std::max(bounds.z, bounds.x);
		context->viewport.w -= 15;
		
		ivec4 iconBounds = {bounds.z - 15, bounds.w - 15, bounds.z, bounds.w};
		bool focused = RECT_CONTAINS_POINT(iconBounds, GetMousePosition());
		bool clicked = focused && GetMouseLeftButton();

		if (Movable(iconBounds)) {
			bounds.z = iconBounds.z;
			bounds.w = iconBounds.w;
		}
		
		GUIDrawIcon(GUI_ICON_SIZE, iconBounds, context->style.colors[clicked ? GUI_COLOR_ACTIVE : focused ? GUI_COLOR_FOCUSED : GUI_COLOR_PANE]);
	}

	GUIDrawBorder(bounds, context->style.colors[GUI_COLOR_BORDER]);
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
	}
}

Layout* GetLayout() {
	return &context->layout;
}

#undef MIN
#undef MIN2
#undef MIN3
#undef RECT_CONTAINS_POINT
