#include <stdio.h>

#include "WindowCanvas.h"
#include <imguisr.h>
#include <imgui.h>

#include "desktop.h"

#if defined (__linux__)
#define KEY_ESCAPE     0x09
#define KEY_BACKSPACE  0x16
#define KEY_SPACE      0x41
#define KEY_HOME       0x6E
#define KEY_END        0x73
#define KEY_UP         0x6F
#define KEY_DOWN       0x74
#define KEY_LEFT       0x71
#define KEY_RIGHT      0x72
#define KEY_DELETE     0x77
#elif defined (_WIN32)
#define KEY_ESCAPE     VK_ESCAPE
#define KEY_BACKSPACE  VK_BACK
#define KEY_SPACE      VK_SPACE
#define KEY_HOME       VK_HOME
#define KEY_END        VK_END
#define KEY_UP         VK_UP
#define KEY_DOWN       VK_DOWN
#define KEY_LEFT       VK_LEFT
#define KEY_RIGHT      VK_RIGHT
#define KEY_DELETE     VK_DELETE
#endif

int main(int argc, char* argv[]) {
	const uint32_t WINDOW_WIDTH = 1024;
	const uint32_t WINDOW_HEIGHT = 720;

	WCanvas canvas(WINDOW_WIDTH, WINDOW_HEIGHT, 32, "Immediate Mode GUI");
	uint32_t* pixelBuffer = (uint32_t*)canvas.getPixelBuffer();

	// GUI
	SoftGUI gui;
	SoftGUIInit(&gui, WINDOW_WIDTH, WINDOW_HEIGHT, pixelBuffer, "consola.ttf", 16.0f);
	gui.context.keyMap[GUI_KEY_HOME  ] = KEY_HOME;
	gui.context.keyMap[GUI_KEY_END   ] = KEY_END;
	gui.context.keyMap[GUI_KEY_UP    ] = KEY_UP;
	gui.context.keyMap[GUI_KEY_DOWN  ] = KEY_DOWN;
	gui.context.keyMap[GUI_KEY_LEFT  ] = KEY_LEFT;
	gui.context.keyMap[GUI_KEY_RIGHT ] = KEY_RIGHT;
	gui.context.keyMap[GUI_KEY_BACK  ] = KEY_BACKSPACE;
	gui.context.keyMap[GUI_KEY_DELETE] = KEY_DELETE;

	desktopDemoInit();

	WEvent event;
	bool running = true;

	while(running) {
		while (canvas.getEvent(event)) {
			switch (event.type) {
			case WEvent::Unknown :
				break;
			case WEvent::WindowClose :
				running = false;
				break;
			case WEvent::KeyPressed :
				if (event.ascii == '\0')  {
					GUIOnKeyEvent(event.keyCode, true);
				} else {
					GUIOnCharEvent(event.ascii);
				}
				switch (event.keyCode) {
				case KEY_ESCAPE :
					running = false;
					break;
				}
				break;
			case WEvent::KeyReleased :
				break;
			case WEvent::CursorMove :
				WMOnCursorEvent(event.x, event.y);
				break;
			case WEvent::ButtonPressed :
				WMOnButtonEvent(event.button - 1, GUI_BUTTON_PRESSED);
				break;
			case WEvent::ButtonReleased :
				WMOnButtonEvent(event.button -1, GUI_BUTTON_RELEASED);
				break;
			case WEvent::WheelDown :
				GUIOnMouseWheelEvent(1);
				break;
			case WEvent::WheelUp :
				GUIOnMouseWheelEvent(-1);
				break;
			}
		}

		memset(pixelBuffer, 100, WINDOW_WIDTH * WINDOW_HEIGHT * 4);

		GUIFrame() {
			desktopDemo();
		}


		canvas.blit();
	}
	SoftGUIUninit(&gui);

	return 0;
}
