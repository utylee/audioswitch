// EndPointController.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <wchar.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include "windows.h"
#include "Mmdeviceapi.h"
#include "policy_config.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"
#include "Shlobj.h"

// Format default string for outputing a device entry. The following parameters will be used in the following order:
// Index, Device Friendly Name
#define DEVICE_OUTPUT_FORMAT "Audio Device %d: %ws"

typedef struct TGlobalState
{
    HRESULT hr;
    int option;
    IMMDeviceEnumerator *pEnum;
    IMMDeviceCollection *pDevices;
    LPWSTR strDefaultDeviceID;
    IMMDevice *pCurrentDevice;
    char* pDeviceFormatStr;
    int deviceStateFilter;
} TGlobalState;

void createDeviceEnumerator(TGlobalState* state);
void prepareDeviceEnumerator(TGlobalState* state);
void enumerateOutputDevices(TGlobalState* state);
HRESULT printDeviceInfo(IMMDevice* pDevice, int index, const char *outFormat, LPWSTR strDefaultDeviceID);
std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);
HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID);
void invalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file,
    unsigned int line, uintptr_t pReserved);

// EndPointController.exe [NewDefaultDeviceID]
int main(int argc, char *argv[])
{
    TGlobalState state;

    // Process command line arguments
    state.option = 0; // 0 indicates list devices.
    state.strDefaultDeviceID = '\0';
    state.pDeviceFormatStr = DEVICE_OUTPUT_FORMAT;
    state.deviceStateFilter = DEVICE_STATE_ACTIVE;

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
            printf("                 defaults to: \"%s\"\n\n", DEVICE_OUTPUT_FORMAT);
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
            state.deviceStateFilter = DEVICE_STATEMASK_ALL;
            continue;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            if ((argc - i) >= 2) {
                state.pDeviceFormatStr = argv[++i];

                // If printf is called with an invalid format string, jump to the invalidParameterHandler function.
                _set_invalid_parameter_handler(invalidParameterHandler);
                _CrtSetReportMode(_CRT_ASSERT, 0);
                continue;
            }
            else
            {
                printf("Missing format string");
                exit(1);
            }
        }
    }

    if (argc == 2) state.option = atoi(argv[1]);

    state.hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(state.hr))
    {
        createDeviceEnumerator(&state);
    }
    return state.hr;
}

// Create a multimedia device enumerator.
void createDeviceEnumerator(TGlobalState* state)
{
    state->pEnum = NULL;
    state->hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
        (void**)&state->pEnum);
    if (SUCCEEDED(state->hr))
    {
        prepareDeviceEnumerator(state);
    }
}

// Prepare the device enumerator
void prepareDeviceEnumerator(TGlobalState* state)
{
    state->hr = state->pEnum->EnumAudioEndpoints(eRender, state->deviceStateFilter, &state->pDevices);
    if SUCCEEDED(state->hr)
    {
        enumerateOutputDevices(state);
    }
    state->pEnum->Release();
}

// Enumerate the output devices
void enumerateOutputDevices(TGlobalState* state)
{
    UINT count;
    state->pDevices->GetCount(&count);

    // If option is less than 1, list devices
    if (state->option < 1)
    {

        // Get default device
        IMMDevice* pDefaultDevice;
        state->hr = state->pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
        if (SUCCEEDED(state->hr))
        {

            state->hr = pDefaultDevice->GetId(&state->strDefaultDeviceID);

            // Iterate all devices
            for (int i = 1; i <= (int)count; i++)
            {
                state->hr = state->pDevices->Item(i - 1, &state->pCurrentDevice);
                if (SUCCEEDED(state->hr))
                {
                    state->hr = printDeviceInfo(state->pCurrentDevice, i, state->pDeviceFormatStr,
                        state->strDefaultDeviceID);
                    state->pCurrentDevice->Release();
                }
            }
        }
    }
    // If option corresponds with the index of an audio device, set it to default
    else if (state->option <= (int)count)
    {
        state->hr = state->pDevices->Item(state->option - 1, &state->pCurrentDevice);
        if (SUCCEEDED(state->hr))
        {
            LPWSTR strID = NULL;
            state->hr = state->pCurrentDevice->GetId(&strID);
            if (SUCCEEDED(state->hr))
            {
                state->hr = SetDefaultAudioPlaybackDevice(strID);
            }
            state->pCurrentDevice->Release();
        }
    }
    // Otherwise inform user than option doesn't correspond with a device
    else
    {
        printf("Error: No audio end-point device with the index '%d'.\n", state->option);
    }

    state->pDevices->Release();
}

HRESULT printDeviceInfo(IMMDevice* pDevice, int index, const char* outFormat, LPWSTR strDefaultDeviceID)
{
    // Device ID
    LPWSTR strID = NULL;
    HRESULT hr = pDevice->GetId(&strID);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    int deviceDefault = (strDefaultDeviceID != '\0' && (wcscmp(strDefaultDeviceID, strID) == 0));

    // Device state
    DWORD dwState;
    hr = pDevice->GetState(&dwState);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    IPropertyStore *pStore;
    hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
    if (SUCCEEDED(hr))
    {
        std::wstring friendlyName = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
        std::wstring Desc = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
        std::wstring interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);

        if (SUCCEEDED(hr))
        {
            printf(outFormat,
                index,
                friendlyName.c_str(),
                dwState,
                deviceDefault,
                Desc.c_str(),
                interfaceFriendlyName.c_str(),
                strID
            );
            printf("\n");
        }

        pStore->Release();
    }
    return hr;
}

std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
    PROPVARIANT prop;
    PropVariantInit(&prop);
    HRESULT hr = pStore->GetValue(key, &prop);
    if (SUCCEEDED(hr))
    {
        std::wstring result (prop.pwszVal);
        PropVariantClear(&prop);
        return result;
    }
    else
    {
        return std::wstring();
    }
}

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
    IPolicyConfigVista *pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
        NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
    if (SUCCEEDED(hr))
    {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
        pPolicyConfig->Release();
    }
    return hr;
}

void invalidParameterHandler(const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved)
{
   printf("\nError: Invalid format_str.\n");
   exit(1);
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
