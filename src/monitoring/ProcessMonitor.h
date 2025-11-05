#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QObject>
#include <QSet>
#include <QHash>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations
class GameList;
class QThread;
class QTimer;

class ProcessMonitor : public QObject
{
    Q_OBJECT

public:
    explicit ProcessMonitor(GameList* gameList, QObject *parent = nullptr);
    ~ProcessMonitor();

public slots:
    void startMonitor();
    void stopMonitor();

private slots:
    void runMonitorLoop();

signals:
    void gameDetected(DWORD pid, const QString& processName);
    void uncategorizedAppFound(const QString& processName);

private:
    GameList* m_gameList; // Pointer to the shared game list (not owned)
    QTimer* m_monitorTimer;
    QSet<DWORD> m_knownRunningPIDs;
    QHash<DWORD, QString> m_activeProcessMap;
    QSet<QString> m_flaggedUncategorizedApps;
    QSet<QString> m_trackedGameSessions; // To prevent duplicate gameDetected signals
};

#endif // PROCESSMONITOR_H