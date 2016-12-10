Injectable Generic Camera System
============================

This is a generic camera system to be used as a base for cameras for taking screenshots within games. 
The main purpose of the system is to hijack the in-game 3D camera by overwriting values in its camera structure
with our own values so we can control where the camera is located, it's pitch/yaw/roll values,
its FoV and the camera's look vector. 

It's written in C++ with some x86/x64 assembler to be able to intercept the location of the 3D camera in the game. 
The system is initially designed for 64bit hosts as all games are 64bit nowadays, but can be reworked to be used for 32bit games. To do so, you have to change the C++ / Linker parameters
of the VC++ project and make sure the assembler is generating x86 code.

## Requirements to build the code
To build the code, you need to have VC++ 2013 or higher, but highly recommended is to use 2015, latest updates. Additionally you need to have installed the Windows SDK, at least the 
windows 8 version. The VC++ installer should install this. The SDK is needed for Direct input/xinput and DirectXMath.h

### External dependencies
It depends on DirectInput8 (which is part of every DX version). It uses DirectXMath for the 3D math, which is a self-contained library.

## Core design
The core idea is that the generic camera system is used to _build_ injectable cameras with. This means that a camera hack for game XYZ uses the code
in this repository as its base, to which changes are made to make it work with game XYZ (which are in the area where the camera values are located, where to inject code etc.) and then
compiled specifically for that game XYZ. Most of the system is designed to be generic, so it doesn't need changing for a particular game, only a small part of
the system needs customization for the target game. 

There are a couple of elements in the system which have to play nice together. The system requires two core pieces of information, which are 
per-game specific. Usually these are obtained by using tools like Cheat Engine. 

* the address where the camera values are located (the camera view matrix and camera coordinates in world space. Some games use quaternion based camera rotations)
* the addresses of statements which write to the camera values. 

To obtain the address where the camera values are located at runtime, in general the game's code is overwritten at a crucial spot so 
execution flow will end up in a short routine in x64 assembler in the camera system. To do this, the system will write a `jmp cameraAddressInterceptor` statement
onto the location in the game's in-memory exe image where a register contains the address of the camera values. In `cameraAddressInterceptor` the assembly will obtain
the address from the given register and write it in a C++ variable. It will then continue executing the code as normal and a `jmp` back to the instruction after 
the one overwritten with `jmp cameraAddressInterceptor` is then executed. The `cameraAddressInterceptor` interception routine checks whether the camera is enabled, and if so, will
skip the original statements, otherwise will execute them. This makes the camera overwrites non-destructive for the code, so there's no re-writing needed when the camera is
enabled/disabled.

The addresses of write statements are used to switch off the in-game input / camera system and make sure our own camera values are used instead (be it the camera view matrix or
camera coordinates and a quaternion). This is done by additional interceptor functions in assembler, e.g. `cameraWriteInterceptor1`. For each write block an interceptor function
is created and code is redirected through these functions using the same `jmp` trick as described above. If the camera is disabled the original statements are executed as normal,
if the camera is enabled, the write statements are skipped. Every write interceptor function also compares its destination register value with the intercepted one to see whether it is
going to write to the camera values or not and if not, it will execute the statements as normal.

## Overall customization per game
TBD.

## Current status
Everything works, one game camera done: Hitman 2016.

## Useful links
* Setting up Visual C++ to use with MASM to call / use x64 assembly: http://lallouslab.net/2016/01/11/introduction-to-writing-x64-assembly-in-visual-studio/
* MASM syntax highlighting: https://marketplace.visualstudio.com/items?itemName=Trass3r.AsmHighlighter. 

## Useful info / notes
* The camera doesn't do any AOB scanning, as this is very complicated. Naively I tried to simply scan through memory with a byte pointer, but not all pages of memory are allocated, 
or can be freed on the fly while you're scanning them. It's then a problem what to do which can end up in undefined memory and become very slow. (what we're trying to find might be
inside a DLL which is loaded at a higher address space). To rework the camera to a new version of the game it's for, try CE's assembler search in memory view. In general it's quite
easy to find back the memory location of the blocks intercepted (as the code blocks are in the interceptor, so the original statements are known) and correct the camera. 

## Acknowledgements
Uses input class from dhpoware for Mouse / Keyboard handling.  

