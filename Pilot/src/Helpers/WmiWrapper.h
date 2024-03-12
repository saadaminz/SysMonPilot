#ifndef WMIWRAPPER_H
#define WMIWRAPPER_H

#include <comdef.h>
#include <Wbemidl.h>

#include "../Monitors/SystemInfo.h"
#include "../Monitors/ProcessInfo.h"

#pragma comment(lib, "wbemuuid.lib")

class WmiWrapper {
public:
    WmiWrapper();
    ~WmiWrapper();

    bool InitializeWMI();
    void GetSystemInfo(SystemInfo& si);
    void GetSystemCounters();
    void GetProcessInfo(ProcessInfo::Vector& piVectorList);

private:
    IWbemLocator* pLocator;
    IWbemServices* pServices;
    bool initialized;
};

#endif // WMIWRAPPER_H
