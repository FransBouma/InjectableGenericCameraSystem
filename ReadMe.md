Injectable Generic Camera System
============================

This is a generic camera system to be used as a base for cameras for taking screenshots within games. 
The main purpose of the system is to hijack the in-game 3D camera by overwriting values in its camera structure
with our own values so we can control where the camera is located, it's pitch/yaw/roll values,
its FoV and the camera's look vector. 

It's written in C++ with some x86/x64 assembler to be able to intercept the location of the 3D camera in the game. 
The system is initially designed for 64bit hosts as all games are 64bit nowadays, but has been reworked to be used for 32bit games too. 

## Overview of folder structure

In the folder `Cameras` you'll find copies of the system source configured for specific games. In the folder `src` the plain system code is preset, which has to be changed for
the specific game you want to target. 

## Requirements to build the code
To build the code, you need to have VC++ 2013 or higher, but highly recommended is to use 2015, latest updates. Additionally you need to have installed the Windows SDK, at least the 
windows 8 version. The VC++ installer should install this. The SDK is needed for Direct input/xinput and DirectXMath.h

### External dependencies
It depends on DirectInput8 (which is part of every DX version). It uses DirectXMath for the 3D math, which is a self-contained library.

## Camera's released: 
* Hitman 2016: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Hitman2016
* Tomb Raider 2013: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/Tombraider
* Assassin's Creed 3: https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Cameras/AssassinsCreed3

## Acknowledgements
Uses input class from dhpoware for Mouse / Keyboard handling.  

