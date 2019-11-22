Injectable camera for Assassin's Creed Odyssey
============================

Current supported game version: v1.5.0+ (UPlay version)  
Binary download: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/ACOD_1013  
Camera version: 1.0.13  
Credits: Otis_Inf. 

### Important
This camera and its system is made by volunteers in their spare time. If you use this camera for commercial activities 
(e.g. you make money with the shots or videos you produce with this camera), consider a donation. 

There's no support. If the camera breaks because of a game update, you're encouraged to send in a pull request with a fix.
Read the enclosed readme for details how to use it. 

### Features

- Camera control: (Also in cut scenes and during a paused game)
	- FoV control
	- Free unlimited camera movement and rotation 
- Game pause / unpause, also in cut scenes. 
- Time of Day control
- Resolution scaling
- Photomode range limiter removal
- HUD toggle
- Disable in-game DoF when camera is enabled. (will re-enable when you disable camera).
- Aspect Ratio limitations have been removed.
- Fog control (much higher than photomode allows)

Experimental:
- Panorama multi-shot
- Lightfield multi-shot
- Single screenshot

### Change log:
v1.0.13: Ctrl-Mousewheel resizes the window again.
v1.0.12: Fixed bug when injecting the camera into a minimized/fullscreen game. 
v1.0.11: Fixed bug in multi-shot functionality and hotsampling where a backbuffer wasn't released properly
v1.0.10: Added Panorama multi-shot functionality.
v1.0.9: Update for v1.5.0 of the game. Merge of Lightfield/single shot screenshot functionality.
v1.0.8: Better AOB scanner added which can deal better with DRM driven block moves. When enabling the camera, in-game DOF is now properly disabled, 
it's no longer necessary to enable the camera menu to disable in-game dof. 
v1.0.7: Fix for 1.4.0 of the game.
v1.0.6: Fixed ToD interception
v1.0.5: Fix for 1.1.4 of the game. 
v1.0.4: Multiple actions bound to the same key are now supported.
v1.0.3: Configurable keybindings added.
v1.0.2: Fog control added. Photomode goes up to 10, we go up to 200 (manual override is possible: ctrl-click and type a value). 
v1.0.1: Fix for 1.0.7 of the game: photomode range limit was broken. Added: AR limits removed 

### Acknowledgements
This camera uses [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu.
