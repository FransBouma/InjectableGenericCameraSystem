AOBGen
============================
A simple AOB generator from copy/pasted disassembly.

![Screenshot 2022-01-22 162743](https://user-images.githubusercontent.com/3628530/150644907-3e4912a9-7d82-4c29-976f-abb51a2417e3.jpg)

This is a small tool that generates an AOB string based on the pasted in x64 assembly code. The x64 assembly code has to be either copied from Cheat Engine 
or x64dbg. 

By default it generates an AOB from all lines in the Assembly textbox, unless one or more lines are selected, in which case it will generate the AOB from 
the opcode bytes in the selected lines. It will wildcard RIP relative addresses, and optionally also offset bytes. It doesn't have any knowledge of which opcodes
are in the code, it does this solely using string logic, so it might make a mistake in edge cases. 

When an AOB has been generated, you can copy it to the clipboard using the `Copy AOB` button or using the `Copy AOB and asm as Markdown` button. The
latter copies the code in code fence lines as well as the AOB for easy usage in your reverse engineering documentation. 

**NOTE**: The tool doesn't know if the AOB is unique, you have to test that yourself.

Release binaries: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/AOBGen_100  
Credits: Otis_Inf

### EULA

Re-hosting binary builds of AOBGen is prohibited unless you have been given permission to do so. 

### License

MIT
