#pragma once

#include <string>
#include <vector>

struct AudioDevice {
    std::wstring friendlyName;
    unsigned long state;
    std::wstring description;
    std::wstring interfaceFriendlyName;
    std::wstring id;
};

class AudioController {
public:
    static bool listDevices(std::vector<AudioDevice> &list, AudioDevice &defaultDevice, bool listAll = false);
    static bool setDefault(const AudioDevice &device);
};
