#include <string>
#include <stdio.h>
#include "WindowCanvas.h"
#include "Renderer.h"

#include <string.h>
#include <filesystem>
#include <algorithm>

#include <dirent.h>
#include <sys/stat.h>
#include <typeinfo>

#ifndef _WIN32
#define VK_ESCAPE  0x09
#define VK_1       0x0A
#define VK_2       0x0B
#define VK_3       0x0C
#define VK_4       0x0D
#define VK_BACK    0x16
#define VK_SPACE   0x41
#define VK_HOME    0x6E
#define VK_END     0x73
#define VK_LEFT    0x71
#define VK_RIGHT   0x72
#define VK_DELETE  0x77
#endif

#define GUI_FS_FILE     (1 << 0)
#define GUI_FS_DIR      (1 << 1)
#define GUI_FS_HIDDEN   (1 << 2)
#define GUI_FS_VISIBLE  (GUI_FS_FILE | GUI_FS_DIR)
#define GUI_FS_ALL      (GUI_FS_VISIBLE | GUI_FS_HIDDEN)

static uint32_t windowFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;

struct FileSelectorContext {
	char cache[PATH_MAX];
	char path[PATH_MAX];
	char filter_name[64];
	uint32_t filter_type;
	uint32_t flags;
	int32_t carrot;
	uint32_t count;
	bool filtering;
};
#define FS_SELECTED_FILE  1
#define FS_SELECTED_DIR   2
#define FS_CLOSED         3
int ReadDir(FileSelectorContext& context, const char* path, uint32_t flags = GUI_FS_VISIBLE);
int FileSelector(FileSelectorContext& fileSelector);

void TestElements() {
	Panel(SplitLayout(GUI_VERTICAL, 0.3f)) {
		Panel(GridLayout(5, 6)) {
			Label("Text");
			Label(GUI_ICON_FILE);
			DummyElement(3);
			
			if (Button("Text")) {
				printf("Pressed text button\n");
			}
			if (Button(GUI_ICON_FILE)) {
				printf("Pressed icon button\n");
			}
			if (Button(GUI_ICON_FILE, "text")) {
				printf("Pressed icon&text button\n");
			}
			DummyElement(2);
			
			{
			static bool value = false;
			if (CheckBox(value)) {
				printf("Checkbox: %s\n", value ? "true" : "false");
			}
			}
			{
			static bool value = false;
			auto bkp = GetLayout()->max;
			GetLayout()->max = {40, 24};
			if (Toggle(value)) {
				printf("Toggle: %s\n", value ? "true" : "false");
			}
			GetLayout()->max = bkp;
			}
			DummyElement(3);
			
			{
			static int value = 0;
			if (Spinner(value)) {
				printf("Spinner: %d\n", value);
			}
			}
			{
			static int value = 0;
			const char* TEXT[] = {
				"Test 0",
				"Test 1",
				"Test 2",
				"Test 3",
			};
			if (Spinner(value, TEXT, 4)) {
				value = std::clamp(value, 0, 3);
				printf("Spinner: %d, %s\n", value, TEXT[value]);
			}
			}
			DummyElement(3);
			
			{
			static char text[128] = "Text";
			static int carrot = -1;
			if (TextBox(text, sizeof(text), carrot)) {
				printf("Textbox: %s\n", text);
			}
			}
			DummyElement(4);
			
			{
			static float progress = 1.0f;
			if (progress > 1.0f) {
				progress -= 1.0f;
			}
			progress += 0.0001f;
			ProgressBar(progress);
			}
			{
			static float value = 0.0f;
			if (Slider(value)) {
				printf("Slider: %f\n", value);
			}
			}
			{
			static float min = 0.0f;
			static float max = 1.0f;
			if (RangeSlider(min, max, GUI_HORIZONTAL)) {
				printf("Slider: %f, %f\n", min, max);
			}
			}
			{
			static float value = 0.5f;
			if (Scrollbar(value, 40.0f / 80.0f, GUI_HORIZONTAL)) {
				printf("Scrollbar: %f\n", value);
			}
			}
			DummyElement(1);
		}

		Panel(GridLayout(30, 3)) {
			DummyElement(1);
			{
			static float value = 0.0f;
			if (Slider(value)) {
				printf("Slider: %f\n", value);
			}
			}
			DummyElement(1);
			{
			static float min = 0.0f;
			static float max = 1.0f;
			if (RangeSlider(min, max, GUI_VERTICAL)) {
				printf("Slider: %f, %f\n", min, max);
			}
			}
			DummyElement(1);
			/**/
			{
			static float value = 0.5f;
			if (Scrollbar(value, 40.0f / 80.0f, GUI_VERTICAL)) {
				printf("Scrollbar: %f\n", value);
			}
			}
			DummyElement(1);
			/**/
		}
	}
}

void TestAbsoluteLayout1() {
	Panel(AbsoluteLayout()) {
		if (Button("Test", GUI_FLAGS_BUTTON, {200, 200, 400, 400})) printf("Pressed Test\n");
	}
}

void TestAbsoluteLayout2() {
	Panel(AbsoluteLayout()) {
		if (Button("Test", GUI_FLAGS_BUTTON, {})) printf("Pressed Test\n");
	}
}

void TestAbsoluteLayout() {
	static int tab = 0;
	TabPanel("Bounds,No Bounds", tab) {
		switch (tab) {
		case 0 : TestAbsoluteLayout1(); break;
		case 1 : TestAbsoluteLayout2(); break;
		}
	}
}

void TestSplitLayout() {
	Panel(SplitLayout(GUI_VERTICAL, 0.5f)) {
		Panel(SplitLayout(GUI_HORIZONTAL, 0.5f)) {
			if (Button("TOP_LEFT")) {
				printf("Pressed TOP_LEFT\n");
			}
			if (Button("TOP_RIGHT")) {
				printf("Pressed TOP_RIGHT\n");
			}
		}
		Panel(SplitLayout(GUI_HORIZONTAL, 0.5f)) {
			if (Button("BOTTOM_LEFT")) {
				printf("Pressed BOTTOM_LEFT\n");
			}
			if (Button("BOTTOM_RIGHT")) {
				printf("Pressed BOTTOM_RIGHT\n");
			}
		}
	}
}

void TestFixSplitLayout() {
	Panel(FixSplitLayout(GUI_VERTICAL, 20)) {
		Button("A");
		Panel(FixSplitLayout(GUI_HORIZONTAL, 200)) {
			Button("B");
			Button("C");
		}
	}
}

void TestBorderLayout() {
	const int ZERO_PADDING = 0;
	const float NORD_PROC = 0.33f;
	const float SOUTH_PROC = 0.33f;
	const float WEST_PROC = 0.33f;
	const float EAST_PROC = 0.33f;
	
	Panel(BorderLayout(GUI_VERTICAL, NORD_PROC, SOUTH_PROC, ZERO_PADDING)) {
		if (Button("NORD")) printf("Pressed NORD\n");
		
		Panel(BorderLayout(GUI_HORIZONTAL, WEST_PROC, EAST_PROC, ZERO_PADDING)) {
			if (Button("WEST")) printf("Pressed WEST\n");
			if (Button("CENTER")) printf("Pressed CENTER\n");
			if (Button("EAST")) printf("Pressed EAST\n");
		}
		
		if (Button("SOUTH")) printf("Pressed SOUTH\n");
	}
}

void TestGridLayout() {
	const int COLLUMN_COUNT = 4;
	const int ROW_COUNT = 4;
	const int CELL_COUNT = COLLUMN_COUNT * ROW_COUNT;
	const int ZERO_PADDING = 0;
	char text[16];
	
	Panel(GridLayout(COLLUMN_COUNT, ROW_COUNT, ZERO_PADDING)) {
		for (uint32_t index = 0; index < CELL_COUNT; ++index) {
			snprintf(text, sizeof(text), "%d", index);
			if (Button(text)) printf("Pressed %s\n", text);
		}
	}
}

void TestLayouts() {
	static int tab = 0;
	TabPanel("Absolute,Split,FixedSplit,Border,Grid", tab) {
		switch (tab) {
		case 0 : TestAbsoluteLayout();  break;
		case 1 : TestSplitLayout();     break;
		case 2 : TestFixSplitLayout();  break;
		case 3 : TestBorderLayout();    break;
		case 4 : TestGridLayout();      break;
		}
	}
}

void TestSplitPanel() {
	static float hproc = 0.5f;
	SplitPanel(GUI_VERTICAL, hproc) {
		static float vtopproc = 0.5f;
		SplitPanel(GUI_HORIZONTAL, vtopproc) {
			if (Button("TOP_LEFT")) printf("Pressed TOP_LEFT\n");
			if (Button("TOP_RIGHT")) printf("Pressed TOP_RIGHT\n");
		}
		
		static float vbotproc = 0.5f;
		SplitPanel(GUI_HORIZONTAL, vbotproc) {
			if (Button("BOTTOM_LEFT")) printf("Pressed BOTTOM_LEFT\n");
			if (Button("BOTTOM_RIGHT")) printf("Pressed BOTTOM_RIGHT\n");
		}
	}
}

void TestTabPanel() {
	static float proc = 0.2f;
	
	static char text[64] = {};
	static int carrot = -1;
	
	static char tabNames[256] = {};
	static int tab = 0;

	SplitPanel(GUI_HORIZONTAL, proc, 5) {
		Panel(GridLayout(1, 20)) {
			Panel(SplitLayout(GUI_HORIZONTAL, 0.3f)) {
				Label("Name:");
				TextBox(text, sizeof(text), carrot);
			}
			if (Button("Add")) {
				if (tabNames[0]) {
					strcat(tabNames, ",");
				}
				strcat(tabNames, text);
				text[0] = 0;
			}
			if (Button("Remove")) {
				printf("Not implemented\n");
			}
		}
		TabPanel(tabNames, tab) {
		}
	}

}

void ButtonGrid(int x, int y, int padding = 0) {
	SetLayout(GridLayout(x, y, padding));
	char text[32];
	for (int index = 0; index < x * y; ++index) {
		snprintf(text, sizeof(text), "Button %d", index);
		if (Button(text)) printf("Pressed %s\n", text);
	}	
}

void TestScrollPanel() {
	const int COLLUMN_COUNT = 2;
	const int ROW_COUNT = 2;
	const int PADDING = 5;
	Panel(GridLayout(COLLUMN_COUNT, ROW_COUNT, PADDING)) {
		// Static with and height
		static int sp1OffsetX = 0, sp1OffsetY = 0;
		ScrollPanel(700, 700, &sp1OffsetX, &sp1OffsetY) {
			ButtonGrid(5, 5);
		}
#if 1
		// Nested static width and height
		static int sp2OffsetX = 0, sp2OffsetY = 0;
		ScrollPanel(500, 500, &sp2OffsetX, &sp2OffsetY) {
			static int sp2bOffsetX = 0, sp2bOffsetY = 0;
			ScrollPanel(700, 700, &sp2bOffsetX, &sp2bOffsetY) {
				ButtonGrid(5, 5);
			}
		}
#else
		DummyElement(1);
#endif
		// Static width & dynamic height
		static int sp4OffsetX = 0;
		ScrollPanel(700, 0, &sp4OffsetX, nullptr) {
			ButtonGrid(5, 3);
		}

		// Dynamic width and static height
		static int sp3OffsetY = 0;
		ScrollPanel(0, 600, nullptr, &sp3OffsetY) {
			ButtonGrid(1, 10);
		}
	}
}

void TestPanels() {
	static int tab = 0;
	TabPanel("Split,Tab,Scroll", tab) {
		switch (tab) {
		case 0 : TestSplitPanel();  break;
		case 1 : TestTabPanel();    break;
		case 2 : TestScrollPanel(); break;
		}
	}
}

void TestDemo1() {
	Panel(BorderLayout(GUI_VERTICAL, 0.15f, 0.15f)) {
		Label("Header", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
		Panel(BorderLayout(GUI_HORIZONTAL, 0.2f, 0.2f)) {
			static int oy = 0;
			ScrollPanel(0, 10 * 30, nullptr, &oy) {
				SetLayout(GridLayout(1, 10, 0));
				for (uint32_t index = 0; index < 10; ++index) {
					Button("Test");
				}
			}

			static int obx = 200 - 32, oby = 0;
			ScrollPanel(800, 400, &obx, &oby) {
				SetLayout(GridLayout(1, 10));
				Label("! @ # $ % ^ & * ( ) _ + ");
				Label("1 2 3 4 5 6 7 8 9 0");
				Label("a b c d e f g h i j k l m n o p q r s t u v w x y z");
				Label("` ~ [ ] \\ ; ' , . / { } | : \" < > ? ");
				Label("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
				Label("! @ # $ % ^ & * ( ) _ + ");
				Label("1 2 3 4 5 6 7 8 9 0");
				Label("a b c d e f g h i j k l m n o p q r s t u v w x y z");
				Label("` ~ [ ] \\ ; ' , . / { } | : \" < > ? ");
				Label("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
			}
			Label("Right", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
		}
		Label("Footer", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
	}
}

void TestDemo2() {
	const char* MESSAGES[] = {
		"",
		"It's X's turn.",
		"It's O's turn.",
		"X won!",
		"O won!",
	};
	const char* CELL_LABEL[] = { "", "X", "O" };
	
	static int state = 0;
	static int cells[9] = {};
	static float proc = 0.2f;
	static int scoreX = 0;
	static int scoreO = 0;
	
	char text[32];
	
	SplitPanel(GUI_HORIZONTAL, proc) {
		Panel(GridLayout(1, 20)) {
			DummyElement(1);
			snprintf(text, 32, "X's score: %d", scoreX);
			Label(text);
			snprintf(text, 32, "O's score: %d", scoreO);
			Label(text);
			if (Button("Reset")) {
				scoreX = scoreO = 0;
			}
		}
		Panel() {
			switch (state) {
			case 0 :
				for (uint32_t index = 0; index < 9; ++index) {
					cells[index] = 0;
				}
				state = 1;
				break;
			case 1 :
			case 2 :
				Panel(GridLayout(3, 3)) {
					for (uint32_t index = 0; index < 9; ++index) {
						if (Button(CELL_LABEL[cells[index]]) && (cells[index] == 0)) {
							cells[index] = state;
							state = (state  % 2 + 1);
						}
					}
					uint16_t won = 0b1111111111111111;
					for (int index = 0; index < 3; ++index) {
						won = // X win conditions
						      (((won >>  0) & 0x1) & (cells[0 + 1 * index] == 1)) <<  0 |
						      (((won >>  1) & 0x1) & (cells[3 + 1 * index] == 1)) <<  1 |
							  (((won >>  2) & 0x1) & (cells[6 + 1 * index] == 1)) <<  2 |
							  (((won >>  3) & 0x1) & (cells[0 + 3 * index] == 1)) <<  3 |
							  (((won >>  4) & 0x1) & (cells[1 + 3 * index] == 1)) <<  4 |
							  (((won >>  5) & 0x1) & (cells[2 + 3 * index] == 1)) <<  5 |
							  (((won >>  6) & 0x1) & (cells[0 + 4 * index] == 1)) <<  6 |
							  (((won >>  7) & 0x1) & (cells[2 + 2 * index] == 1)) <<  7 |
							  // O win conditions
						      (((won >>  8) & 0x1) & (cells[0 + 1 * index] == 2)) <<  8 |
						      (((won >>  9) & 0x1) & (cells[3 + 1 * index] == 2)) <<  9 |
							  (((won >> 10) & 0x1) & (cells[6 + 1 * index] == 2)) << 10 |
							  (((won >> 11) & 0x1) & (cells[0 + 3 * index] == 2)) << 11 |
							  (((won >> 12) & 0x1) & (cells[1 + 3 * index] == 2)) << 12 |
							  (((won >> 13) & 0x1) & (cells[2 + 3 * index] == 2)) << 13 |
							  (((won >> 14) & 0x1) & (cells[0 + 4 * index] == 2)) << 14 |
							  (((won >> 15) & 0x1) & (cells[2 + 2 * index] == 2)) << 15 |
							  0;
					}
					if (won & 0xFF) {
						scoreX += 1;
						state = 3;
					} else if (won & 0xFF00) {
						scoreO += 1;
						state = 4;
					}
				}
				break;
			case 3 :
			case 4 :
				Panel(BorderLayout(GUI_HORIZONTAL)) {
					DummyElement();
					Panel(GridLayout(1, 10)) {
						DummyElement(3);
						Label(MESSAGES[state]);
						if (Button("Restart")) {
							state = 0;
						}
					}
				}
				break;
			}
		}
		ivec2 mpos = GUIGetActiveContext()->mousePosition;
		mpos.x += 15;
		mpos.y += 15;
		GUIDrawText((((state - 1) % 2) ? "O" : "X"), {mpos.x, mpos.y, mpos.x + 15, mpos.y + 15}, {255, 255, 255, 255}, 0);
	}
}

void TestDemo3() {
	struct Dialogue {
		struct Line {
			int person;
			const char* text;
		};
		const char* summary;
		const Line lines[8];
	};
	const Dialogue lines[] = {
		{ "Hello", 
			{
				{ 0, "Hello" },
				{ 1, "Oh, Hi Mark!" },
			}
		},
		{ "<TEST_DIAG0>", },
		{ "<TEST_DIAG1>", },
		{ "<TEST_DIAG2>", },
		{ "<TEST_DIAG3>", },
		{ "Bye", 
			{
				{ 0, "Bye" },
				{ 1, "See you around." },
			}
		},
	};
	const uint32_t LINE_COUNT = sizeof(lines) / sizeof(lines[0]);


	Panel(BorderLayout(GUI_VERTICAL, 0.2f, 0.25f)) {
		Layout* layout = GetLayout();
		layout->max.x = (GUIGetActiveContext()->viewport.z - GUIGetActiveContext()->viewport.x) * 0.9f;
		
		Label("<TEXT>", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
		DummyElement();
		
		static int offsetY = 0;
		ScrollPanel(0, 160, nullptr, &offsetY) {
			Panel(GridLayout(1, 6, 0)) {
				for (int index = 0; index < (int)LINE_COUNT; ++index) {
					if (Button(lines[index].summary)) {
					}
				}
			}
		}
	}
}

void TestDemo4() {
	static float hSplit = 0.2f;
	static float vSplit = 0.9f;
	static int offsetY = 0;
	SplitPanel(GUI_HORIZONTAL, hSplit) {
		ScrollPanel(0, 600, nullptr, &offsetY) {
			Panel(GridLayout(1, 15)) {
				for (int index = 0; index < 15; ++index) {
					if (Button("<TEST_CHAT>")) {
					}
				}
			}
		}
		SplitPanel(GUI_VERTICAL, vSplit) {
			const int LINE_COUNT = 20;
			static int offsetY = 0;
			ScrollPanel(0, LINE_COUNT * 20, nullptr, &offsetY) {
				Panel(GridLayout(1, LINE_COUNT)) {
					for (int index = 0; index < LINE_COUNT; ++index) {
						Label("<TEST_LINE>");
					}
				}
			}
			Panel(SplitLayout(GUI_HORIZONTAL, 0.8f, 2)) {
				static char text[256];
				static int carrot = -1;
				TextBox(text, sizeof(text), carrot);
				Button("Send");
			}
		}
	}
}

void TestDemos() {
	static int tab = 0;
	TabPanel("Webpage,X&O,Dialogue,Chat", tab) {
		switch (tab) {
		case 0 : TestDemo1(); break;
		case 1 : TestDemo2(); break;
		case 2 : TestDemo3(); break;
		case 3 : TestDemo4(); break;
		}
	}
}

void Test000() {
	static ivec4 bounds = {0, 0, 800, 600};
	static int tab = 0;

	if (windowFlags & GUI_VISIBLE) {
		Window(&bounds, "Test00", 0, &windowFlags) {
			TabPanel("Elements,Layouts,Panels,Demos", tab) {
				switch (tab) {
				case 0 : TestElements(); break;
				case 1 : TestLayouts();  break;
				case 2 : TestPanels();   break;
				case 3 : TestDemos();    break;
				}
			}
		}
	} else {
			TabPanel("Elements,Layouts,Panels,Demos", tab) {
				switch (tab) {
				case 0 : TestElements(); break;
				case 1 : TestLayouts();  break;
				case 2 : TestPanels();   break;
				case 3 : TestDemos();    break;
				}
			}
	}
}

int main(int argc, char* argv[]) {
	printf("sizeof(Layout) = %u\n", (uint32_t)sizeof(Layout));
	printf("sizeof(GUIStyle) = %u\n", (uint32_t)sizeof(GUIStyle));
	printf("sizeof(GUIContext) = %u\n", (uint32_t)sizeof(GUIContext));
	const uint32_t WINDOW_WIDTH = 1024;
	const uint32_t WINDOW_HEIGHT = 720;

	WCanvas canvas(WINDOW_WIDTH, WINDOW_HEIGHT, 32, "Immediate Mode GUI");
	uint32_t* pixelBuffer = (uint32_t*)canvas.getPixelBuffer();

	// GUI
	SoftGUI gui;
	SoftGUIInit(&gui, WINDOW_WIDTH, WINDOW_HEIGHT, pixelBuffer, "consola.ttf", 16.0f);
	gui.context.keyMap[GUI_KEY_HOME  ] = VK_HOME;
	gui.context.keyMap[GUI_KEY_END   ] = VK_END;
	gui.context.keyMap[GUI_KEY_LEFT  ] = VK_LEFT;
	gui.context.keyMap[GUI_KEY_RIGHT ] = VK_RIGHT;
	gui.context.keyMap[GUI_KEY_BACK  ] = VK_BACK;
	gui.context.keyMap[GUI_KEY_DELETE] = VK_DELETE;

	WEvent event;
	bool running = true;
	FileSelectorContext fileSelector;
	ReadDir(fileSelector, ".");
	
	while(running) {
		while (canvas.getEvent(event)) {
			switch (event.type) {
			case WEvent::Unknown :
				break;
			case WEvent::WindowClose :
				running = false;
				break;
			case WEvent::KeyPressed :
				if (event.ascii == '\0')  {
					GUIOnKeyEvent(event.keyCode, true);
				} else {
					GUIOnCharEvent(event.ascii);
				}
#if 1
				switch (event.keyCode) {
				case 0x41 : windowFlags ^= GUI_VISIBLE; break;
				}
#else
				printf("Pressed key 0x%X\n", event.keyCode);
#endif
				break;
			case WEvent::KeyReleased :
				if (event.keyCode == VK_ESCAPE) {
					running = false;
				} else if (event.keyCode == VK_SPACE) {
					ReadDir(fileSelector, ".");
				}
				break;
			case WEvent::CursorMove :
				WMOnCursorEvent(event.x, event.y);
				break;
			case WEvent::ButtonPressed :
				WMOnButtonEvent(event.button - 1, GUI_BUTTON_PRESSED);
				break;
			case WEvent::ButtonReleased :
				WMOnButtonEvent(event.button -1, GUI_BUTTON_RELEASED);
				break;
			case WEvent::WheelDown :
				GUIOnMouseWheelEvent(1);
				break;
			case WEvent::WheelUp :
				GUIOnMouseWheelEvent(-1);
				break;
			}
		}

		memset(pixelBuffer, 100, WINDOW_WIDTH * WINDOW_HEIGHT * 4);
		
		GUIFrame() {
#if 1
			Test000(); 
#else
			switch (FileSelector(fileSelector)) {
			case FS_SELECTED_FILE :
				printf("Selected file: %s\n", fileSelector.path);
				break;
			}
#endif
		}
		
		canvas.blit();
	}
	SoftGUIUninit(&gui);

	return 0;
}

float GetWidthProc(int size) {
	GUIContext* context = GUIGetActiveContext();
	float k = (float)(context->viewport.z - context->viewport.x);
	return (float)size / k;
}

float GetHeightProc(int size) {
	GUIContext* context = GUIGetActiveContext();
	float k = (float)(context->viewport.w - context->viewport.y);
	return (float)size / k;
}

int ReadDir(FileSelectorContext& fileSelector, const char* path, uint32_t flags) {
	int ans = 0;
	uint32_t offset = 0;
	
	fileSelector.cache[0] = 0;
	fileSelector.filter_name[0] = 0;
	fileSelector.filter_type = flags;
	fileSelector.carrot = -1;
	fileSelector.count = 0;
	fileSelector.filtering = false;
	fileSelector.flags = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_WINDOW_TITLEBAR | GUI_WINDOW_MOVE | GUI_WINDOW_SIZE;
	
	if (realpath(path, fileSelector.path) == nullptr){
		return -1;
	}

	DIR* dir = opendir(fileSelector.path);
	if (dir == nullptr) {
		fileSelector.cache[offset] = 0;
		return -1;
	}

	while (struct dirent *ent = readdir(dir)) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
			continue;
		}
		const uint32_t length = strlen(ent->d_name);
		if (offset + length >= sizeof(fileSelector.cache)) {
			printf("Cache overflow detected\n");
			goto LB_EXIT;
		}

		switch(ent->d_type) {
		case DT_DIR :
			fileSelector.cache[offset] = 'd';
			offset += 1;
			break;
		case DT_REG :
			fileSelector.cache[offset] = 'f';
			offset += 1;
			break;
		default :
			continue;
		}

		memcpy(fileSelector.cache + offset, ent->d_name, length);
		offset += length;

		fileSelector.cache[offset] = 0;
		offset += 1;

		++ans;
	}

LB_EXIT:
	closedir(dir);
	fileSelector.cache[offset] = 0;
	offset += 1;
	return ans;
}
#if 0
int FileSelectorPanel(FileSelectorContext& fileSelector) {
	int ans = 0;

	GUIContext* context = GUIGetActiveContext();
	const float lineHeight = 25.0f;
	const float width = context->viewport.z - context->viewport.x - 4;
	const float height = context->viewport.w - context->viewport.y - 24;
	const float lineProc = lineHeight / height;

	Panel(BorderLayout(GUI_VERTICAL, lineProc, 2.0f * lineProc, 0)) {
		// Header panel
		Panel(SplitLayout(GUI_VERTICAL, GetWidthProc(lineHeight), 0)) {
			if (Button(GUI_ICON_ARROW_UP)) {
				strncat(fileSelector.path, "/..", 255 - strlen(fileSelector.path));
				ReadDir(fileSelector, fileSelector.path);
			}
			Label(fileSelector.path, GUI_FLAGS_LABEL | GUI_OUTLINE);
		}
		// Body panel
		const ivec4 cbounds = LayoutGetAbsoluteBounds(ivec4(), false);
		fileSelector.count = (fileSelector.count == 0) ? 1 : fileSelector.count;
		const uint32_t y = (float)(cbounds.w - cbounds.y) / lineHeight;
		const uint32_t x = fileSelector.count / y + (fileSelector.count % y > 0);
		fileSelector.count = 0;
		Panel(GridLayout(x, y, 0)) {
			uint32_t offset = 0;
			uint32_t buttonFlags = 0;
			while (fileSelector.cache[offset]) {
				const bool dir = (fileSelector.cache[offset++] == 'd');
				if (((fileSelector.filter_type & GUI_FS_HIDDEN) == 0) && (fileSelector.cache[offset + 0] == '.')) {
					goto LB_NEXT_ENTRY;
				}
				if (((fileSelector.filter_type & GUI_FS_FILE) == 0) && (dir == false)) {
					goto LB_NEXT_ENTRY;
				}
				if (((fileSelector.filter_type & GUI_FS_DIR) == 0) && (dir == true)) {
					goto LB_NEXT_ENTRY;
				}
				if (fileSelector.filtering && fileSelector.filter_name[0] && strstr(&fileSelector.cache[offset], fileSelector.filter_name) == NULL) {
					goto LB_NEXT_ENTRY;
				}
				buttonFlags = GUI_FLAGS_BUTTON;
				if (strcmp(&fileSelector.cache[offset], fileSelector.filter_name) == 0) {
					buttonFlags |= GUI_CLICKED;
				}
				if (Button(dir? GUI_ICON_FOLDER : GUI_ICON_FILE, &fileSelector.cache[offset], buttonFlags)) {
					if (dir) {
						fileSelector.filter_name[0] = 0;
						sprintf(fileSelector.path, "%s/%s", fileSelector.path, &fileSelector.cache[offset]);
						ReadDir(fileSelector, fileSelector.path);
						break;
					} else {
						strcpy(fileSelector.filter_name, &fileSelector.cache[offset]);
					}
					fileSelector.filtering = false;
				}
				fileSelector.count += 1;
LB_NEXT_ENTRY:
				while (fileSelector.cache[offset++]); // Jump to next string
			}
		}

		// Footer panel
		Panel(GridLayout(1, 2, 0)) {
			Panel(SplitLayout(GUI_VERTICAL, 1.0f - (lineHeight / width), 0)) {
				if (TextBox(fileSelector.filter_name, sizeof(fileSelector.filter_name), fileSelector.carrot)) {
					fileSelector.filtering = true;
				}
				if (Button(GUI_ICON_CLOSE)) {
					fileSelector.filter_name[0] = 0;
					fileSelector.filtering = false;
				}
			}
			Panel(GridLayout(2, 1, 0)) { 
				if (Button("Cancel")) {
					ans = FS_CLOSED;
				}
				if (Button("Select") && fileSelector.filter_name[0]) {
					sprintf(fileSelector.path, "%s/%s", fileSelector.path, fileSelector.filter_name);
					ReadDir(fileSelector, fileSelector.path);
					ans = FS_SELECTED_FILE;
				}
			}
		}
	}

	return ans;
}

int FileSelector(FileSelectorContext& fileSelector) {
	static ivec4 bounds = {0, 0, 300, 300};
	int ans = 0;

	Window(&bounds, "File selector", 0, &fileSelector.flags) {
		switch((ans = FileSelectorPanel(fileSelector))) {
		case FS_SELECTED_FILE :
		case FS_SELECTED_DIR :
		case FS_CLOSED :
			fileSelector.flags &= ~GUI_VISIBLE;
			break;
		}
	}
	
	return ans;
}
#endif