#ifndef __DEMO_DESKTOP_H__
#define __DEMO_DESKTOP_H__

#include "tests.h"
#include "demos.h"

enum WindowID {
	WID_PAN1,
	WID_PAN2,
	WID_TEST,
	WID_BRWS,
	WID_XO,
	WID_CHAT,
	WID_CALC,
	WID_NOTE,
	WID_PLYR,
	WID_THEM,
	WID_MINE,

	WID_COUNT
};

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

void startPanel();

struct WindowInfo2 {
	ivec4 bounds;
	const char* title;
	const char* footer;
	void(*proc)();
	uint32_t flags;
};

static WindowInfo2 windows[] = {
	{
		{0, 0, 50, 50},
		"", "",
		startPanel,
		GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE,
	},
	{
		{0, 0, 150, 720},
		"", "",
		startPanel,
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
		{50, 50, 850, 650},
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
};

void startPanel() {
	static bool panelVisible = true;

	if (panelVisible) {
			Panel(GridLayout(1, 600 / 25)) {
				for (uint32_t windowID = WID_TEST; windowID < WID_COUNT; ++windowID) {
					Panel(SplitLayout(GUI_HORIZONTAL, 0.8f)) {
						Label(windows[windowID].title, GUI_VISIBLE | GUI_FOREGROUND | GUI_ALIGN_LEFT);
						{
						bool value = (windows[windowID].flags & GUI_VISIBLE);
						Toggle(value);
						if (value) {
							windows[windowID].flags |= GUI_VISIBLE;
						} else {
							windows[windowID].flags &= ~GUI_VISIBLE;
						}
						}
					}
				}
				if (Button("<<")) {
					panelVisible = false;
					windows[WID_PAN1].flags |=  GUI_VISIBLE;
					windows[WID_PAN2].flags &= ~GUI_VISIBLE;
				}
			}
	} else {
		if (Button(">>")) {
			panelVisible = true;
			windows[WID_PAN1].flags &= ~GUI_VISIBLE;
			windows[WID_PAN2].flags |=  GUI_VISIBLE;
			WMBringIDToFront(WID_PAN2);
			guiGetActiveContext()->modal = WID_PAN2;
		}
	}
}

void desktopDemoInit() {
	for (uint32_t windowID = 0; windowID < WID_COUNT; ++windowID) {
		WMRegister(&windows[windowID].bounds, &windows[windowID].flags, windowID);
	}
	WMBringIDToFront(WID_PAN2);
	guiGetActiveContext()->modal = WID_PAN2;
}

void desktopDemo() {
	GUIContext* gui = guiGetActiveContext();

	for (uint32_t windowIndex = 0; windowIndex < gui->count; ++windowIndex) {
		gui->events_enabled = gui->windows[windowIndex].receiveEvents;
		uint32_t windowID = gui->windows[windowIndex].id;
		uint32_t padding = 0;
		Window(&windows[windowID].bounds, windows[windowID].title, windows[windowID].footer, padding, &windows[windowID].flags) {
			windows[windowID].proc();
		}
	}
	gui->events_enabled = true;
}

#endif // __DEMO_DESKTOP_H__
