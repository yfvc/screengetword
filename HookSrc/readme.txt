Here you can find source codes. Note: to compile NtProcDrv you need NTDDK to be installed on your computer.

To run compiled sample, place tunew20.dll in any folder that is specified in PATH variable (windows/system32 for example). Then run HookDemo.exe. After it is has been started all process creation routines will be intercepted.  Press "Enter" key and hook will be uninstalled. Note: you should have an administrator rights for properly running application. 

To verify if it works fine try to start any application after HookDemo has been run. You should see message box "Before starting process." on process startup and "Before ending." right before process will be ended.

Andriy Oriekhov
www.toleron.com