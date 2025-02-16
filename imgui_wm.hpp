/*
  Immediate Mode Graphical User Interface - Window Manager extension
*/

#ifndef __IMGUI_WINDOW_MANAGER_HPP__
#define __IMGUI_WINDOW_MANAGER_HPP__

#include "imgui.h"

namespace imguiwm {

static constexpr uint32_t WM_WINDOW_COUNT  = 16

// Window Manager Flags
static constexpr uint32_t WMF_NONE = 0 
static constexpr uint32_t WMF_MODAL_LOCK =  1

// TODO: Split functionality?
// #define WMF_MODAL_LOCK_POSITION  (1 << 0)
// #define WMF_MODAL_LOCK_VISIBLE   (1 << 1)
// #define WMF_MODAL_LOCK_EVENTS    (1 << 2)
// #define WMF_MODAL_SOFT           (WMF_NONE)
// #define WMF_MODAL_HARD           (WMF_MODAL_SOFT | WMF_MODAL_LOCK_POSITION | WMF_MODAL_LOCK_VISIBLE | WMF_MODAL_LOCK_EVENTS)

struct Context {
	struct WindowInfo {
		ivec4* bounds;
		uint32_t* flags;
		int32_t id;
		bool receiveEvents;
	} windows[WM_WINDOW_COUNT];

    GUIContext context;

	uint32_t count;
	uint32_t flags;
	int32_t modal;
};

bool WMRegister(ivec4* bounds, uint32_t* flags, int32_t id = -1);
bool WMUnregister(GUIContext* context, int32_t id);

void WMBringIndexToFront(int32_t selected);
int32_t WMGetIndexByID(int32_t id);
void WMBringIDToFront(int32_t id);

void WMOnCursorEvent(int32_t x, int32_t y);
void WMOnButtonEvent(int32_t button, int32_t value);

// ============================================================================
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

} // namespace imguiwm

#endif // __IMGUI_WINDOW_MANAGER_HPP__
