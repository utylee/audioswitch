#pragma once

#include <string>
#include <vector>

struct AudioDevice {
    std::string friendlyName;
    unsigned long state;
    std::string description;
    std::string interfaceFriendlyName;
    std::wstring id;
};

class AudioController {
public:
    static bool listDevices(std::vector<AudioDevice> &list, AudioDevice &defaultDevice, bool listAll = false);
    static bool setDefault(const AudioDevice &device);
};
