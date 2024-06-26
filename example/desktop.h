#include "widgets.h"
#include "layouts.h"
#include "containers.h"
#include "demo1.h"
#include "demo2.h"
#include "demo3.h"
#include "demo4.h"
#include "calculator.h"
#include "file_selector.h"
#include "text_editor.h"
#include "mediaplayer.h"
#include "theme_editor.h"

FileSelectorContext fs = {
	.bounds = {0, 0, 300, 300},
	.flags = GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_WINDOW_TITLEBAR | GUI_WINDOW_MOVE | GUI_WINDOW_SIZE,
};

static ivec4 testsWindowBounds = {0, 0, 800, 600};
static uint32_t testsWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static ivec4 demosWindowBounds = {50, 50, 850, 650};
static uint32_t demosWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static ivec4 calculatorWindowBounds = {100, 100, 300, 400};
static uint32_t calculatorWindowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

static bool startMenuVisible = false;

void testsWindow(ivec4* bounds, uint32_t* flags) {
	static int tab = 0;
	Window(bounds, "Tests", 0, flags) {
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
	Window(&demosWindowBounds, "Demos", 0, &demosWindowFlags) {
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

	Window(&bounds, "", 0, &flags) {
		SetLayout(BorderLayout(GUI_HORIZONTAL, 0.15f, 0.09f));
		if (Button("Apps")) {
			startMenuVisible = !startMenuVisible;
		}
		Panel(GridLayout(10, 1)) {
			if (testsWindowFlags & GUI_VISIBLE) {
				if (Button("Tests")) {
					WMBringIDToFront(0);
				}
			}
			if (demosWindowFlags & GUI_VISIBLE) {
				if (Button("Demos")) {
					WMBringIDToFront(1);
				}
			}
			if (calculatorWindowFlags & GUI_VISIBLE) {
				if (Button("Calculator")) {
					WMBringIDToFront(2);
				}
			}
			if (fs.flags & GUI_VISIBLE) {
				if (Button("File Selector")) {
					WMBringIDToFront(3);
				}
			}
			if (notepadFlags & GUI_VISIBLE) {
				if (Button("Notepad")) {
					WMBringIDToFront(4);
				}
			}
			if (mediaPlayerFlags & GUI_VISIBLE) {
				if (Button("Video Player")) {
					WMBringIDToFront(5);
				}
			}
			if (themeEditorFlags & GUI_VISIBLE) {
				if (Button("Theme")) {
					WMBringIDToFront(6);
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

	Window(&bounds, "", 0, &flags) {
		Panel(FixSplitLayout(GUI_VERTICAL, -30, 0, 0)) {
			static int offset = 0;
			ScrollPanel(0, 700, NULL, &offset, 0) {
				Panel(GridLayout(1, 16, 1)) {
					if (Button("Tests")) {
						testsWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(0);
						startMenuVisible = false;
					}
					if (Button("Demos")) {
						demosWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(1);
						startMenuVisible = false;
					}
					if (Button("Calculator")) {
						calculatorWindowFlags |= GUI_VISIBLE;
						WMBringIDToFront(2);
						startMenuVisible = false;
					}
					if (Button("File selector")) {
						startMenuVisible = false;
						ReadDir(fs, ".", GUI_FS_ALL);
						WMBringIDToFront(3);
					}
					if (Button("Notepad")) {
						notepadFlags |= GUI_VISIBLE;
						startMenuVisible = false;
						WMBringIDToFront(4);
					}
					if (Button("Video Player")) {
						mediaPlayerFlags |= GUI_VISIBLE;
						startMenuVisible = false;
						WMBringIDToFront(5);
					}
					if (Button("Theme")) {
						themeEditorFlags |= GUI_VISIBLE;
						startMenuVisible = false;
						WMBringIDToFront(6);
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

	WMRegister(&testsWindowBounds, &testsWindowFlags, 0);
	WMRegister(&demosWindowBounds, &demosWindowFlags, 1);
	WMRegister(&calculatorWindowBounds, &calculatorWindowFlags, 2);
	WMRegister(&fs.bounds, &fs.flags, 3);
	WMRegister(&notepadBounds, &notepadFlags, 4);
	WMRegister(&mediaPlayerBounds, &mediaPlayerFlags, 5);
	WMRegister(&themeEditorBounds, &themeEditorFlags, 6);
}

void desktopDemo() {
	GUIContext* gui = GUIGetActiveContext();

	for (uint32_t windowIndex = 0; windowIndex < gui->count; ++windowIndex) {
		gui->events_enabled = (startMenuVisible ? false : gui->windows[windowIndex].receiveEvents);
		switch (gui->windows[windowIndex].id) {
		case 0 : testsWindow(&testsWindowBounds, &testsWindowFlags); break;
		case 1 : demosWindow(&demosWindowBounds, &demosWindowFlags); break;
		case 2 : calculatorWindow(&calculatorWindowBounds, &calculatorWindowFlags); break;
		case 3 : FileSelectorWindow(fs); break;
		case 4 : notepadWindow(); break;
		case 5 : mediaPlayerWindow(); break;
		case 6 : themeEditorWindow() ; break;
		}
	}
	gui->events_enabled = true;
	if (taskbarWindow()) {
		startMenuWindow();
	}
}
