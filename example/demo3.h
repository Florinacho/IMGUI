#include <imgui.h>

void demo3Panel() {
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
