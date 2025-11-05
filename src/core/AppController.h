#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations to reduce header includes
class GameList;
class ProcessMonitor;
class GameSessionManager;
class ConfigWindow;
class QSystemTrayIcon;

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController();

public slots:
    void onShowConfig();
    void onUncategorizedAppFound(const QString& processName);
    void onQuit();

private:
    // Owned Components
    GameList* m_gameList;
    ProcessMonitor* m_processMonitor;
    GameSessionManager* m_sessionManager;
    ConfigWindow* m_configWindow;
    QSystemTrayIcon* m_trayIcon;
};

#endif // APPCONTROLLER_H