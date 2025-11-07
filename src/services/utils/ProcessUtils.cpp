#include "ProcessUtils.h"
#include <psapi.h>
#include <QSet>
#include <QString>
#include <QHash>

#pragma comment(lib, "Psapi.lib")

namespace ProcessUtils{

    bool terminateProcess(DWORD pid) {
        // 1. Get a handle to the process
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if(hProcess==NULL){
            return false;
        }

        // 2. Terminate the process
        BOOL result = TerminateProcess(hProcess, 1);

        // 3. Close the handle
        CloseHandle(hProcess);

        return (result != 0);
    }

    QHash<DWORD, QString> getActiveProcesses(){
        QHash<DWORD, QString> processMap;

        // --- Windows API Code to get processes ---
        DWORD aProcesses[1024], cbNeeded, cProcesses;

        // 1. Get a list of all process IDs (PIDs)
        if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)){
            return processMap; // Return empty set on error
        }

        // 2. Calculate how many PIDs were returned
        cProcesses = cbNeeded / sizeof(DWORD);

        // 3. Loop through each process
        for(unsigned int i = 0; i < cProcesses; i++){
            DWORD pid = aProcesses[i];
            if(pid != 0){ // Skip idle/system processes
                // 4. Get a "handle" (a reference) to the process
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                                FALSE, aProcesses[i]);

                if(hProcess != NULL){
                    // Use wide characters for Windows API
                    wchar_t szProcessName[MAX_PATH];

                    // 5. Get the process's executable name (e.g., "chrome.exe")
                    if(GetModuleBaseNameW(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(wchar_t))){
                        QString qName = QString::fromWCharArray(szProcessName).toLower();
                        // 6. Convert to QString and add to our set
                        processMap.insert(pid, qName);
                    }

                    // 7. Close the handle to free up resources
                    CloseHandle(hProcess);
                }
            }
        }
        return processMap;
    }

    void updateActiveProcessMap(QHash<DWORD, QString>& currentMap){
        // 1. Get a "snapshot" of all currently running PIDs
        DWORD aProcesses[1024], cbNeeded;
        if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)){
            return;
        }

        int cProcesses = cbNeeded / sizeof(DWORD);

        // 2. Put all current PIDs into a QSet for fast O(1) lookups
        QSet<DWORD> currentPIDsSet;
        for(int i = 0; i < cProcesses; ++i){
            if(aProcesses[i] != 0){
                currentPIDsSet.insert(aProcesses[i]);
            }
        }

        // 3. Check for CLOSED processes (in our map, but not in the snapshot)
        //    We must use an iterator to safely remove items from a QHash while looping
        auto it = currentMap.begin();
        while(it != currentMap.end()){
            if(!currentPIDsSet.contains(it.key())){
                // THis PID is in our map, but not running anymore.
                // Erase it from the map we were passed (m_activeProcessMap).
                it = currentMap.erase(it);
            } else{
                // This PID is still running, move to the next item.
                ++it;
            }
        }

        // 4. Check for NEW processes (in the snapshot, but not in our map)
        for (DWORD pid : currentPIDsSet){
            if(!currentMap.contains(pid)){
                // This is a new process. Get its name and add it to our map.
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                                FALSE, pid);
                if(hProcess != NULL){
                    wchar_t szProcessName[MAX_PATH];
                    if(GetModuleBaseNameW(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(wchar_t))){
                        QString qName = QString::fromWCharArray(szProcessName).toLower();
                        // Add the new process to the map we were passed (m_activeProcessMap).
                        currentMap.insert(pid, qName);
                    }
                    CloseHandle(hProcess);
                }
            }
        }
    }
} // namespace ProcessUtils