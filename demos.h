#ifndef __DEMOS_H__
#define __DEMOS_H__

#include <imgui.h>
#include <stdlib.h>
#include <time.h>


/** Browser **/
void browserPanel() {
	auto addressPanel = []() {
		Panel(BorderLayout(GUI_HORIZONTAL, 0.2f, 0.1f)) {
			Panel(GridLayout(3, 1)) {
				Button("<");
				Button(GUI_ICON_REFRESH);
				Button(GUI_ICON_HOME);
			}
			static int carrot = -1;
			static char address[32] = "www.test.org";
			TextBox(address, sizeof(address), carrot);
			Button("...");
		}
	};

	auto headerPanel = []() {
		Label("Header", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
	};

	auto bodyPanel = []() {
		auto contentLeftPanel = []() {
			static int oy = 0;
			ScrollPanel(0, 10 * 30, nullptr, &oy) {
				guiSetLayout(GridLayout(1, 10, 0));
				for (uint32_t index = 0; index < 10; ++index) {
					Button("Test");
				}
			}
		};

		auto contentCenterPanel = []() {
			static int obx = 200 - 32, oby = 0;
			ScrollPanel(800, 400, &obx, &oby) {
				guiSetLayout(GridLayout(1, 10));
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
		};

		auto contentRightPanel = []() {
			Label("Right", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
		};
		
		Panel(BorderLayout(GUI_HORIZONTAL, 0.2f, 0.2f)) {
			contentLeftPanel();
			contentCenterPanel();
			contentRightPanel();
		}
	};

	auto footerPanel = []() {
		Label("Footer", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
	};

	Panel(FixSplitLayout(GUI_VERTICAL, 25)) {
		addressPanel();
		Panel(BorderLayout(GUI_VERTICAL, 0.15f, 0.15f)) {
			headerPanel();
			bodyPanel();
			footerPanel();
		}
	}
}


/** Calculator **/
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
					if (label[0] == '=') {
						text[0] = '\0';
					} else {
						strncat(text, label, sizeof(text) - strlen(text) - 1);
					}
				}
			}
		}
	}
}


/*** Chat ***/
void chatPanel() {
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


/** Media player **/
void mediaPlayerPanel() {
	static const char* LIBRARY_BUTTON_LABELS[] = {
		"Open",
		"Home",
		"Music",
		"Video",
		"Queue",
		"Playlists",
	};
	static const char* PLAY_BUTTON_LABELS[] = {
		">",
		"||",
	};
	static bool playing = false;
	static uint32_t rawProgress = 0;
	static float progress = 0.0f;
	static int speed = 1;
	static float volume = 0.5f;

	static char labelText[32];

	const ivec4 bounds = guiLayoutGetAbsoluteBounds(false);
	const uint32_t width = bounds.z - bounds.x;

	if (playing) {
		snprintf(labelText, sizeof(labelText), "Playing %s at %dx ... %d%%", "test.mp4", speed, int(progress * 100.0f));
	} else {
		snprintf(labelText, sizeof(labelText), "Paused");
	}

	Panel(SplitLayout(GUI_VERTICAL, 0.85, 0)) {
		static float proc = 0.2f;

		if (width > 450 ) {
			SplitPanel(GUI_HORIZONTAL, proc) {
				static int xProg = 0;
				ScrollPanel(100, 200, NULL, &xProg) {
					Panel(FixSplitLayout(GUI_VERTICAL, 200)) {
						Panel(GridLayout(1, 6)) {
							for (auto label : LIBRARY_BUTTON_LABELS) {
								if (Button(label)) {
									printf("%s\n", label);
								}
							}
						}
					}
				}
				Label(labelText);
			}
		} else {
			Label(labelText);
		}

		Panel(FixSplitLayout(GUI_VERTICAL, 25, 0)) {
			progress = (double)rawProgress / (double)0x0000FFFF;
			Panel(BorderLayout(GUI_HORIZONTAL, 0.04f, 0.04f)) {
				if (Button("<<")) {
					speed = std::max(speed - 1, -5);
				}
				if (Slider(progress, GUI_HORIZONTAL)) {
					rawProgress = progress * 0x0000FFFF;
				}
				if (Button(">>")) {
					speed = std::min(speed + 1, 5);
				}
			}
			Panel(BorderLayout(GUI_HORIZONTAL, 0.35f, 0.35f)) {
				Panel(SplitLayout(GUI_VERTICAL)) {
					Label("Volume");
					Slider(volume, GUI_HORIZONTAL);
				}
				Panel(GridLayout(3, 1)) {
					if (Button("|<")) {
						rawProgress = 0;
					}
					if (Button(PLAY_BUTTON_LABELS[playing])) {
						playing = !playing;
					}
					if (Button(">|")) {
						rawProgress = 0;
					}
				}
				DummyElement();
			}
		}
	}
	rawProgress = std::clamp<uint32_t>(rawProgress + playing * speed, 0, 0x0000FFFF);
}


/*** Minesweeper ***/
static uint32_t collumnCount = 8;
static uint32_t rowCount = 10;
static uint32_t bombCount = 5;
static uint32_t tableSize = 0;
static uint8_t* table = NULL;

void newGamePanel() {
	static int cols = collumnCount;
	static int rows = rowCount;
	static int bombs = bombCount;

	Panel(GridLayout(1, 9)) {
		Label("MINESWEEPER");
		DummyElement();
		Spinner(cols);
		Spinner(rows);
		Spinner(bombs);
		DummyElement();
		DummyElement();
		if (Button("Start")) {
			srand (time(NULL));

			collumnCount = cols;
			rowCount = rows;
			tableSize = collumnCount * rowCount;
			table = new uint8_t[tableSize];
			memset(table, 0, tableSize);

			while (bombs > 0) {
				uint32_t cell = rand() % tableSize;
				if (table[cell] == 0) {
					table[cell] = '*';
					--bombs;
				} else {
					continue;
				}
			}
		}
		Button("Exit");
	}
}

int getNeighborCount(int index) {
	int ans = 0;
	
	int ix = index / collumnCount;
	int iy = index % collumnCount;

	for (int y = std::max<int>(iy - 1, 0); y < std::min<int>(iy + 1, rowCount); ++y) {
		for (int x = std::max<int>(ix - 1, 0); x < std::min<int>(ix + 1, collumnCount); ++x) {
			ans += (table[y * collumnCount + x] == '*');
		}
	}

	return ans;
}

int expand(int index) {
	int32_t count = 0;

	switch (table[index]) {
	case 0 :
		table[index] = 1;
		break;
	case 1 :
		break;
	case '*' :
		table[index] = 'X';
		count = -1;
	default :

		break;
	}

	return count;
}

void ingamePanel() {
	static bool dead = false;
	static uint32_t clicks = 0;

	Panel(FixSplitLayout(GUI_VERTICAL, 30)) {
		Panel(GridLayout(5, 1)) {
			Label("0");
			DummyElement();
			if (Button(dead ? ":(" : ":)")) {
				if (table != NULL) {
					delete [] table;
					table = NULL;
					dead = false;
					clicks = 0;
				}
				return;
			}
			DummyElement();
			{
			char tmp[16];
			snprintf(tmp, sizeof(tmp), "%d", clicks);
			Label(tmp);
			}
		}

		Panel(GridLayout(collumnCount, rowCount)) {
			for (uint32_t index = 0; index < tableSize; ++index) {
				char tmp[2] = {'\0', '\0'};
				uint32_t flags = GUI_FLAGS_BUTTON;
				if (dead) {
					flags ^= GUI_ENABLED;
				}

				switch(table[index]) {
				case 0 :
					break;
				case 1 :
					flags |= GUI_CLICKED;
					break;
				case 'X' :
					flags |= GUI_CLICKED;
				default :
					tmp[0] = table[index];
				}

				if (Button(tmp, flags)) {
					switch (table[index]) {
					case 1 :
						break;
					case '*' :
						dead = true;
						break;
					default :
						++clicks;
						break;
					}
					expand(index);
				}
			}
		}
	}
}

void minesweeperPanel() {
	if (table == NULL) {
		newGamePanel();
	} else {
		ingamePanel();
	}
}


/*** Notepad ***/
void notepadPanel() {
	static int carrot = -1;
	static uint32_t alignment = 0;
	static uint32_t flags = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_MULTILINE;
	static char notepadText[256] = "Hello World!\n:)";

	Panel(FixSplitLayout(GUI_VERTICAL, 22, 0)) {
		Panel(GridLayout(4, 1, 0)) {
			uint32_t buttonFlags = 0;

			buttonFlags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_LEFT  ) ? GUI_CLICKED : 0);
			if (Button(GUI_ICON_ARROW_LEFT,  buttonFlags)) alignment ^= GUI_ALIGN_LEFT;

			buttonFlags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_RIGHT ) ? GUI_CLICKED : 0);
			if (Button(GUI_ICON_ARROW_RIGHT, buttonFlags)) alignment ^= GUI_ALIGN_RIGHT;

			buttonFlags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_TOP   ) ? GUI_CLICKED : 0);
			if (Button(GUI_ICON_ARROW_UP,    buttonFlags)) alignment ^= GUI_ALIGN_TOP;

			buttonFlags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_BOTTOM) ? GUI_CLICKED : 0);
			if (Button(GUI_ICON_ARROW_DOWN,  buttonFlags)) alignment ^= GUI_ALIGN_BOTTOM;
		}
		TextArea(notepadText, sizeof(notepadText), carrot, flags | alignment);
	}
}


/*** Settings ***/
void colorEntry(const char* label, color_t& color) {	
	auto u8Slider = [](uint8_t& value) {
		GUIContext* gui = guiGetContext();
		const ivec4 bounds = guiLayoutGetAbsoluteBounds(false);
		bool ans = false;
		char tmp[4];
		
		float fvalue = (float)value / 255.0f;
		if (Slider(fvalue, GUI_HORIZONTAL, GUI_VISIBLE | GUI_ENABLED | GUI_FOREGROUND | GUI_OUTLINE)) {
			value = fvalue * 255.0f;
			ans = true;
		}
		snprintf(tmp, 4, "%.hhu", value);
		guiDrawText(tmp, bounds, gui->skin.colors[GUI_COLOR_TEXT], GUI_ALIGN_CENTER);
		return ans;
	};
	
	auto coloredRect = [](const color_t& color) {
		guiDrawQuad(guiLayoutGetAbsoluteBounds(), color);
	};

	Panel(GridLayout(5, 1)) {
		Label(label);
		u8Slider(color.x);
		u8Slider(color.y);
		u8Slider(color.z);
		coloredRect(color);
	}
}

void settingsPanel() {
	GUIContext* gui = guiGetContext();
	Panel(GridLayout(1, 9)) {
		colorEntry("Text",     gui->skin.colors[GUI_COLOR_TEXT]);
		colorEntry("Disabled", gui->skin.colors[GUI_COLOR_TEXT_DISABLED]);
		colorEntry("Border",   gui->skin.colors[GUI_COLOR_BORDER]);
		colorEntry("Panel",    gui->skin.colors[GUI_COLOR_PANEL]);
		colorEntry("Titlebar", gui->skin.colors[GUI_COLOR_TITLEBAR]);
		colorEntry("Pane",     gui->skin.colors[GUI_COLOR_PANE]);
		colorEntry("Focused",  gui->skin.colors[GUI_COLOR_FOCUSED]);
		colorEntry("Active",   gui->skin.colors[GUI_COLOR_ACTIVE]);

		if (Button("Reset")) {
			gui->skin.colors[GUI_COLOR_TEXT]          = {230, 230, 230, 255};
			gui->skin.colors[GUI_COLOR_TEXT_DISABLED] = {150, 150, 150, 255};
			gui->skin.colors[GUI_COLOR_TITLEBAR]      = { 25,  25,  25, 200};
			gui->skin.colors[GUI_COLOR_PANEL]         = { 50,  50,  50, 255};
			gui->skin.colors[GUI_COLOR_PANE]          = { 75,  75,  75, 255};
			gui->skin.colors[GUI_COLOR_FOCUSED]       = {110, 110, 120, 255};
			gui->skin.colors[GUI_COLOR_ACTIVE]        = { 75,  75, 255, 255};
			gui->skin.colors[GUI_COLOR_BORDER]        = { 25,  25,  25, 255};
		}
	}
}

/** TicTacToe **/
void ticTacToePanel() {
	const char* MESSAGES[] = {
		"",
		"It's X's turn.",
		"It's O's turn.",
		"X won!",
		"O won!",
	};
	const char* CELL_LABEL[] = { "", "X", "O" };
	
	static int state = 0;
	static uint8_t cells[9] = {};
	static float proc = 0.25f;
	static int scoreX = 0;
	static int scoreO = 0;
	
	char text[16];
	
	SplitPanel(GUI_VERTICAL, proc) {
		Panel(GridLayout(1, 2, 0)) {
			snprintf(text, sizeof(text), "X %.2d : %.2d O", scoreX, scoreO);
			Label(text);
			Panel(GridLayout(2, 1)) {
				if (Button("Reset score")) {
					scoreX = scoreO = 0;
				}
				if (Button("Restart")) {
					state = 0;
				}
			}
		}
		Panel() {
			switch (state) {
			case 0 : // Initialize
				memset(cells, 0, sizeof(cells));
				state = 1;
				break;
			case 1 : // X's turn
			case 2 : // O's turn
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
			case 3 : // X won
			case 4 : // O won
				Panel(BorderLayout(GUI_HORIZONTAL)) {
					DummyElement();
					Label(MESSAGES[state]);
				}
				break;
			}
		}
		ivec2 cursor = guiGetContext()->mousePosition;
		guiDrawText((((state - 1) % 2) ? "O" : "X"), {cursor.x + 15, cursor.y + 15, cursor.x + 30, cursor.y + 30}, {255, 255, 255, 255});
	}
}

#endif // __DEMOS_H__
