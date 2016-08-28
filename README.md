## AudioSwitch

Windows command-line tool that switches the default audio output device.
This work is heavily based on [AudioEndPointController by Dan Stevens](https://github.com/DanStevens/AudioEndPointController) with some
changes:
- Default behavior (without any command line flags) is to switch between audio
  devices that are defined in `%APPDATA%\AudioSwitch\devices.txt`
- Build system is now CMake

### Requirements
- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/download/)
- [Visual Studio](https://www.visualstudio.com/de-de/products/visual-studio-express-vs)
  OR [Visual C++ Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools)

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
