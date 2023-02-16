#include <imgui.h>

void demo2Panel() {
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