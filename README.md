## AudioSwitch

[![Appveyor](https://ci.appveyor.com/api/projects/status/github/syxolk/audioswitch?svg=true)](https://ci.appveyor.com/project/syxolk/audioswitch)

Windows command-line tool that switches the default audio output device.
This work is heavily based on [AudioEndPointController by Dan Stevens](https://github.com/DanStevens/AudioEndPointController) with some
changes:
- Default behavior (without any command line flags) is to do
  whatever is configured in `%LOCALAPPDATA%\AudioSwitch\config.txt`
- On first start the user is prompted to edit the config file
- Build system is now CMake
- No need for .NET or Visual C++ Redistributable

What this program does NOT do:
- Display fancy GUI
- Run in background
- Register hotkeys

If you want any of these, you should better use
[AudioSwitcher_v1 by xenolightning](https://github.com/xenolightning/AudioSwitcher_v1) or
[AudioSwitch by sirWest](https://github.com/sirWest/AudioSwitch)

**The main reason you want to use this: If you got ROCCAT hardware you
can only select EXE files as Quicklaunch targets for Macro keys in the driver.
Command-line arguments and Batch scripts are not allowed. However, this program
has no dependencies on ROCCAT drivers, use it as you like.**

### Requirements
- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/download/)
- [Visual Studio](https://www.visualstudio.com/de-de/products/visual-studio-express-vs)
  OR [Visual C++ Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools)
- [WiX Toolset](http://wixtoolset.org/releases/) to build MSI files

### Compile
```
mkdir build
cd build
cmake -G "Visual Studio 14 2015" ..
cmake --build . --config Release
```

### Create Installer
Generate MSI file while in `build` directory:
```
cpack
```

### Credits
Icons made by Iconshock from www.iconarchive.com are free for non-commercial use:
[Music Icon](http://www.iconarchive.com/show/super-vista-general-icons-by-iconshock/music-icon.html)
