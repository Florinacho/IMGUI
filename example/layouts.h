#include <imgui.h>

void TestAbsoluteLayout() {
	Panel(AbsoluteLayout()) {
		if (Button("Test", GUI_FLAGS_BUTTON, {200, 200, 400, 400})) printf("Pressed Test\n");
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