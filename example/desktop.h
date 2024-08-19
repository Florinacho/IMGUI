#include "widgets.h"
#include "layouts.h"
#include "containers.h"
#include "demo1.h"
#include "demo2.h"
#include "demo3.h"
#include "demo4.h"
#include "calculator.h"
#include "notepad.h"
#include "mediaplayer.h"
#include "theme_editor.h"
#include "minesweeper.h"

enum WindowID {
	WID_TEST,
	WID_DEMO,
	WID_CALC,
	WID_NOTE,
	WID_PLYR,
	WID_THEM,
	WID_MINE,

	WID_COUNT
};

static ivec4 testsWindowBounds = {0, 0, 800, 600};
static uint32_t testsWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static ivec4 demosWindowBounds = {50, 50, 850, 650};
static uint32_t demosWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static ivec4 calculatorWindowBounds = {100, 100, 300, 400};
static uint32_t calculatorWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static ivec4 minesweeperWindowBounds = {100, 100, 400, 500};
static uint32_t minesweeperWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static bool startMenuVisible = false;

void testsWindow(ivec4* bounds, uint32_t* flags) {
	static int tab = 0;
	Window(bounds, "Tests", "Individual element demo", 0, flags) {
		TabPanel("Widgets,Layouts,Panels", tab) {
			switch (tab) {
			case 0 : TestWidgets();  break;
			case 1 : TestLayouts();  break;
			case 2 : TestPanels();   break;
			}
		}
	}
}

void demosWindow(ivec4* bounds, uint32_t* flags) {
	static int tab = 0;
	Window(&demosWindowBounds, "Demos", "", 0, &demosWindowFlags) {
		TabPanel("Webpage,X&O,Dialogue,Chat", tab) {
			switch (tab) {
			case 0 : demo1Panel(); break;
			case 1 : demo2Panel(); break;
			case 2 : demo3Panel(); break;
			case 3 : demo4Panel(); break;
			}
		}
	}
}

bool taskbarWindow() {
	static ivec4 bounds = {0, 690, 1024, 720};
	static uint32_t flags = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;
	static uint32_t rawTime = 0;

	Window(&bounds, "", nullptr, 0, &flags) {
		SetLayout(BorderLayout(GUI_HORIZONTAL, 0.15f, 0.09f));
		if (Button("Apps")) {
			startMenuVisible = !startMenuVisible;
		}
		Panel(GridLayout(10, 1)) {
			if (testsWindowFlags & GUI_VISIBLE) {
				if (Button("Tests")) {
					WMBringIDToFront(WID_TEST);
				}
			}
			if (demosWindowFlags & GUI_VISIBLE) {
				if (Button("Demos")) {
					WMBringIDToFront(WID_DEMO);
				}
			}
			if (calculatorWindowFlags & GUI_VISIBLE) {
				if (Button("Calculator")) {
					WMBringIDToFront(WID_CALC);
				}
			}
			if (notepadFlags & GUI_VISIBLE) {
				if (Button("Notepad")) {
					WMBringIDToFront(WID_NOTE);
				}
			}
			if (mediaPlayerFlags & GUI_VISIBLE) {
				if (Button("Video Player")) {
					WMBringIDToFront(WID_PLYR);
				}
			}
			if (themeEditorFlags & GUI_VISIBLE) {
				if (Button("Theme")) {
					WMBringIDToFront(WID_THEM);
				}
			}
			if (minesweeperWindowFlags & GUI_VISIBLE) {
				if (Button("Minesweeper")) {
					WMBringIDToFront(WID_MINE);
				}
			}
		}
		{
		rawTime += 1;
		char timeText[32];
		uint32_t time = rawTime / 1000;
		uint32_t seconds = time % 60;
		uint32_t minutes = (time / 60) % 60;
		uint32_t hours = (time / 3600) % 24;
		snprintf(timeText, sizeof(timeText), "%.2d:%.2d:%.2d", hours, minutes, seconds);
		Label(timeText);
		}
	}

	return startMenuVisible;
}

void startMenuWindow() {
	static ivec4 bounds = {0, 400, 200, 690};
	static uint32_t flags = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE;

	Window(&bounds, "", "", 0, &flags) {
		Panel(FixSplitLayout(GUI_VERTICAL, -30, 0, 0)) {
			static int offset = 0;
			ScrollPanel(0, 700, NULL, &offset, 0) {
				Panel(GridLayout(1, 16, 1)) {
					if (Button("Tests")) {
						testsWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(WID_TEST);
						startMenuVisible = false;
					}
					if (Button("Demos")) {
						demosWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(WID_DEMO);
						startMenuVisible = false;
					}
					if (Button("Calculator")) {
						calculatorWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(WID_CALC);
						startMenuVisible = false;
					}
					if (Button("Notepad")) {
						notepadFlags |= GUI_VISIBLE;
						startMenuVisible = false;
						WMBringIDToFront(WID_NOTE);
					}
					if (Button("Video Player")) {
						mediaPlayerFlags |= GUI_VISIBLE;
						startMenuVisible = false;
						WMBringIDToFront(WID_PLYR);
					}
					if (Button("Theme")) {
						themeEditorFlags |= GUI_VISIBLE;
						startMenuVisible = false;
						WMBringIDToFront(WID_THEM);
					}
					if (Button("Minesweeper")) {
						minesweeperWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(WID_MINE);
						startMenuVisible = false;
					}
				}
			}
			Panel(GridLayout(3, 1)) {
				Button("Logoff");
				Button("Restart");
				Button("Logout");
			}
		}
	}
}

void desktopDemoInit() {
	testsWindowFlags &= ~GUI_VISIBLE;
	demosWindowFlags &= ~GUI_VISIBLE;
	calculatorWindowFlags &= ~GUI_VISIBLE;
	notepadFlags &= ~GUI_VISIBLE;
	mediaPlayerFlags &= ~GUI_VISIBLE;
	themeEditorFlags &= ~GUI_VISIBLE;
	minesweeperWindowFlags &= ~GUI_VISIBLE;

	WMRegister(&testsWindowBounds, &testsWindowFlags, WID_TEST);
	WMRegister(&demosWindowBounds, &demosWindowFlags, WID_DEMO);
	WMRegister(&calculatorWindowBounds, &calculatorWindowFlags, WID_CALC);
	WMRegister(&notepadBounds, &notepadFlags, WID_NOTE);
	WMRegister(&mediaPlayerBounds, &mediaPlayerFlags, WID_PLYR);
	WMRegister(&themeEditorBounds, &themeEditorFlags, WID_THEM);
	WMRegister(&minesweeperWindowBounds, &minesweeperWindowFlags, WID_MINE);
}

void desktopDemo() {
	GUIContext* gui = GUIGetActiveContext();

	for (uint32_t windowIndex = 0; windowIndex < gui->count; ++windowIndex) {
		gui->events_enabled = (startMenuVisible ? false : gui->windows[windowIndex].receiveEvents);
		switch (gui->windows[windowIndex].id) {
		case WID_TEST : testsWindow(&testsWindowBounds, &testsWindowFlags); break;
		case WID_DEMO : demosWindow(&demosWindowBounds, &demosWindowFlags); break;
		case WID_CALC : calculatorWindow(&calculatorWindowBounds, &calculatorWindowFlags); break;
		case WID_NOTE : notepadWindow(); break;
		case WID_PLYR : mediaPlayerWindow(); break;
		case WID_THEM : themeEditorWindow() ; break;
		case WID_MINE : minesweeperWindow(&minesweeperWindowBounds, &minesweeperWindowFlags); break;
		}
	}
	gui->events_enabled = true;
	if (taskbarWindow()) {
		static float proc = 0.5f;
		SplitPanel(GUI_HORIZONTAL, proc) {
			startMenuWindow();
			DummyElement();
		}
	}
}
