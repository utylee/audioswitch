#include "controller.h"

#include "windows.h"
#include "Mmdeviceapi.h"
#include "policy_config.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"

static bool buildAudioDevice(AudioDevice &device, IMMDevice* imm);
static std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);

bool AudioController::listDevices(std::vector<AudioDevice> &list, AudioDevice &defaultDevice, bool listAll) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if(FAILED(hr)) {
        return false;
    }

    // create device enumerator
    IMMDeviceEnumerator *pEnum;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
            (void**)&pEnum);
    if(FAILED(hr)) {
        return false;
    }

    // get device collection
    IMMDeviceCollection *pDevices;
    hr = pEnum->EnumAudioEndpoints(eRender, listAll ? DEVICE_STATEMASK_ALL : DEVICE_STATE_ACTIVE, &pDevices);
    if(FAILED(hr)) {
        return false;
    }

    // get number of devices
    unsigned int numOfDevices;
    pDevices->GetCount(&numOfDevices);
    if(FAILED(hr)) {
        return false;
    }

    // get default device
    IMMDevice* pDefaultDevice;
    hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
    if(FAILED(hr)) {
        return false;
    }
    buildAudioDevice(defaultDevice, pDefaultDevice);

    //iterate
    for(unsigned int i = 0; i < numOfDevices; i++) {
        IMMDevice *imm;
        hr = pDevices->Item(i, &imm);
        if(SUCCEEDED(hr)) {
            AudioDevice device;
            if(buildAudioDevice(device, imm)) {
                list.push_back(device);
            }
            imm->Release();
        }
    }

    // clean up
    pDevices->Release();
    pEnum->Release();

    return true;
}

bool AudioController::setDefault(const AudioDevice &device) {
    IPolicyConfigVista *pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
        NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), reinterpret_cast<LPVOID *>(&pPolicyConfig));
    if (SUCCEEDED(hr)) {
        hr = pPolicyConfig->SetDefaultEndpoint(device.id.c_str(), reserved);
        pPolicyConfig->Release();
    }
    return SUCCEEDED(hr);
}

// INTERNAL FUNCTIONS

bool buildAudioDevice(AudioDevice &device, IMMDevice* imm) {
    // get ID
    wchar_t* strID = NULL;
    HRESULT hr = imm->GetId(&strID);
    if(FAILED(hr)) {
        return false;
    }
    device.id = strID;

    // get state
    hr = imm->GetState(&device.state);
    if(FAILED(hr)) {
        return false;
    }

    // get properties
    IPropertyStore *pStore;
    hr = imm->OpenPropertyStore(STGM_READ, &pStore);
    if(FAILED(hr)) {
        return false;
    }

    device.friendlyName = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
    device.description = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
    device.interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);

    pStore->Release();

    return true;
}

std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
    PROPVARIANT prop;
    PropVariantInit(&prop);
    HRESULT hr = pStore->GetValue(key, &prop);
    if (SUCCEEDED(hr)) {
        std::wstring result(prop.pwszVal);
        PropVariantClear(&prop);
        return result;
    } else {
        return std::wstring();
    }
}
