#include "controller.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include "Shlobj.h"

int main(int argc, char *argv[]) {
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

// EndPointController.exe [NewDefaultDeviceID]
int unused_main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            printf("Lists active audio end-point playback devices or sets default audio end-point\n");
            printf("playback device.\n\n");
            printf("USAGE\n");
            printf("  EndPointController.exe [-a] [-f format_str]  Lists audio end-point playback\n");
            printf("                                               devices that are enabled.\n");
            printf("  EndPointController.exe device_index          Sets the default playvack device\n");
            printf("                                               with the given index.\n");
            printf("\n");
            printf("OPTIONS\n");
            printf("  -a             Display all devices, rather than just active devices.\n");
            printf("  -f format_str  Outputs the details of each device using the given format\n");
            printf("                 string. If this parameter is ommitted the format string\n");
            printf("                 Parameters that are passed to the 'printf' function are\n");
            printf("                 ordered as follows:\n");
            printf("                   - Device index (int)\n");
            printf("                   - Device friendly name (wstring)\n");
            printf("                   - Device state (int)\n");
            printf("				     - Device default? (1 for true 0 for false as int)\n");
            printf("                   - Device description (wstring)\n");
            printf("                   - Device interface friendly name (wstring)\n");
            printf("                   - Device ID (wstring)\n");
            exit(0);
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            continue;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            if ((argc - i) >= 2) {
                continue;
            }
            else
            {
                printf("Missing format string");
                exit(1);
            }
        }
    }

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
