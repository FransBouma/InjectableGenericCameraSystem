Injectable camera for Resident Evil 2
============================

Current supported game version: v1.0+  
Binary download: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/RE2_102  
Camera version: 1.0.2  
Credits: Otis_Inf and Jim2Point0.

### Important
This camera and its system is made by volunteers in their spare time. If you use this camera for commercial activities 
(e.g. you make money with the shots or videos you produce with this camera), consider a donation. 

There's no support. If the camera breaks because of a game update, you're encouraged to send in a pull request with a fix.
Read the enclosed readme for details how to use it. 

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

### DirectX 11 only
The tools only work with DirectX11 of the game. Use the in-game setting to switch to DirectX11 if you've opted to use DirectX12
*before* you inject the tools.

### Scaling factor
* Be careful with the resolution scaling factor in the camera tools settings. Using a value of 4-5 or higher with a very 
high resolution will likely make the game become unresponsive and crash if you don't have the latest greatest videocard.
Resolution scaling already creates a high-res framebuffer, so e.g. using a factor of 2.0 on a 5K resolution effectively
means the game renders a 10K image, something it won't be able to do, most likely.

### Acknowledgements
This camera uses [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu.
