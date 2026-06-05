RTTI Dumper / explorer for Cheat Engine
=================================================

This is a small utility implemented in Lua script which is used inside Cheat Engine to dump a module's RTTI types, 
find instances of those types and reverse lookup instances in memory what RTTI type they have. 

### Usage

To use this script, start Cheat Engine, attach it to a process and open Memory Viewer. Then press `Ctrl-L` to open the Lua Engine window.
Click `File -> Open script` to open the `RTTI_Explorer_v5.lua` script. Then click the big `Execute` button on the right. A new window opens
showing the features of the tool. Its central panel logs the basic flow how to use it. 

### Disclaimer

This tool is mainly totally vibecoded using Claude by GhostInTheCamera with as starting point the [RTTI article on the Framed website](https://framedsc.com/GeneralGuides/using_rtti.htm), 
after which it's manually adjusted to fix some bugs and add some features (like reverse lookup range). The code is therefore not the best, but works fine. 