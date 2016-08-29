#include "controller.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include "Windows.h"
#include "Shlobj.h"

// http://stackoverflow.com/questions/8945018/hiding-the-black-window-in-c
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine, int nCmdShow) {

    std::vector<AudioDevice> list;
    AudioDevice defaultDev;

    AudioController::listDevices(list, defaultDev);

    if(list.size() <= 1) {
        return 0;
    }

    int defaultIndex = 0;

    for(unsigned int i = 0; i  < list.size(); i++) {
        if(list[i].id == defaultDev.id) {
            defaultIndex = i;
        }
    }

    int nextIndex = (defaultIndex + 1) % list.size();
    AudioController::setDefault(list[nextIndex]);

    return 0;
}

void create_config() {
    wchar_t* appdataPath;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appdataPath);
    printf("%ws\n", appdataPath);
    std::wstring appdata = appdataPath;
    appdata += L"\\AudioSwitch";
    CreateDirectoryW(appdata.c_str(), NULL);
    appdata += L"\\devices.txt";
    CreateFileW(appdata.c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    ShellExecuteW(0, 0, appdata.c_str(), 0, 0, SW_SHOW);
}
