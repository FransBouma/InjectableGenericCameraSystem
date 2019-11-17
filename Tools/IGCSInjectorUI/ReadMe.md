IGCSInjector UI
============================
Generic injector for camera dlls into game exes, using a Windows UI

This is a small tool to inject dlls (e.g. the IGCS based camera tools, but others as well) into running processes, e.g. games. While ExtremeInjector
or other injectors might work OK, chances are Windows Defender will simply block their usage, especially on Windows 10. Not with IGCSInjectorUI, as it
apparently doesn't match Defender's checks so it's left alone. 

It allows you to pick the running process to inject a dll into and then select the dll to inject. It keeps track of which process-dll combinations you've used
so next time you choose the same process, it will auto-select the same dll. 

The injection process is the same as the command line version: it simply allocates some memory inside the target exe's host process 
and loads the dll into that memory, and then starts the dll as if the host process has loaded the dll itself. Nothing nasty, it's actually designed windows
functionality (e.g. debuggers use this too). 

Release binaries: https://github.com/FransBouma/InjectableGenericCameraSystem/releases/tag/IGCSInjectorUI_102  
Credits: Otis_Inf

### How to use
Start the IGCSInjectorUI, click on `Select` to select the process to inject to and the `Browse` button to select the DLL to inject.

If you want to pre-configure the IGCSInjectorUI, you can adjust the IGCSInjectorUI.exe.config file and change the `defaultDllName` and `defaultProcessName`
appConfig elements. You don't need to specify both, you can also e.g. specify only the `defaultDllName`. 

IGCSInjectorUI filters out any process that doesn't have a Window with a title. This makes it likely you'll pick the right executable, even if the game e.g. spawns
several processes during startup. 

### EULA

Re-hosting binary builds of IGCSInjectorUI is prohibited unless you have been given permission to do so. 