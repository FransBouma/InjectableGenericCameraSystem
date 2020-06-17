Injectable camera for Resident Evil 2
============================

Current supported game version: v1.0+  
Camera version: 1.0.2  
Credits: Otis_Inf and Jim2Point0.

For updates and support: https://www.patreon.com/Otis_Inf

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
