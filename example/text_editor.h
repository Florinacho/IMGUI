#ifndef __DEMO_TEXT_EDITOR_H__
#define __DEMO_TEXT_EDITOR_H__

#include <imgui.h>

static ivec4 notepadBounds = {200, 200, 500, 400};
static uint32_t notepadFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;
static char notepadText[256] = "Hello World!\n:)";
static char notepadTitle[32] = "Notepad";

void notepadPanel() {
	static int carrot = -1;
	static uint32_t alignment = 0;
	static uint32_t flags = GUI_VISIBLE | GUI_ENABLED | GUI_BACKGROUND | GUI_FOREGROUND | GUI_OUTLINE | GUI_MULTILINE;

	Panel() {
		Panel(FixSplitLayout(GUI_VERTICAL, 22, 0)) {
			Panel(GridLayout(8, 1, 0)) {
				uint32_t bflags = 0;
				bflags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_LEFT) ? GUI_CLICKED : 0);
				if (Button(GUI_ICON_ARROW_LEFT, bflags)) alignment ^= GUI_ALIGN_LEFT;

				bflags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_RIGHT) ? GUI_CLICKED : 0);
				if (Button(GUI_ICON_ARROW_RIGHT, bflags)) alignment ^= GUI_ALIGN_RIGHT;

				bflags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_TOP) ? GUI_CLICKED : 0);
				if (Button(GUI_ICON_ARROW_UP, bflags)) alignment ^= GUI_ALIGN_TOP;

				bflags = GUI_FLAGS_BUTTON | ((alignment & GUI_ALIGN_BOTTOM) ? GUI_CLICKED : 0);
				if (Button(GUI_ICON_ARROW_DOWN, bflags)) alignment ^= GUI_ALIGN_BOTTOM;
			}
			TextArea(notepadText, sizeof(notepadText), carrot, flags | alignment);
		}
	}
}

void notepadWindow() {
	Window(&notepadBounds, notepadTitle, 2, &notepadFlags) {
		notepadPanel();
	}
}

#endif // __DEMO_TEXT_EDITOR_H__