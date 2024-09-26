#ifndef __TESTS_H__
#define __TESTS_H__

#include <imgui.h>


/*** Widgets ***/
#include <stdio.h>
#include <string.h>
#include <algorithm>

void TestWidgets() {
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
			auto bkp = guiGetLayout()->max;
			guiGetLayout()->max = {40, 24};
			if (Toggle(value)) {
				printf("Toggle: %s\n", value ? "true" : "false");
			}
			guiGetLayout()->max = bkp;
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
			if (Slider(value, GUI_HORIZONTAL)) {
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
			if (Slider(value, GUI_VERTICAL)) {
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


/*** Layouts ***/
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
	const float NORD_PROC = 0.25f;
	const float SOUTH_PROC = 0.25f;
	const float WEST_PROC = 0.25f;
	const float EAST_PROC = 0.25f;

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
	const int COLLUMN_COUNT = 16;
	const int ROW_COUNT = 16;
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
	TabPanel("Split,FixedSplit,Border,Grid", tab) {
		switch (tab) {
		case 0 : TestSplitLayout();     break;
		case 1 : TestFixSplitLayout();  break;
		case 2 : TestBorderLayout();    break;
		case 3 : TestGridLayout();      break;
		}
	}
}


/*** Containers ***/
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
	guiSetLayout(GridLayout(x, y, padding));
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
		// Dynamic width and height
		static int sp1OffsetX = 0, sp1OffsetY = 0;
		ScrollPanel(700, 700, &sp1OffsetX, &sp1OffsetY) {
			ButtonGrid(5, 5);
		}
#if 0
		// Nested static width and height
		static int sp2OffsetX = 0, sp2OffsetY = 0;
		ScrollPanel(500, 500, &sp2OffsetX, &sp2OffsetY) {
			static int sp2bOffsetX = 0, sp2bOffsetY = 0;
			ScrollPanel(700, 700, &sp2bOffsetX, &sp2bOffsetY) {
				ButtonGrid(5, 5);
			}
		}
#else
		Label("Nested scroll panels are broken");
#endif
		// Static width & dynamic width
		static int sp4OffsetX = 0;
		ScrollPanel(700, 0, &sp4OffsetX, nullptr) {
			ButtonGrid(6, 1);
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

#endif // __TESTS_H__
