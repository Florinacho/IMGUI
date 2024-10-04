#ifndef __DEMO_DESKTOP_H__
#define __DEMO_DESKTOP_H__

#include "tests.h"
#include "demos.h"

enum WindowID {
	WID_CTRL,
	WID_TEST,
	WID_BRWS,
	WID_XO,
	WID_CHAT,
	WID_CALC,
	WID_NOTE,
	WID_PLYR,
	WID_THEM,
	WID_MINE,
	WID_ICON,

	WID_COUNT
};

void controlPanel();

void testsPanel() {
	static int tab = 0;
	TabPanel("Widgets,Layouts,Panels", tab) {
		switch (tab) {
		case 0 : TestWidgets();  break;
		case 1 : TestLayouts();  break;
		case 2 : TestPanels();   break;
		}
	}
}

struct WindowInfo2 {
	ivec4 bounds;
	const char* title;
	const char* footer;
	void(*proc)();
	uint32_t flags;
};

static WindowInfo2 windows[] = {
	{
		{0, 0, 150, 250},
		"", "",
		controlPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_VISIBLE,
	},	
	{
		{0, 0, 800, 600},
		"Tests", "",
		testsPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{50, 50, 850, 650},
		"Browser", "",
		browserPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{50, 50, 600, 450},
		"Chat", "",
		chatPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{100, 100, 300, 400},
		"Calculator", "",
		calculatorPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{200, 200, 500, 400},
		"Notepad", "",
		notepadPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{100, 100, 800, 600},
		"Mediaplayer", "Enjoy your media",
		mediaPlayerPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{200, 200, 600, 500},
		"Settings", "Customize the look and feel",
		settingsPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{50, 50, 300, 350},
		"TicTacToe", "",
		ticTacToePanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{100, 100, 400, 500},
		"Minesweeper", "",
		minesweeperPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
	{
		{100, 100, 400, 300},
		"Icon viewer", "",
		iconViewerPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_ALIGN_CENTER | GUI_WINDOW_MOVE | GUI_WINDOW_DECORATION | GUI_WINDOW_SIZE,
	},
};

void controlPanel() {
	Panel(GridLayout(1, WID_COUNT + 1)) {
		for (uint32_t windowID = WID_TEST; windowID < WID_COUNT; ++windowID) {
			bool visible = (windows[windowID].flags & GUI_VISIBLE);

			Panel(SplitLayout(GUI_HORIZONTAL, 0.8f)) {
				Label(windows[windowID].title, GUI_VISIBLE | GUI_FOREGROUND | GUI_ALIGN_LEFT);
				Toggle(visible);
				if (visible) {
					windows[windowID].flags |= GUI_VISIBLE;
				} else {
					windows[windowID].flags &= ~GUI_VISIBLE;
				}
			}
		}

		if (Button("Show all")) {
			for (uint32_t windowID = 0; windowID < WID_COUNT; ++windowID) {
				windows[windowID].flags |= GUI_VISIBLE;
				windows[WID_CTRL].flags &= ~GUI_VISIBLE;
			}
		}
		if (Button("Hide all")) {
			for (uint32_t windowID = 0; windowID < WID_COUNT; ++windowID) {
				windows[windowID].flags &= ~GUI_VISIBLE;
			}
		}
	}
}

void desktopDemoInit() {
	for (uint32_t windowID = 0; windowID < WID_COUNT; ++windowID) {
		WMRegister(&windows[windowID].bounds, &windows[windowID].flags, windowID);
	}
	WMBringIDToFront(WID_CTRL);
	guiGetContext()->windowManager.modal = WID_CTRL;
	guiGetContext()->windowManager.flags = WMF_NONE;
}

void desktopDemo() {
	GUIContext* gui = guiGetContext();
	const bool events_enabled_backup = gui->events_enabled;

	for (uint32_t windowIndex = 0; windowIndex < gui->windowManager.count; ++windowIndex) {
		gui->events_enabled = gui->windowManager.windows[windowIndex].receiveEvents;
		uint32_t windowID = gui->windowManager.windows[windowIndex].id;
		uint32_t padding = 0;
		Window(&windows[windowID].bounds, windows[windowID].title, windows[windowID].footer, padding, &windows[windowID].flags) {
			windows[windowID].proc();
		}
	}
	gui->events_enabled = events_enabled_backup;
}

void showControlPanel() {
	static const uint32_t PANEL_WIDTH  = 150;
	static const uint32_t PANEL_HEIGHT = (WID_COUNT + 2) * 20;

	windows[WID_CTRL].bounds.x = GetMousePosition().x;
	windows[WID_CTRL].bounds.y = GetMousePosition().y;
	windows[WID_CTRL].bounds.z = windows[WID_CTRL].bounds.x + PANEL_WIDTH;
	windows[WID_CTRL].bounds.w = windows[WID_CTRL].bounds.y + PANEL_HEIGHT;

	if (windows[WID_CTRL].bounds.z > 1024) {
		windows[WID_CTRL].bounds.z = windows[WID_CTRL].bounds.x;
		windows[WID_CTRL].bounds.x -= PANEL_WIDTH;
	}

	if (windows[WID_CTRL].bounds.w > 720) {
		windows[WID_CTRL].bounds.w = windows[WID_CTRL].bounds.y;
		windows[WID_CTRL].bounds.y -= PANEL_HEIGHT;
	}

	WMBringIDToFront(WID_CTRL);
	guiGetContext()->windowManager.modal = WID_CTRL;
	windows[WID_CTRL].flags |=  GUI_VISIBLE;
}

#endif // __DEMO_DESKTOP_H__
