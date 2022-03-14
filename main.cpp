#include <string>
#include <stdio.h>
#include <windows.h>

#define IMGUI_IMPLEMENTATION
#include "Backend.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void GUICalculatorWindow();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	// Register the window class.
	const char CLASS_NAME[]  = "Sample Window Class";
	const uint32_t WINDOW_WIDTH = 1024;
	const uint32_t WINDOW_HEIGHT = 720;
	const DWORD dwstyle = WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZEBOX | WS_VISIBLE;

	WNDCLASS wc = { };
	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	RECT r = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	AdjustWindowRect(&r, dwstyle, false);

	// Create the window.
	HWND hwnd = CreateWindow(CLASS_NAME, "Win32 imGUI example", dwstyle, 0, 0, r.right - r.left, r.bottom - r.top, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) {
		printf("Win32 error: Unable to create window.\n");
		return 1;
	}
	
	// Create bitmap
	HDC hdc = GetDC(hwnd);
	if (hdc == 0) {
		printf("Win32 error: Unable to get display handle.\n");
		return -1;
	}
	HDC hDCMem = CreateCompatibleDC(hdc);
	SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)hDCMem);
	
	BITMAPINFO bitmapinfo = {0}; 
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfo.bmiHeader.biWidth = WINDOW_WIDTH;
	bitmapinfo.bmiHeader.biHeight = -WINDOW_HEIGHT;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biBitCount = 32;

	uint32_t* bitmapPixels = NULL;
	HBITMAP bitmap = ::CreateDIBSection(hDCMem, &bitmapinfo, DIB_RGB_COLORS, (VOID**)&bitmapPixels, NULL, 0);
	HGDIOBJ oldBitmap = ::SelectObject(hDCMem, bitmap);

	// GUI
	Win32GUIContext win32GUI;
	Win32GUIInit(&win32GUI, "consola.ttf", 16.0f);
	win32GUI.context.opaqueData = bitmapPixels;
	win32GUI.context.viewport = {0, 0, (int32_t)WINDOW_WIDTH, (int32_t)WINDOW_HEIGHT};

	// Run the message loop.
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		memset(bitmapPixels, 100, WINDOW_WIDTH * WINDOW_HEIGHT * 4);
		
		GUIFrame() {
			GUICalculatorWindow();
		}
	}
	// TODO: Cleanup
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	const uint32_t WINDOW_WIDTH  = 1024; // TODO: calculate this
	const uint32_t WINDOW_HEIGHT = 720;

	HDC hDCMem = (HDC)GetWindowLongPtr(hwnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE :
		WMOnCursorEvent(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_LBUTTONDOWN :
		WMOnButtonEvent(GUI_BUTTON_LEFT, GUI_BUTTON_PRESSED);
		return 0;
	case WM_LBUTTONUP :
		WMOnButtonEvent(GUI_BUTTON_LEFT, GUI_BUTTON_RELEASED);
		return 0;
	case WM_MOUSEWHEEL :
		GUIOnMouseWheelEvent(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1);
		return 0;
	case WM_KEYDOWN :
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		GUIOnKeyEvent(wParam, true);
		return 0;
	case WM_CHAR :
		GUIOnCharEvent(wParam);
		return 0;
	case WM_PAINT:
		HDC hdc = GetDC(hwnd);
		ExtFloodFill(hdc, 0, 0, RGB(255, 255, 255), FLOODFILLSURFACE);
		BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hDCMem, 0, 0, SRCCOPY);
		ReleaseDC(hwnd, hdc);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GUICalculatorWindow() {
	static ivec4 bounds = {0, 0, 200, 300};
	static uint32_t flags = GUI_DEFAULT_FLAGS_WINDOW | GUI_WINDOW_SIZE;
	static char text[32] = {0};
	static int32_t carrot = -1;

	static const char* BUTTON_TEXT[4][4] = {
		"7", "8", "9", "+",
		"4", "5", "6", "-",
		"1", "2", "3", "*",
		"0", ".", "=", "/",
	};

	Window(&bounds, "Calculator", &flags) {
		SetLayout(SplitLayout(false, 0.18));
		TextBox(text, sizeof(text), carrot);
		Panel(GridLayout(4, 4)) {
			for (uint32_t y = 0; y < 4; ++y) {
				for (uint32_t x = 0; x < 4; ++x) {
					if (Button(BUTTON_TEXT[y][x])) {
						strncat(text, BUTTON_TEXT[y][x], sizeof(text) - strlen(text) - 1);
					}
				}
			}
		}
	}
}
