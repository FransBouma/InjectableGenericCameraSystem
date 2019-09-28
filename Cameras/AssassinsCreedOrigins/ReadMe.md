Injectable camera for Assassin's Creed Origins
============================

Current supported game version: v1.4+ (UPlay version)  
Binary download: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/ACO_1010  
Camera version: 1.0.10  
Credits: Otis_Inf. Thanks to DeadEndThrills for HUD render location.

### Important
This camera and its system is made by volunteers in their spare time. If you use this camera for commercial activities 
(e.g. you make money with the shots or videos you produce with this camera), consider a donation. 

There's no support. If the camera breaks because of a game update, you're encouraged to send in a pull request with a fix.
Read the enclosed readme for details how to use it. 

### Changes
* v1.0.10: Fixed for v1.51 of the game (FoV and ToD correction)
* v1.0.9:  Fixed issue with outdated code in interceptor which caused lighting/shadow mess when using the camera in certain areas.
* v1.0.8:  Fixed for v1.2 of the game (ToD correction)
* v1.0.7:  Fixed for v1.1 of the game.
* v1.0.6:  For game pause/unpause it now calls the native game functions for engine pause/unpause. Should fix any crashes related to pause/unpause.
* v1.0.5:  Added more reliable timestop and HUD toggle.
* v1.0.4:  Added own injector, fixed some offsets for game version v1.0.5
* v1.0.3:  Added timestop 
* v1.0.2:  Fixed for v1.03 of the game.
* v1.0.1:  Added: time of day selection in camera menu.

### Acknowledgements
This camera uses [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu.
