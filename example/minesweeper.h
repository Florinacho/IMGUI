#ifndef __DEMO_MINESWEEPER_H__
#define __DEMO_MINESWEEPER_H__

#include <imgui.h>
#include <stdlib.h>
#include <time.h>

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

void minesweeperWindow(ivec4* bounds, uint32_t* flags) {
	Window(bounds, "Minesweeper", nullptr, 0, flags) {
		minesweeperPanel();
	}
}

#endif // __DEMO_MINESWEEPER_H__
