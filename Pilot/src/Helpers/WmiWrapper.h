#ifndef WMIWRAPPER_H
#define WMIWRAPPER_H

#include <comdef.h>
#include <Wbemidl.h>

#include "../Monitors/SystemInfo.h"

#pragma comment(lib, "wbemuuid.lib")

class WmiWrapper {
public:
    WmiWrapper();
    ~WmiWrapper();

    bool Initialize();
    void GetSystemInfo(SystemInfo& si);

private:
    IWbemLocator* pLocator;
    IWbemServices* pServices;
    bool initialized;
};

#endif // WMIWRAPPER_H
