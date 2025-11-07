#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QObject>
#include <QSet>
#include <QHash>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations
class QThread;
class QTimer;

class ProcessMonitor : public QObject
{
    Q_OBJECT

public:
    explicit ProcessMonitor(QObject *parent = nullptr);
    ~ProcessMonitor();

public slots:
    void startMonitor();
    void stopMonitor();

private slots:
    void runMonitorLoop();

signals:
    void processStarted(DWORD pid, const QString& processName);
    void processTerminated(DWORD pid);

private:
    
    QTimer* m_monitorTimer;
    QSet<DWORD> m_knownRunningPIDs;
    QHash<DWORD, QString> m_activeProcessMap;
};

#endif // PROCESSMONITOR_H