// ddc_ic.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <Windows.h>
#include <WinUser.h>
#include <physicalmonitorenumerationapi.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <highlevelmonitorconfigurationapi.h>

#pragma comment(lib, "Dxva2.lib")

using namespace std;

int main() {
    HWND currentWindow = GetConsoleWindow();
    if (currentWindow == NULL) {
        std::cout << "GetConsoleWindow ERROR" << endl;
        return 1;
    }

    HMONITOR monitor = MonitorFromWindow(currentWindow, MONITOR_DEFAULTTONULL);
    if (monitor == NULL) {
        std::cout << "MonitorFromWindow" << std::endl;
        return 1;
    }

    DWORD size;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(monitor, &size)) {
        cerr << "GetNumberOfPhysicalMonitorsFromHMONITOR ERROR\n";
        return 1;
    }

    LPPHYSICAL_MONITOR lppstruct = (LPPHYSICAL_MONITOR)malloc(size * sizeof(PHYSICAL_MONITOR));
    if (lppstruct == NULL) {
        cerr << "Failed to alloc.\n";
        return 1;
    }

    if (!GetPhysicalMonitorsFromHMONITOR(monitor, size, lppstruct)) {
        cerr << "GetPhysicalMonitorsFromHMONITOR ERROR\n";
        return 1;
    }

    wprintf(L"%s\n", lppstruct[0].szPhysicalMonitorDescription);

    DWORD min, cur, max;
    if (!GetMonitorBrightness(lppstruct[0].hPhysicalMonitor, &min, &cur, &max)) {
        cerr << "GetMonitorBrightness ERROR\n";
        return 1;
    }

    printf("pdwMinimumBrightness: %lu\npdwCurrentBrightness: %lu\npdwMaximumBrightness: %lu\n", min, cur, max);

    DWORD strLength;
    if (!GetCapabilitiesStringLength(lppstruct[0].hPhysicalMonitor, &strLength)) {
        cerr << "GetCapabilitiesStringLength ERROR\n";
        return 1;
    }

    LPSTR capStr = (LPSTR)malloc(strLength);
    if (capStr == NULL) {
        cerr << "malloc error.\n";
        return 1;
    }

    if (!CapabilitiesRequestAndCapabilitiesReply(lppstruct[0].hPhysicalMonitor, capStr, strLength)) {
        cerr << "CapabilitiesRequestAndCapabilitiesReply ERROR\n";
        return 1;
    }

    printf("%s\n", capStr);

    if (!SetVCPFeature(lppstruct[0].hPhysicalMonitor, 0x15, 0x45)) {
        cerr << "SetVCPFeature ERROR\n";
        return 1;
    }

    if (!DestroyPhysicalMonitors(size, lppstruct)) {
        cerr << "DestroyPhysicalMonitors ERROR\n";
        return 1;
    }

    free(lppstruct);
    free(capStr);
}

/*
(
    prot(monitor)
    type(lcd)
    GL850cmds(01 02 03 0C E3 F3)
    vcp(
        14(05 08 0B)
        60(11 12 0F 10)
        D6(01 04)
        F5(00 01 02)
        F6(00 01 02)
        15(01 06 09 10 11 13 14 28 29 32 44 48)
        F7(00 01 02 03)
        F8(00 01)
        F9 EF FD(00 01)
        FE(00 01 02)

        02 04 05 08 10 12
        16 18 1A 52
        AC AE B2 B6 C0 C6 C8 C9
        DF 62 8D F4
        4D 4E 4F
        FF
    )
    mccs_ver(2.1)
    mswhql(1)
)
*/