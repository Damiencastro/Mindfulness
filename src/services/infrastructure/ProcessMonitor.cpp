#include "ProcessMonitor.h"
#include "ApplicationRepository.h"
#include "../utils/ProcessUtils.h"

#include <QTimer>
#include <QHash>
#include <QDebug>

#include <windows.h>
#include <psapi.h>
#include <vector>

ProcessMonitor::ProcessMonitor(QObject *parent)
    : QObject(parent),
      m_monitorTimer(nullptr)
{
    // 1. Create the timer that will drive the monitor loop
    m_monitorTimer = new QTimer(this);

    // 2. Connect the timer's timeout to our main loop function
    connect(m_monitorTimer, &QTimer::timeout, this, &ProcessMonitor::runMonitorLoop);

    // Note: We don't start the timer here. AppController will call
    // startMonitor() after moving this object to the background thread
}

ProcessMonitor::~ProcessMonitor()
{
    if(m_monitorTimer){
        m_monitorTimer->stop();
    }
}

void ProcessMonitor::startMonitor()
{
    // Start the timer. It will now emit timeout() every 2000ms.
    m_monitorTimer->start(2000); // 2-second interval
}

void ProcessMonitor::stopMonitor()
{
    m_monitorTimer->stop();
}

void ProcessMonitor::runMonitorLoop()
{
    // 1. Update our persistent map (m_activeProcessMap) in-place.
    //    This is the fast, pass-by-reference call.
    ProcessUtils::updateActiveProcessMap(m_activeProcessMap);

    // 2. Check for closed applications.
    //    This loop checks our "processed" list (m_knownRunningPIDs)
    //    against the "live" list (m_activeProcessMap).
    auto it = m_knownRunningPIDs.begin();
    while (it != m_knownRunningPIDs.end()){
        DWORD knownPID = *it;
        if(!m_activeProcessMap.contains(knownPID)){
            // This process *was* known, but is now closed.
            // Remove it from teh "known" list so we can detect it again if it relaunches
            it = m_knownRunningPIDs.erase(it);
            emit processTerminated(knownPID);
        } else{
            ++it;
        }
    }

    // 3. Check for new applications
    //    This loop checks the "live" list (m_activeProcessMap)
    //    against our "processed" list (m_knownRunningPIDs).

    //    We use the C++11 compatible iterator method.
    auto map_it = m_activeProcessMap.constBegin();
    auto map_end = m_activeProcessMap.constEnd();

    while (map_it != map_end){
        DWORD pid = map_it.key();
        const QString& appName = map_it.value();

        // Check if we've already seen and processed this PID
        if(m_knownRunningPIDs.contains(pid)){
            ++map_it; // Move to the next item
            continue; // Not a new process, skip it.
        }

        // --- This is a NEW process ---
        // (It's in m_activeProcessMap but not in m_knownRunningPIDs)

        // 1. Add it to our "processed" list so we don't spam signals
        m_knownRunningPIDs.insert(pid);

        // 2. Emit processStarted
        emit processStarted(pid, appName);
        
        ++map_it; // Move to the next item.
    }
}