## AudioSwitch

[![Appveyor](https://ci.appveyor.com/api/projects/status/github/syxolk/audioswitch?svg=true)](https://ci.appveyor.com/project/syxolk/audioswitch)

Windows command-line tool that switches the default audio output device.
This work is heavily based on [AudioEndPointController by Dan Stevens](https://github.com/DanStevens/AudioEndPointController) with some
changes:
- Default behavior (without any command line flags) is to switch between audio
  devices that are defined in `%APPDATA%\AudioSwitch\devices.txt`
- Build system is now CMake

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
