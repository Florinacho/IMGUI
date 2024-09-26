# Cross-platform immediate mode GUI library for C++

## Example:
```
Panel(SplitLayout(GUI_HORIZONTAL)) {
    static char text[64];
    static int carrot = -1;

    TextBox(text, sizeof(text), carrot);

    if (Button("print")) {
        printf("Text: %s\n", text);
    }
}
```

## Widgets supported:
- Label
- Button
- CheckBox
- Toggle
- Spinner
- ProgressBar
- Slider
- RangeSlider
- TextBox
- ScrollBar
- Panel

## Containers:
- Window
- SplitPanel
- TabPanel
- ScrollPanel

## Layouts supported:
- Absolute
- Split
- Border
- Grid

## Backends:
The lib uses software rendering for drawing but this can be easily replaced by implementing the following callbacks:
- GUIContext::drawLine
- GUIContext::drawQuad
- GUIContext::drawText
- GUIContext::drawIcon
- GUIContext::getTextSize

## Dependencies:
- Software rendering: [WindowCanvas](https://github.com/Florinacho/WindowCanvas), [STB TrueType](https://github.com/nothings/stb)
