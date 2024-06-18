#include <imgui.h>

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
