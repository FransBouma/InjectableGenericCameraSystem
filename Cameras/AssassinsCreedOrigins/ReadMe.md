Injectable camera for Assassin's Creed Origins
============================

Current supported game version: v1.02+ (UPlay version)  
Camera version: 1.0.0  
Camera release binaries: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/ACO_100  
Credits: Otis_Inf

### How to use
Read the enclosed readme.txt for details, really! Run the game as **administrator** otherwise it won't work.

### Important
* MSI Afterburner (and like other stuff that creates an overlay) do **not** work together with the camera tools: 
the game either crashes or the overlay won't show up. Use the Ubisoft Uplay overlay to take screenshots instead 
as that works OK: configure it to save uncompressed copies of the screenshots in the UPlay settings. 
I'm sorry but I can't fix this. 

* Be careful with the resolution scaling factor in the camera tools settings. Using a value of 2 or higher with a very 
high resolution will likely make the game become unresponsive and crash if you don't have the latest greatest videocard.
Resolution scaling already creates a high-res framebuffer, so e.g. using a factor of 2.0 on a 5K resolution effectively
means the game renders a 10K image, something it won't be able to do, most likely.

* These camera tools were made with and for the UPlay version of the game. If you use it on the Steam version, it might not
work, though I expect it to work OK as the code likely is the same so the AOB scans will find the code regardless. If it doesn't
work, then... too bad, sorry.

### EULA
To use these camera tools, you have to comply to the following:
If you ask me a question which is answered in the enclosed readme.txt, so i.o.w. you didn't read it at all, 
you owe me a new AAA game. Easy, right? 

### Acknowledgements
This camera uses [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu.
