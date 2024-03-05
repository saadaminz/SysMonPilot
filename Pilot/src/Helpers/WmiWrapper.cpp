#include <Poco/Logger.h>

#include "WmiWrapper.h"
#include "../Monitors/SystemInfo.h"

WmiWrapper::WmiWrapper() : pLocator(nullptr), pServices(nullptr), initialized(false) {}

WmiWrapper::~WmiWrapper() {
    if (pServices) {
        pServices->Release();
    }
    if (pLocator) {
        pLocator->Release();
    }
    CoUninitialize();
}

bool WmiWrapper::Initialize() {
    Poco::Logger& logger = Poco::Logger::get("WmiWrapper");
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        logger.error("Failed to initialize COM library. Error code = 0x%lx", hr);
        return false;
    }

    hr = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr
    );

    if (FAILED(hr)) {
        logger.error("Failed to initialize security. Error code = 0x%lx", hr);
        CoUninitialize();
        return false;
    }

    hr = CoCreateInstance(
        CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLocator);

    if (FAILED(hr)) {
        logger.error("Failed to create IWbemLocator object. Error code = 0x%lx", hr);
        CoUninitialize();
        return false;
    }

    hr = pLocator->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        nullptr,
        nullptr,
        0,
        NULL,
        0,
        0,
        &pServices);

    if (FAILED(hr)) {
        logger.error("Could not connect to WMI namespace. Error code = 0x%lx", hr);
        pLocator->Release();
        CoUninitialize();
        return false;
    }

    logger.information("Initialized WMI connection successfully.");
    initialized = true;
    return true;
}

void WmiWrapper::GetSystemInfo(SystemInfo& si) {
    Poco::Logger& logger = Poco::Logger::get("WmiWrapper");
    
    if (!initialized) {
        logger.error("WMI Helper is not initialized.");
        return;
    }

    /******************************************************************************************************************************************************
        Win32_Processor - NumberOfCores | NumberOfLogicalProcessors | Name | MaxClockSpeed | Architecture
    *******************************************************************************************************************************************************/

    IEnumWbemClassObject* pEnumerator = nullptr;
    HRESULT hrCPU = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT NumberOfCores, NumberOfLogicalProcessors, Name, MaxClockSpeed, Architecture FROM Win32_Processor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator);

    if (FAILED(hrCPU)) {
        logger.error("Query for processor information failed. Error code = 0x%lx", hrCPU);
    }
    else {
        IWbemClassObject* pclsObjCPU = nullptr;
        ULONG uReturnCPU = 0;

        while (pEnumerator) {
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObjCPU, &uReturnCPU);

            if (0 == uReturnCPU) {
                break;
            }

            VARIANT vtProp;

            // Todo: Handling multiple processors.

            hr = pclsObjCPU->Get(L"NumberOfCores", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.cpu_cores += vtProp.intVal;
                VariantClear(&vtProp);
            }

            hr = pclsObjCPU->Get(L"NumberOfLogicalProcessors", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.cpu_threads += vtProp.intVal;
                VariantClear(&vtProp);
            }

            hr = pclsObjCPU->Get(L"Name", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                if (si.cpu_name.empty()) {
                    si.cpu_name = _bstr_t(vtProp.bstrVal);
                }
                VariantClear(&vtProp);
            }

            hr = pclsObjCPU->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.cpu_max_clock = vtProp.uintVal;
                VariantClear(&vtProp);
            }

            unsigned int arch = 0;
            hr = pclsObjCPU->Get(L"Architecture", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                arch = vtProp.uintVal;
                VariantClear(&vtProp);
                switch (arch) {
                case 0: si.cpu_architecture = "x86"; break;
                case 1: si.cpu_architecture = "MIPS"; break;
                case 2: si.cpu_architecture = "Alpha"; break;
                case 3: si.cpu_architecture = "PowerPC"; break;
                case 6: si.cpu_architecture = "ia64"; break;
                case 9: si.cpu_architecture = "x64"; break;
                default: si.cpu_architecture = "Unkown"; break;
                }
            }

            pclsObjCPU->Release();
        }

        pEnumerator->Release();
    }

    /******************************************************************************************************************************************************
        Win32_OperatingSystem - Caption | OSArchitecture | Version | ProductType
    *******************************************************************************************************************************************************/

    IEnumWbemClassObject* pEnumeratorOS = nullptr;
    HRESULT hrOS = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT Caption, OSArchitecture, Version, ProductType FROM Win32_OperatingSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumeratorOS);

    if (FAILED(hrOS)) {
        logger.error("Query for operating system information failed. Error code = 0x%lx", hrOS);
    }
    else {
        IWbemClassObject* pclsObjOS = nullptr;
        ULONG uReturnOS = 0;

        while (pEnumeratorOS) {
            HRESULT hr = pEnumeratorOS->Next(WBEM_INFINITE, 1, &pclsObjOS, &uReturnOS);

            if (0 == uReturnOS) {
                break;
            }

            VARIANT vtProp;

            hr = pclsObjOS->Get(L"Caption", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.os_name = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjOS->Get(L"OSArchitecture", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.os_architecture = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjOS->Get(L"Version", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.os_version = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }
            unsigned int pType = 0;
            hr = pclsObjOS->Get(L"ProductType", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                pType = vtProp.uintVal;
                VariantClear(&vtProp);
                switch (pType) {
                case 1: si.os_type = "Work Station"; break;
                case 2: si.os_type = "Domain Controller"; break;
                case 3: si.os_type = "Server"; break;
                default: si.os_type = "Unkown"; break;
                }
            }

            pclsObjOS->Release();
        }

        pEnumeratorOS->Release();
    }

    /******************************************************************************************************************************************************
        Win32_PhysicalMemory - Capacity
    *******************************************************************************************************************************************************/

    IEnumWbemClassObject* pEnumeratorMemory = nullptr;
    HRESULT hrMemory = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT Capacity FROM Win32_PhysicalMemory"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumeratorMemory);

    if (FAILED(hrMemory)) {
        logger.error("Query for physical memory information failed. Error code = 0x%lx", hrMemory);
    }
    else {
        IWbemClassObject* pclsObjMemory = nullptr;
        ULONG uReturnMemory = 0;
        UINT64 memoryBytes = 0;

        while (pEnumeratorMemory) {
            HRESULT hr = pEnumeratorMemory->Next(WBEM_INFINITE, 1, &pclsObjMemory, &uReturnMemory);

            if (0 == uReturnMemory) {
                break;
            }

            VARIANT vtProp;

            hr = pclsObjMemory->Get(L"Capacity", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                memoryBytes += _wcstoui64(vtProp.bstrVal, NULL, 10);
                VariantClear(&vtProp);
                si.memory_mb = (memoryBytes / (1024 * 1024));
            }

            pclsObjMemory->Release();
        }

        pEnumeratorMemory->Release();
    }

    /******************************************************************************************************************************************************
        Win32_ComputerSystem - Caption | DNSHostName | Domain | Manufacturer | Model | PrimaryOwnerName | SystemFamily
    *******************************************************************************************************************************************************/

    IEnumWbemClassObject* pEnumeratorCS = nullptr;
    HRESULT hrCS = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT Caption, DNSHostName, Domain, Manufacturer, Model, PrimaryOwnerName, SystemFamily FROM Win32_ComputerSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumeratorCS);

    if (FAILED(hrCS)) {
        logger.error("Query for computer system failed. Error code = 0x%lx", hrCS);
    }
    else {
        IWbemClassObject* pclsObjeCS = nullptr;
        ULONG uReturnCS = 0;

        while (pEnumeratorCS) {
            HRESULT hr = pEnumeratorCS->Next(WBEM_INFINITE, 1, &pclsObjeCS, &uReturnCS);

            if (0 == uReturnCS) {
                break;
            }

            VARIANT vtProp;

            hr = pclsObjeCS->Get(L"Caption", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_user = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjeCS->Get(L"DNSHostName", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_hostname = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjeCS->Get(L"Domain", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_domain = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjeCS->Get(L"Manufacturer", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_manufacturer = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjeCS->Get(L"Model", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_model = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjeCS->Get(L"PrimaryOwnerName", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_owner = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            hr = pclsObjeCS->Get(L"SystemFamily", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                si.sys_family = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            pclsObjeCS->Release();
        }
        pEnumeratorCS->Release();
    }
}