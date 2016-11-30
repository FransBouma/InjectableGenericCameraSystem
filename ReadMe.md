Injectable Generic Camera System
============================

This will hopefully grow to a generic camera system to be used for taking screenshots within games. 
The main purpose of the system is to hijack the in-game 3D camera by overwriting its camera matrix
with our own camera matrix so we can control where the camera is located, it's pitch/yaw/roll values,
its FoV and the camera's look vector. 

It's written in C++ with some x64 assembler to be able to intercept the location of the 3D camera in the game. 
The system is designed for 64bit hosts as all games are 64bit nowadays. 

## Core design

The core idea is that the generic camera system is used to _build_ injectable cameras with. This means that a camera hack for game XYZ uses the code
in this repository as its base, to which changes are made to make it work with game XYZ (which are in the area where the matrix is located, where to inject code etc.) and then
compiled specifically for that game XYZ. Most of the system is designed to be generic, so it doesn't need changing for a particular game, only a small part of
the system needs customization for the target game. 

There are a couple of elements in the system which have to play nice together. The system requires two core pieces of information, which are 
per-game specific. Usually these are obtained by using tools like Cheat Engine. 

* the address where the camera matrix is located
* the addresses of statements which write to the camera matrix memory area. 

To obtain the address where the camera matrix is located at runtime, in general the game's code is overwritten at a crucial spot so 
execution flow will end up in a short routine in x64 assembler in the camera system. To do this, the system will write a `jmp interceptor` statement
onto the location in the game's in-memory exe image where a register contains the address of the camera matrix. In `interceptor` the assembly will obtain
the address from the given register and write it in a C++ variable. It will then continue executing the code as normal and a `jmp` back to the instruction after 
the one overwritten with `jmp interceptor` is 

The addresses of write statements are used to switch off the in-game input / camera system and make sure our own camera matrix is used instead. Every time the camera is *enabled*
by the user, the addresses of the write statements are replaced with `nop` statements so they do nothing, so the matrix the system calculates isn't overwritten by the game code. When the camera is *disabled*
these instructions are restored so the game continues as normal. 

### How it works

* the camera system is injected as a dll with an injector (like Extreme Injector) into the game's running 64bit process. 
* upon attach, the camera system will start a thread which will: 
	* open a console and redirect stdout/in/err to that console
	* obtain the host's base address in its own memory space
	* start the core system
* the core system will do the following: 
	* put the interceptor asm code in place in the game's in-memory image so the system's interceptor assembly is called and able to obtain the
      camera matrix address. 
	* wait for the camera matrix address to become filled in. 
	* once the camera matrix is found, it will initialize the system's camera object with the current matrix' look vector and position.
	* the system will then go into the input processing loop. 

The input processing loop will obtain input from XInput. If the user enables the camera, it will act upon what keys the user presses. 
Based on the user's input it will call functions of the system's camera matrix. After the input processing is complete, the system's camera object is asked to calculate
a new camera matrix. This camera matrix is then written on the location of the game's camera matrix. 

## Overall customization per game
The system requires very little customization per game. Only the interceptor code is custom per game and which addresses to disable with `nop` statements is custom. All other code
is generic and can be re-used per game. 

## Current status
It's early days, the dll is now injectable, a console can be created and it can obtain the parent's process base address which is crucial to be able to overwrite any statements. 
More soon :)