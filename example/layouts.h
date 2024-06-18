#include <imgui.h>

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
	TabPanel("Split,FixedSplit,Border,Grid", tab) {
		switch (tab) {
		case 0 : TestSplitLayout();     break;
		case 1 : TestFixSplitLayout();  break;
		case 2 : TestBorderLayout();    break;
		case 3 : TestGridLayout();      break;
		}
	}
}
