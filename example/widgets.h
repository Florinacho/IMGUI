#include <imgui.h>

#include <stdio.h>
#include <string.h>
#include <algorithm>

void TestWidgets() {
	Panel(SplitLayout(GUI_VERTICAL, 0.3f)) {
		Panel(GridLayout(5, 6)) {
			Label("Text");
			Label(GUI_ICON_FILE);
			DummyElement(3);
			
			if (Button("Text")) {
				printf("Pressed text button\n");
			}
			if (Button(GUI_ICON_FILE)) {
				printf("Pressed icon button\n");
			}
			if (Button(GUI_ICON_FILE, "text")) {
				printf("Pressed icon&text button\n");
			}
			DummyElement(2);
			
			{
			static bool value = false;
			if (CheckBox(value)) {
				printf("Checkbox: %s\n", value ? "true" : "false");
			}
			}
			{
			static bool value = false;
			auto bkp = GetLayout()->max;
			GetLayout()->max = {40, 24};
			if (Toggle(value)) {
				printf("Toggle: %s\n", value ? "true" : "false");
			}
			GetLayout()->max = bkp;
			}
			DummyElement(3);
			
			{
			static int value = 0;
			if (Spinner(value)) {
				printf("Spinner: %d\n", value);
			}
			}
			{
			static int value = 0;
			const char* TEXT[] = {
				"Test 0",
				"Test 1",
				"Test 2",
				"Test 3",
			};
			if (Spinner(value, TEXT, 4)) {
				value = std::clamp(value, 0, 3);
				printf("Spinner: %d, %s\n", value, TEXT[value]);
			}
			}
			DummyElement(3);

			{
			static char text[128] = "Text";
			static int carrot = -1;
			if (TextBox(text, sizeof(text), carrot)) {
				printf("Textbox: %s\n", text);
			}
			}
			DummyElement(4);

			{
			static float progress = 1.0f;
			if (progress > 1.0f) {
				progress -= 1.0f;
			}
			progress += 0.0001f;
			ProgressBar(progress);
			}
			{
			static float value = 0.0f;
			if (Slider(value)) {
				printf("Slider: %f\n", value);
			}
			}
			{
			static float min = 0.0f;
			static float max = 1.0f;
			if (RangeSlider(min, max, GUI_HORIZONTAL)) {
				printf("Slider: %f, %f\n", min, max);
			}
			}
			{
			static float value = 0.5f;
			if (Scrollbar(value, 40.0f / 80.0f, GUI_HORIZONTAL)) {
				printf("Scrollbar: %f\n", value);
			}
			}
			DummyElement(1);
		}

		Panel(GridLayout(30, 3)) {
			DummyElement(1);
			{
			static float value = 0.0f;
			if (Slider(value)) {
				printf("Slider: %f\n", value);
			}
			}
			DummyElement(1);
			{
			static float min = 0.0f;
			static float max = 1.0f;
			if (RangeSlider(min, max, GUI_VERTICAL)) {
				printf("Slider: %f, %f\n", min, max);
			}
			}
			DummyElement(1);
			/**/
			{
			static float value = 0.5f;
			if (Scrollbar(value, 40.0f / 80.0f, GUI_VERTICAL)) {
				printf("Scrollbar: %f\n", value);
			}
			}
			DummyElement(1);
			/**/
		}
	}
}