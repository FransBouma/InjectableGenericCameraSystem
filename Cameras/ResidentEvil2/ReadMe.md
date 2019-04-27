Injectable camera for Resident Evil 2
============================

Current supported game version: v1.0+  
Camera version: 1.0.0  
Camera release binaries: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/RE2_100  
Credits: Otis_Inf and Jim2Point0.

Brought to you by [FRAMED. Screenshotting community](https://framedsc.github.io). 

![](https://framedsc.github.io/Images/FRAMED_LogoBigDarkTransparent800px.png)

### Features

- Camera control: (Also in cut scenes and during a paused game)
	- FoV control
	- Free unlimited camera movement and rotation 
- Game pause / unpause, also in cut scenes. 
- Resolution scaling
- Hud toggle
- Aspect Ratio selection.
- DoF removal in cutscenes.
- Vignette removal

### Important: DirectX 11 only
The tools only work with DirectX11 of the game. Use the in-game setting to switch to DirectX11 if you've opted to use DirectX12
*before* you inject the tools.

### How to use
Read the enclosed readme.txt for details, really! Run the game as **administrator** otherwise it won't work.

### Important
* Be careful with the resolution scaling factor in the camera tools settings. Using a value of 4-5 or higher with a very 
high resolution will likely make the game become unresponsive and crash if you don't have the latest greatest videocard.
Resolution scaling already creates a high-res framebuffer, so e.g. using a factor of 2.0 on a 5K resolution effectively
means the game renders a 10K image, something it won't be able to do, most likely.

### EULA
To use these camera tools, you have to comply to the following:
If you ask me a question which is answered in the enclosed readme.txt, so i.o.w. you didn't read it at all, 
you owe me a new AAA game. Easy, right? 

### Acknowledgements
This camera uses [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu.
