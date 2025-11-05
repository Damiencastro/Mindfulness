#ifndef PROCESSUTILS_H
#define PROCESSUTILS_H

#include <windows.h> // For DWORD
#include <QSet>
#include <QString>
#include <QHash>

namespace ProcessUtils
{
    bool terminateProcess(DWORD pid);
    
    QHash<DWORD, QString> getActiveProcesses();

    void updateActiveProcessMap(QHash<DWORD, QString>& currentMap);
}

#endif // PROCESSUTILS_H