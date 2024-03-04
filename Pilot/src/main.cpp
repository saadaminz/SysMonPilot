#include <iostream>

#include "Helpers/WmiWrapper.h"
#include "Monitors/SystemInfo.h"

int main() {
    SystemInfo si;
    WmiWrapper wmi;
    if (wmi.Initialize()) {
        wmi.GetSystemInfo(si);
        si.PrintInfo();
    }
    return 0;
}