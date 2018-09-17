Injectable Generic Camera System
============================

This is a generic injectable camera system which is used as a base for cameras for taking screenshots within games. 
The main purpose of the system is to hijack the in-game 3D camera by overwriting values in its camera structure
with our own values so we can control where the camera is located, it's pitch/yaw/roll values,
its FoV and the camera's look vector. Some camera implementations have additional features like timestop.

It's written in C++ with some x86/x64 assembler to be able to intercept the location of the 3D camera in the game. 
The system is initially designed for 64bit hosts as all games are 64bit nowadays, but has been reworked to be used for 32bit games too. 

## Folder structure description

In the folder `Cameras` you'll several implementations of the system, adapted for specific games. 

The cameras don't use a shared piece of code as in general cameras have to be adapted to a game pretty deeply and I didn't want to make a big
configurable ball. Additionally, cameras are often written once and perhaps fixed once or twice when the game is updated, but that's it. Copying
the code for each camera and adapting it makes possible to add new features to future cameras without affecting the older ones. 

## Requirements to build the code
To build the code, you need to have VC++ 2015 update 3 or higher, newer cameras need VC++ 2017. 
Additionally you need to have installed the Windows SDK, at least the windows 8 version. The VC++ installer should install this. 
The SDK is needed for DirectXMath.h

### External dependencies
There's an external dependency on [MinHook](https://github.com/TsudaKageyu/minhook) through a git submodule. This should be downloaded
automatically when you clone the repo. The camera uses DirectXMath for the 3D math, which is a self-contained .h file, from the Windows SDK. 

## Camera's released: 
* Assassin's Creed 3: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/AssassinsCreed3
* Assassin's Creed Origins: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/AssassinsCreedOrigins
* Batman Arkham Knight: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/BatmanArkhamKnight
* Bulletstorm Full Clip Edition: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/BulletstormFCE
* Deus Ex: Mankind Divided: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DXMD
* DOOM x64: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DOOM
* Dying Light: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/DyingLight
* Hellblade: Senua's Sacrifice: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Hellblade
* Hitman 2016: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Hitman2016
* Homefront: The Revolution: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/HomefrontTheRevolution
* Lords of the Fallen: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/LordsOfTheFallen
* Mafia III: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Mafia3
* Metal Gear Solid V: The Phantom Pain: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/MGS5
* Prey (2017): https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Prey
* Ryse: Son of Rome: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/RyseSonOfRome
* Shadow of the Tomb Raider: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/SOTTR
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


