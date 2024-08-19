#ifndef __DEMO_CALCULATOR_H__
#define __DEMO_CALCULATOR_H__

#include <imgui.h>

void calculatorPanel() {
	static const char* LABELS[] = {
		"7", "8", "9", "-",
		"4", "5", "6", "+",
		"1", "2", "3", "/",
		"0", "=", ".", "*",
	};

	static char text[64];
	static int carrot = -1;

	Panel(SplitLayout(GUI_VERTICAL, 0.2f)) {
		TextBox(text, sizeof(text), carrot);

		Panel(GridLayout(4, 4, 2)) {
			for (auto label : LABELS) {
				if (Button(label)) {
					switch (label[0]) {
					case '=' :
						// calculate
						text[0] = 0;
						break;
					default :
						strncat(text, label, sizeof(text) - strlen(text) - 1);
						break;
					}
				}
			}
		}
	}
}

void calculatorWindow(ivec4* bounds, uint32_t* flags) {
	Window(bounds, "Calculator", "", 0, flags) {
		calculatorPanel();
	}
}

#endif // __DEMO_CALCULATOR_H__
