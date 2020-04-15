Injectable Generic Camera System
============================

This is a generic injectable camera system which is used as a base for cameras for taking screenshots within games. 
The main purpose of the system is to hijack the in-game 3D camera by overwriting values in its camera structure
with our own values so we can control where the camera is located, it's pitch/yaw/roll values,
its FoV and the camera's look vector. Some camera implementations have additional features like timestop.

It's written in C++ with some x86/x64 assembler to be able to intercept the location of the 3D camera in the game. 
The system is initially designed for 64bit hosts as all games are 64bit nowadays, but has been reworked to be used for 32bit games too. 

## Re-hosting binaries
All binaries of my tools are hosted here. You're not allowed to re-host the binaries on other modsites, like Nexus Mods. 

## Folder structure description

In the folder `Cameras` you'll several implementations of the system, adapted for specific games. 

The cameras don't use a shared piece of code as in general cameras have to be adapted to a game pretty deeply and I didn't want to make a big
configurable ball. Additionally, cameras are often written once and perhaps fixed once or twice when the game is updated, but that's it. Copying
the code for each camera and adapting it makes possible to add new features to future cameras without affecting the older ones. 

## Requirements to build the code
To build the code, you need to have VC++ 2017 or higher, newer cameras need VC++ 2019. 
Additionally you need to have installed the Windows SDK, at least the windows 8 version. The VC++ installer should install this. 
The SDK is needed for DirectXMath.h

### External dependencies
There's an external dependency on [MinHook](https://github.com/TsudaKageyu/minhook) through a git submodule. This should be downloaded
automatically when you clone the repo. The camera uses DirectXMath for the 3D math, which is a self-contained .h file, from the Windows SDK. 

### Commercial usage 
These cameras and its system are made by volunteers in their spare time. If you use these camera for commercial activities 
(e.g. you make money with the shots or videos you produce with this camera), consider a donation. 

### Support
There's no support. If a camera breaks because of a game update, you're encouraged to send in a pull request with a fix. Cameras get updated from time to time
but the older the camera gets, the more likely it's not going to be updated. 

### Binaries with the releases
Some cameras have binaries with their release, most don't. Only the most recent cameras have binary distributions. If you need a binary for an older game,
ask, and I might re-upload it. 

## Camera's released: 
* Assassin's Creed 3: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/AssassinsCreed3
* Assassin's Creed Odyssey: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/AssassinsCreedOdyssey
* Assassin's Creed Origins: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/AssassinsCreedOrigins
* Batman Arkham Knight: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/BatmanArkhamKnight
* Bulletstorm Full Clip Edition: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/BulletstormFCE
* Deus Ex: Mankind Divided: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DXMD
* Devil May Cry 5: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DMC5
* Dishonored 2: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Dishonored2
* DOOM x64: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DOOM
* Dying Light: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DyingLight
* GreedFall: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Greedfall
* Hellblade: Senua's Sacrifice: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Hellblade
* Hitman 2016: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Hitman2016
* Homefront: The Revolution: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/HomefrontTheRevolution
* Injustice 2: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Injustice2
* Kingdom Come Deliverance: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/KingdomComeDeliverance
* Lords of the Fallen: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/LordsOfTheFallen
* Mafia III: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Mafia3
* Metal Gear Solid V: The Phantom Pain: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/MGS5
* Metro Exodus: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/MetroExodus
* Prey (2017): https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Prey
* Resident Evil 2: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/ResidentEvil2
* Resident Evil 3: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/ResidentEvil3
* Ryse: Son of Rome: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/RyseSonOfRome
* Shadow of the Tomb Raider: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/SOTTR
* Star Wars Jedi: Fallen Order: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/SWFO
* The Evil Within 2: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/TEW2
* Titanfall 2: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Titanfall2
* Tomb Raider 2013: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Tombraider
* Watch_Dogs: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Watch_Dogs
* Watch_Dogs2: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Watch_Dogs2
* Wolfenstein II: The new colossus: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Wolfenstein2

## In-depth article about IGCS and how to create camera tools
I've written a long, in-depth article about how to create camera tools and how IGCS works [on my blog](https://weblogs.asp.net/fbouma/let-s-add-a-photo-mode-to-wolfenstein-ii-the-new-colossus-pc).

## Acknowledgements
Some camera code uses [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu.

## Donations
I do this for fun, not profit, but can't create cameras for games I don't own. If you want me to make cameras for new games, please donate
to https://www.paypal.me/FransBouma. Additionally, after I've finished a game I usually uninstall it and it might be a camera gets broken after that date. 
For that I'm sorry.



