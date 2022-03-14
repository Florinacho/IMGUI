# GUI   
Cross-platform immediate mode GUI library for C++.  
GUI widgets supported:  
* Label  
* Button  
* CheckBox  
* Toggle  
* Spinner  
* ProgressBar  
* Slider  
* RangeSlider  
* TextBox  
* ListBox  
* ScrollBar  
* Panel  
* Window  
* SplitPanel  
* TabPanel  
* Group  

Currently implemented backends: Win32.  
The following function callbacks must be implemented for other backends:  
* GUIContext::drawLine
* GUIContext::drawQuad  
* GUIContext::drawText  
* GUIContext::drawIcon  
* GUIContext::getTextSize  

Win32 backend dependencies:  
 * STB TTF lib https://github.com/nothings/stb  