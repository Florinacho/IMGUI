#include <imgui.h>

void demo4Panel() {
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