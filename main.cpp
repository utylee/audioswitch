#include "controller.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include "Windows.h"
#include "Shlobj.h"

struct AudioSwitchConfig {
    enum class Mode {
        CIRCLE, TOGGLE
    };

    Mode mode;
    std::string firstDevice;
    std::string secondDevice;
};

std::string getConfigPath();
void firstRun(const std::string &configPath);
void createDefaultConfig(const std::string &configPath);
void readConfig(std::ifstream &fs, AudioSwitchConfig &config);
std::string trim(std::string str, const std::string &delims = " \t\n\r\f\v");
void circleMode();
void toggleMode(const AudioSwitchConfig &config);

// http://stackoverflow.com/questions/8945018/hiding-the-black-window-in-c
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine, int nCmdShow) {

    std::string configPath = getConfigPath();
    std::ifstream fs(configPath);

    if(fs.is_open()) {
        AudioSwitchConfig config;
        readConfig(fs, config);

        switch(config.mode) {
        case AudioSwitchConfig::Mode::CIRCLE:
            circleMode();
            break;
        case AudioSwitchConfig::Mode::TOGGLE:
            toggleMode(config);
            break;
        }
    } else {
        firstRun(configPath);
    }

    return 0;
}

std::string getConfigPath() {
    wchar_t* appdataPath;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appdataPath);
    std::wstring appdata = appdataPath;
    appdata += L"\\AudioSwitch";
    CreateDirectoryW(appdata.c_str(), NULL);
    appdata += L"\\config.txt";

    char path[MAX_PATH];
    size_t charsConverted = 0;
    wcstombs_s(&charsConverted, path, sizeof path, appdata.c_str(), appdata.length());

    return path;
}

void firstRun(const std::string &configPath) {
    const char infoText[] =
        "This seems to be the first time you are running AudioSwitch.\n\n"
        "YES - Edit the config file.\n"
        "NO - Keep the standard configuration.\n"
        "CANCEL - Don't do anything and show this message again.\n";
    int result = MessageBoxA(NULL, infoText, "AudioSwitch - First Start",
                             MB_YESNOCANCEL | MB_ICONINFORMATION | MB_SYSTEMMODAL);

    if(result == IDYES || result == IDNO) {
        createDefaultConfig(configPath);
    }

    if(result == IDYES) {
        ShellExecuteA(0, 0, configPath.c_str(), 0, 0, SW_SHOW);
    }
}

void createDefaultConfig(const std::string &configPath) {
    std::vector<AudioDevice> list;
    AudioDevice defaultDev;
    AudioController::listDevices(list, defaultDev);

    std::string firstDevice = defaultDev.friendlyName;
    std::string secondDevice;

    for(const auto& device : list) {
        if(defaultDev.id != device.id) {
            secondDevice = device.friendlyName;
            break;
        }
    }

    std::ofstream fs(configPath);
    fs << "# This is the configuration file for AudioSwitch\n"
          "# You can find this file here: " << configPath << "\n"
          "#\n"
          "# Mode can be circle or toggle\n"
          "# - circle: Iterate over all activated devices.\n"
          "# - toggle: Switch between two devices defined as first_device and second_device.\n"
          "# For first_device and second_device the device name or a substring of the device name is allowed.\n"
          "\n"
          "mode = circle\n"
          "\n"
          "first_device = " << firstDevice << "\n"
          "second_device = " << secondDevice << "\n"
          "\n"
          "# This is a list of all audio devices that are currently enabled:\n";

    for(const auto& device : list) {
        fs << "# " << device.friendlyName << "\n";
    }

    fs.close();
}

void readConfig(std::ifstream &fs, AudioSwitchConfig &config) {
    std::string line;
    while (std::getline(fs, line)) {
        if(line[0] == '#') {
            // comment line
            continue;
        }

        size_t index = line.find_first_of("=");
        if(index == std::string::npos) {
            // probably empty or invalid line
            continue;
        }

        std::string key = trim(line.substr(0, index));
        std::string value = trim(line.substr(index + 1));

        if(key == "mode") {
            if(value == "circle") {
                config.mode = AudioSwitchConfig::Mode::CIRCLE;
            } else if(value == "toggle") {
                config.mode = AudioSwitchConfig::Mode::TOGGLE;
            } else {
                // TODO error
                config.mode = AudioSwitchConfig::Mode::CIRCLE;
            }
        } else if(key == "first_device") {
            config.firstDevice = value;
        } else if(key == "second_device") {
            config.secondDevice = value;
        } else {
            // TODO error
        }
    }
}

std::string trim(std::string str, const std::string& delims) {
    str.erase(0, str.find_first_not_of(delims));
    str.erase(str.find_last_not_of(delims) + 1);
    return str;
}

void circleMode() {
    std::vector<AudioDevice> list;
    AudioDevice defaultDev;
    AudioController::listDevices(list, defaultDev);

    if(list.size() <= 1) {
        return;
    }

    auto nextIt = list.begin();

    for(auto it = list.begin(); it != list.end(); ++it) {
        if(it->id == defaultDev.id) {
            nextIt = it;
            break;
        }
    }

    nextIt++;

    // if reached end, then start from beginning
    if(nextIt == list.end()) {
        nextIt = list.begin();
    }

    AudioController::setDefault(*nextIt);
}

void toggleMode(const AudioSwitchConfig &config) {
    std::vector<AudioDevice> list;
    AudioDevice defaultDev;
    AudioController::listDevices(list, defaultDev);

    std::string lookFor;
    if(defaultDev.friendlyName.find(config.firstDevice) != std::string::npos) {
        lookFor = config.secondDevice;
    } else {
        lookFor = config.firstDevice;
    }

    for(const auto& device : list) {
        if(device.friendlyName.find(lookFor) != std::string::npos) {
            AudioController::setDefault(device);
            break;
        }
    }
}
