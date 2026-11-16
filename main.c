#include <windows.h>
#include <pdh.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <psapi.h>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "psapi.lib")

typedef struct {
    char name[MAX_PATH];
    DWORD pid;
    double memory_mb;
} ProcessInfo;

int compare_processes(const void* a, const void* b) {
    ProcessInfo* p1 = (ProcessInfo*)a;
    ProcessInfo* p2 = (ProcessInfo*)b;
    if (p2->memory_mb > p1->memory_mb) return 1;
    if (p2->memory_mb < p1->memory_mb) return -1;
    return 0;
}

double get_memory_usage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return (double)memInfo.dwMemoryLoad;
}

void get_disk_usage(double* total_gb, double* used_gb, double* free_gb) {
    ULARGE_INTEGER freeBytesToCaller, totalBytes, freeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesToCaller, &totalBytes, &freeBytes)) {
        *total_gb = (double)totalBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
        *free_gb = (double)freeBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
        *used_gb = *total_gb - *free_gb;
    } else {
        *total_gb = 0; *used_gb = 0; *free_gb = 0;
    }
}

void get_top_processes(ProcessInfo* plist, int* count) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return;
    cProcesses = cbNeeded / sizeof(DWORD);
    int valid_count = 0;
    for (unsigned int i = 0; i < cProcesses; i++) {
        DWORD pid = aProcesses[i];
        if (pid == 0) continue;
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess != NULL) {
            HMODULE hMod;
            DWORD cbNeededMod;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeededMod)) {
                GetModuleBaseNameA(hProcess, hMod, plist[valid_count].name, sizeof(plist[valid_count].name));
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    plist[valid_count].pid = pid;
                    plist[valid_count].memory_mb = (double)pmc.WorkingSetSize / (1024.0 * 1024.0);
                    valid_count++;
                    if (valid_count >= 50) {
                        CloseHandle(hProcess);
                        break;
                    }
                }
            }
            CloseHandle(hProcess);
        }
    }
    qsort(plist, valid_count, sizeof(ProcessInfo), compare_processes);
    *count = valid_count < 3 ? valid_count : 3;
}

int main() {
    SetConsoleOutputCP(65001);

    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PDH_HQUERY netQuery;
    PDH_HCOUNTER netRecv;

    if (PdhOpenQuery(NULL, 0, &cpuQuery) != ERROR_SUCCESS) return 1;
    if (PdhAddEnglishCounterA(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuTotal) != ERROR_SUCCESS) return 1;

    if (PdhOpenQuery(NULL, 0, &netQuery) != ERROR_SUCCESS) return 1;
    if (PdhAddEnglishCounterA(netQuery, "\\Network Interface(*)\\Bytes Received/sec", 0, &netRecv) != ERROR_SUCCESS) {
    }

    PdhCollectQueryData(cpuQuery);
    PdhCollectQueryData(netQuery);
    Sleep(1000);

    while (true) {
        PdhCollectQueryData(cpuQuery);
        PdhCollectQueryData(netQuery);

        PDH_FMT_COUNTERVALUE cpuVal, netVal;
        double cpu_usage = 0.0;
        double net_speed = 0.0;

        if (PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &cpuVal) == ERROR_SUCCESS) {
            cpu_usage = cpuVal.doubleValue;
        }

        if (PdhGetFormattedCounterValue(netRecv, PDH_FMT_DOUBLE, NULL, &netVal) == ERROR_SUCCESS) {
            net_speed = netVal.doubleValue / 1024.0;
        }

        double ram_usage = get_memory_usage();
        double d_total = 0, d_used = 0, d_free = 0;
        get_disk_usage(&d_total, &d_used, &d_free);

        ProcessInfo plist[50];
        int p_count = 0;
        get_top_processes(plist, &p_count);

        printf("{\"cpu\": %.2f, \"ram\": %.2f, \"disk_total\": %.2f, \"disk_used\": %.2f, \"net_recv\": %.2f, \"procs\": [", 
            cpu_usage, ram_usage, d_total, d_used, net_speed);

        for (int i = 0; i < p_count; i++) {
            printf("{\"name\": \"%s\", \"pid\": %lu, \"mem\": %.1f}%s", 
                plist[i].name, plist[i].pid, plist[i].memory_mb, (i == p_count - 1) ? "" : ",");
        }
        printf("]}\n");
        fflush(stdout);

        Sleep(1000);
    }

    PdhCloseQuery(cpuQuery);
    PdhCloseQuery(netQuery);
    return 0;
}