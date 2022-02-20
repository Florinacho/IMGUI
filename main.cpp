#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdint.h>

#include <sys/time.h>
#include <windows.h>

#define IMGUI_INCLUDE_WINDOW_MANAGER
#define IMGUI_IMPLEMENTATION
#include "Backend.h"

int64_t GetMilliSeconds() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

void TriggerModal(void* userData, const ivec4& bounds) {
	int64_t* modalAlertTriggerTime = (int64_t*)userData;
	*modalAlertTriggerTime = GetMilliSeconds();
	printf("\a");
}

/*****************************************************************************/
/** GUI demo functions                                                      **/
/*****************************************************************************/
uint32_t resizeWindowFlags      = GUI_VISIBLE | GUI_WINDOW_DECORATION | GUI_WINDOW_MOVE | GUI_WINDOW_SIZE;
uint32_t utilWindowFlags        = GUI_DEFAULT_FLAGS_WINDOW &  (GUI_DEFAULT_FLAGS_WINDOW ^ GUI_WINDOW_CLOSE);
uint32_t styleConfiguratorFlags = GUI_VISIBLE | GUI_WINDOW_DECORATION | GUI_WINDOW_MOVE | GUI_WINDOW_SIZE;
uint32_t popupFlags             =               GUI_WINDOW_DECORATION | GUI_WINDOW_MOVE;

ivec4 utilWindowBounds = {0, 237, 304, 650};
ivec4 resizeWindowBounds = {0, 0, 499, 650};
ivec4 styleConfiguratorBounds = {500, 0, 1023, 650};
ivec4 popupBounds = {400, 300, 623, 411};

uint64_t lastSecondTime = GetMilliSeconds();
uint32_t frameCount = 0;
uint32_t fps = 0;

int64_t modalAlertTriggerTime = 0;

void ShowPopup() {
	GUIContext* context = GUIGetActiveContext();
	
	popupBounds = {360, 300, 623, 411};
	popupFlags |= GUI_VISIBLE;
	WMBringIDToFront(context, 3);
	context->modal = 3;
}

void UtilWindow() {
	Window(&utilWindowBounds, "", &utilWindowFlags) {
		SetLayout(AbsoluteLayout());
		if (Button("TITLEBAR", GUI_DEFAULT_FLAGS_BUTTON, {8, 8, 128, 28})) {
			resizeWindowFlags ^= GUI_WINDOW_TITLEBAR;
		}
		if (Button("VISIBLE", GUI_DEFAULT_FLAGS_BUTTON, {8, 36, 128, 56})) {
			resizeWindowFlags ^= GUI_VISIBLE;
		}
		if (Button("MOVABLE", GUI_DEFAULT_FLAGS_BUTTON, {8, 64, 128, 84})) {
			resizeWindowFlags ^= GUI_WINDOW_MOVE;
		}
		if (Button("SIZABLE", GUI_DEFAULT_FLAGS_BUTTON, {8, 92, 128, 112})) {
			resizeWindowFlags ^= GUI_WINDOW_SIZE;
		}
		static int scrollbarValue = 0;
		ScrollBar(scrollbarValue, 3, true,  0.33f, {136, 8, 160, 112});
		ScrollBar(scrollbarValue, 3, false, 0.33f, {8, 120, 128, 140});

		static bool checkboxValue = false;
		CheckBox(checkboxValue, GUI_DEFAULT_FLAGS_CHECKBOX, {8, 148, 28, 168});

		static int spinerValue = 123;
		Spinner(spinerValue, GUI_DEFAULT_FLAGS_SPINNER, {8, 176, 128, 196});

		static float sliderValue = 0.5f;
		Slider(sliderValue, GUI_DEFAULT_FLAGS_SLIDER, {168, 8, 188, 112});
		Slider(sliderValue, GUI_DEFAULT_FLAGS_SLIDER, {8, 206, 128, 226});
	}
}

void ResizeWindow() {
	Window(&resizeWindowBounds, "Resize window", &resizeWindowFlags) {
		static float weight = 0.5f;
		SplitPanel(false, weight) {
			static const char* tabNames[] = {"LIST", "SPLIT", "FPS", "MENU", "LOG"};
			static const uint32_t tabCount = sizeof(tabNames) / sizeof(tabNames[0]);
			static int selectedTab = 0;

			TabPanel(tabNames, tabCount, selectedTab) {
				switch (selectedTab) {
				case 0 :
					static int listBox1Selected = 0;
					static int listBox2Selected = 0;
					static int listBox2Offset = 0;
					static const char* ListBoxItems[] = {
						"Line 0",
						"Line 1",
						"Line 2",
						"Line 3",
						"Line 4",
						"Line 5",
						"Line 6",
						"Line 7",
						"Line 8",
						"Line 9",
					};
					static float weight = 0.5f;
					SplitPanel(true, weight) {
						static int offset = 0;
						if (ListBox(listBox1Selected, offset, ListBoxItems, 10)) {
							printf("ListBox: %d\n", listBox1Selected);
						}
						static float tp = 0.5f;
						SplitPanel(false, tp) {
							Label("Hello there");
							if (ListBox(listBox2Selected, listBox2Offset, ListBoxItems, 10, GUI_ALIGN_CENTER)) {
								printf("ListBox: %d\n", listBox2Selected);
							}
						}
					}
					break;
				case 1 :
					static float spp1 = 0.5f;
					static float spp2 = 0.5f;
					SplitPanel(false, spp1) {
						Label("Hello");
						SplitPanel(true, spp2) {
							Label("from the");
							Label("other side");
						}
					}
					break;
				case 2 :
					Panel(GridLayout(1, 1)) {
						Group("TEST") {
							Panel(SplitLayout(false, 0.85f)) {
								static char tmp[32];
								sprintf(tmp, "FPS: %d", fps);
								Label(tmp);
								
								static float progress = 0.5f;
								//progress = fmod(progress + 0.001f, 1.0f);
								Layout* layout = GetLayout();
								layout->max.y = 20;
								ProgressBar(progress);
							}
						}
					}
					break;
				case 3 :
					Panel(GridLayout(1, 9)) {
						Label("Options");

						Panel(SplitLayout(true, 0.5f)) {
							static float value = 0.3f;
							Label("Volume", GUI_VISIBLE | GUI_FOREGROUND | GUI_ALIGN_LEFT_TOP);
							Slider(value);
						}

						Panel(SplitLayout(true, 0.5f)) {
							static int value = 1;
							Label("Slots", GUI_VISIBLE | GUI_FOREGROUND  | GUI_ALIGN_LEFT_TOP);
							Spinner(value);
						}
						
						Panel(SplitLayout(true, 0.5f)) {
							static bool value = true;
							Label("Subtitle", GUI_VISIBLE | GUI_FOREGROUND  | GUI_ALIGN_LEFT_TOP);
							CheckBox(value);
						}
						
						Panel(SplitLayout(true, 0.5f)) {
							Label("Account", GUI_VISIBLE | GUI_FOREGROUND  | GUI_ALIGN_LEFT_TOP);
							static char text[32];
							static int carrot = -1;
							if (TextBox(text, sizeof(text), carrot)) {
								printf("TextBox1[%u]: \"%s\"\n", strlen(text), text);
							}
						}

						Panel(SplitLayout(true, 0.5f)) {
							Label("Password", GUI_VISIBLE | GUI_FOREGROUND  | GUI_ALIGN_LEFT_TOP);

							static char text[32];
							static int carrot = -1;
							if (TextBox(text, sizeof(text), carrot, GUI_DEFAULT_FLAGS_TEXTBOX | GUI_HIDDEN)) {
								printf("TextBox2: \"%s\"\n", text);
							}
						}

						DummyElement(2);
						
						Layout* layout = GetLayout();
						layout->max.x = 300;
						Panel(SplitLayout(true, 0.5f)) {
							Button("BACK");
							Button("APPLY");
						}
					}
					break;
				case 4 :
					Panel(SplitLayout(false, 0.9f)) {
						static std::string labelText = "";
						static char textBoxText[256] = {0};
						static int carrot = -1;
						Label(labelText.c_str(), GUI_VISIBLE | GUI_FOREGROUND);
						Panel(SplitLayout(true, 0.85f)) {
							TextBox(textBoxText, sizeof(textBoxText), carrot);
							if (Button("Send")) {
								labelText += textBoxText;
								labelText += "\n";
								textBoxText[0] = '\0';
								carrot = 0;
							}
						}
					}
					break;
				}
			}

			Panel(GridLayout(1, 3)) {
				for (uint32_t index = 0; index < 3; ++index) {
					static char tmp[32];
					snprintf(tmp, sizeof(tmp), "Group %d", index);
					Group(tmp) {
						SetLayout(GridLayout(4, 1));
						Label("Label");
						
						Panel(GridLayout(2, 2)) {
							static bool checked[] = {false, true, false};
							
							Layout* layout = GetLayout();
							layout->max.y = 30;
							
							Label("Check");
							if (CheckBox(checked[index])) {
								printf("Checkbox %d: %d\n", index, checked[index]);
							}
							
							Label("Toggle");
							if (Toggle(checked[index])) {
								printf("Toggle %d: %d\n", index, checked[index]);
							}
						}
						
						Panel(GridLayout(1, 3)) {
							Layout* layout = GetLayout();
							layout->max.y = 20;
							static int values[] = {0, 0, 0};
							if (Spinner(values[index])) {
								printf("Spinner %d.1: %d\n", index, values[index]);
							}
							if (Spinner(values[index])) {
								printf("Spinner %d.2: %d\n", index, values[index]);
							}
							if (Spinner(values[index])) {
								printf("Spinner %d.3: %d\n", index, values[index]);
							}
						}

						if (Button("Button")) {
							printf("Button %d pressed\n", index);
							ShowPopup();
						}
					}
				}
			}
		}
	}
}

void CustomSlider(uint8_t& value) {
	char tmp[8];
	float fvalue = ((float)value) / 255.0f;
	Panel(GridLayout(1, 1)) {
		Slider(fvalue);
		value = fvalue * 255.0f;
		sprintf(tmp, "%hhu", value);
		Label(tmp);
	}
}

void ColorSlider(const char* text, color& color) {
	Group(text) {
		SetLayout(GridLayout(3, 1));
		Layout* layout = GetLayout();
		layout->max.x = 35;
		CustomSlider(color.x);
		CustomSlider(color.y);
		CustomSlider(color.z);
	}
}

void StyleConfigurator() {
	GUIContext* context = GUIGetActiveContext();
	Window(&styleConfiguratorBounds, "Style", &styleConfiguratorFlags) {
		SetLayout(SplitLayout(true, 0.5f));
		Panel(GridLayout(1, 7)) {
			ColorSlider("TEXT",     context->style.colors[GUI_COLOR_TEXT]);
			ColorSlider("BORDER",   context->style.colors[GUI_COLOR_BORDER]);
			ColorSlider("TITLEBAR", context->style.colors[GUI_COLOR_TITLEBAR]);
			ColorSlider("PANEL",    context->style.colors[GUI_COLOR_PANEL]);
			ColorSlider("PANE",     context->style.colors[GUI_COLOR_PANE]);
			ColorSlider("FOCUSED",  context->style.colors[GUI_COLOR_FOCUSED]);
			ColorSlider("ACTIVE",   context->style.colors[GUI_COLOR_ACTIVE]);
		}
		Panel(GridLayout(1, 10)) {
			Group("TITLE") {
				int value = context->style.values[GUI_VALUE_TITLEBAR_HEIGHT];
				if (Spinner(value)) {
					context->style.values[GUI_VALUE_TITLEBAR_HEIGHT] = std::min((int)std::max(value, (int)context->style.values[GUI_VALUE_TEXT_SCALE] * 7 + 2), 40);
				}
			}
			Group("SCROLL") {
				int value = context->style.values[GUI_VALUE_SCROLLBAR_WIDTH];
				if (Spinner(value)) {
					context->style.values[GUI_VALUE_SCROLLBAR_WIDTH] = std::min((int)std::max(value, (int)context->style.values[GUI_VALUE_TEXT_SCALE] * 7 + 2), 40);
				}
			}
			Group("SLIDER") {
				int value = context->style.values[GUI_VALUE_SLIDER_WIDTH];
				if (Spinner(value)) {
					context->style.values[GUI_VALUE_SLIDER_WIDTH] = std::min((int)std::max(value, 3), 40);
				}
			}
			Group("GROUP OFFSET") {
				float proc = (float)context->style.values[GUI_VALUE_GROUP_OFFSET] / 255.0f;
				if (Slider(proc)) {
					context->style.values[GUI_VALUE_GROUP_OFFSET] = (uint8_t)(proc * 255.0f);
				}
			}
		}
	}
}

inline bool ModalFlash(int64_t modalAlertTriggerTime) {
	static const uint64_t ALERT_DURATION = 2100;
	static const uint64_t FLASH_DURATION = 300;
	const uint64_t timeDiff = GetMilliSeconds() - modalAlertTriggerTime;
	return ((timeDiff < ALERT_DURATION) && ((timeDiff / FLASH_DURATION) % 2) == 0);
}

void ModalPopup(int64_t modalAlertTriggerTime) {
	GUIContext* context = GUIGetActiveContext();
	bool styleChanged = false;
	GUIStyle styleBackup;
	
	if (styleChanged = ModalFlash(modalAlertTriggerTime)) {
		styleBackup = context->style;
		context->style.colors[GUI_COLOR_TITLEBAR] = context->style.colors[GUI_COLOR_ACTIVE];
		context->style.colors[GUI_COLOR_BORDER] = context->style.colors[GUI_COLOR_ACTIVE];
	}
	
	Window(&popupBounds, "POPUP", &popupFlags) { 
		SetLayout(SplitLayout(false, 0.6f));
		Label("You have clicked a button");
		Panel(GridLayout(3, 1)) {
			DummyElement();
			if (Button("OK")) {
				popupFlags ^= GUI_VISIBLE;
			}
			DummyElement();
		}
	}
	
	if (styleChanged) {
		context->style = styleBackup;
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	const uint32_t WINDOW_WIDTH = 1024;
	const uint32_t WINDOW_HEIGHT = 720;
	
	static HDC hdc;
	static HDC hDCMem;
	static HBITMAP bitmap;
	static HGDIOBJ oldBitmap;
	static uint32_t* bitmapPixels;
	static BITMAPINFO bitmapinfo = {0}; 	

	static GUIContext guiContext;
	
    switch (uMsg) {
	case WM_CREATE :
		hdc = GetDC(hwnd);
		if (hdc == 0) {
			printf("Win32 ERROR: Unable to get display handle.\n");
			return -1;
		}
		hDCMem = CreateCompatibleDC(hdc);

		bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapinfo.bmiHeader.biWidth = WINDOW_WIDTH;
		bitmapinfo.bmiHeader.biHeight = -WINDOW_HEIGHT;
		bitmapinfo.bmiHeader.biPlanes = 1;
		bitmapinfo.bmiHeader.biBitCount = 32;

		bitmap = ::CreateDIBSection(hDCMem, &bitmapinfo, DIB_RGB_COLORS, (VOID**)&bitmapPixels, NULL, 0);
		oldBitmap = ::SelectObject(hDCMem, bitmap);

		/*** GUI ***/
		GUIContextInit(&guiContext);
		guiContext.drawLine = MyDrawLine;
		guiContext.drawQuad = MyDrawQuad;
		guiContext.drawText = MyDrawText;
		guiContext.drawIcon = MyDrawIcon;
		guiContext.getTextSize = MyGetTextSize;
		guiContext.opaqueData = bitmapPixels;

		guiContext.viewport = {0, 0, (int32_t)WINDOW_WIDTH, (int32_t)WINDOW_HEIGHT};
		guiContext.layout = AbsoluteLayout();

		guiContext.keyMap[GUI_KEY_HOME  ] = VK_HOME;
		guiContext.keyMap[GUI_KEY_END   ] = VK_END;
		guiContext.keyMap[GUI_KEY_LEFT  ] = VK_LEFT;
		guiContext.keyMap[GUI_KEY_RIGHT ] = VK_RIGHT;
		guiContext.keyMap[GUI_KEY_BACK  ] = VK_BACK;
		guiContext.keyMap[GUI_KEY_DELETE] = VK_DELETE;

		guiContext.modalAlertProc = TriggerModal;
		guiContext.modalUserData = &modalAlertTriggerTime;

		WMRegister(&guiContext, &utilWindowBounds, &utilWindowFlags);
		WMRegister(&guiContext, &resizeWindowBounds, &resizeWindowFlags);
		WMRegister(&guiContext, &styleConfiguratorBounds, &styleConfiguratorFlags);
		WMRegister(&guiContext, &popupBounds, &popupFlags);

		GUISetActiveContext(&guiContext);
		return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

	case WM_MOUSEMOVE :
		WMOnCursorEvent(&guiContext, LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_LBUTTONDOWN :
		WMOnButtonEvent(&guiContext, GUI_BUTTON_LEFT, GUI_BUTTON_PRESSED);
		return 0;

	case WM_LBUTTONUP :
		WMOnButtonEvent(&guiContext, GUI_BUTTON_LEFT, GUI_BUTTON_RELEASED);
		return 0;
		
	case WM_MOUSEWHEEL :
		GUIOnMouseWheelEvent(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1);
		return 0;

	case WM_KEYDOWN :
		switch (wParam) {
		case VK_ESCAPE :
			PostQuitMessage(0);
			break;
		}
		GUIOnKeyEvent(wParam, true);
		return 0;
		
	case WM_CHAR :
		GUIOnCharEvent(wParam);
		return 0;
		
    case WM_PAINT:
		if (GetMilliSeconds() - lastSecondTime >= 1000ULL) {
			fps = frameCount;
			frameCount = 0;
			lastSecondTime += 1000ULL;
			printf("FPS: %d\n", fps);
		}
		
		ExtFloodFill(hdc, 0, 0, RGB(255, 255, 255), FLOODFILLSURFACE);
		memset(bitmapPixels, 75, WINDOW_WIDTH * WINDOW_HEIGHT * 4);
		
		GUIFrame() {
			for (uint32_t index = 0; index < guiContext.count; ++index) {
				guiContext.events_enabled = guiContext.windows[index].receiveEvents;
				switch (guiContext.windows[index].id) {
				case 0 : UtilWindow();                      break;
				case 1 : ResizeWindow();                    break;
				case 2 : StyleConfigurator();               break;
				case 3 : ModalPopup(modalAlertTriggerTime); break;
				}
			}
		}
		
		BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hDCMem, 0, 0, SRCCOPY);
		++frameCount;
        return 0;
    }
	
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    // Register the window class.
    const char CLASS_NAME[]  = "Sample Window Class";
	const uint32_t WINDOW_WIDTH = 1024;
	const uint32_t WINDOW_HEIGHT = 720;
	const DWORD dwstyle = WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZEBOX | WS_VISIBLE;
	
	HWND hwnd;
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

	RECT r = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	AdjustWindowRect(&r, dwstyle, false);

    // Create the window.
    hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        "Learn to Program Windows",    // Window text
        dwstyle,                        // Window style
        // Size and position
        0, 0, r.right - r.left, r.bottom - r.top, // r.left, r.top, 
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );
    if (hwnd == NULL) {
        return 1;
    }

	/*** STB Font ***/
	LoadTTFFont(NULL, "consola.ttf", 16.0f);

    // Run the message loop.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}