#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations to reduce header includes
// Repositories
class ApplicationRepository;

// Services
class ProcessMonitor;           // Infrastructure
class ProcessEventDispatcher;   // Application

// Managers
class GameSessionManager;       // Game Session Manager
class CategorizationManager;

// UI
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
    void onQuit();

private:
    // Owned Components
    
    // Repositories
    ApplicationRepository* m_appRepository;

    // Managers
    GameSessionManager* m_sessionManager;
    CategorizationManager* m_categorizationManager;

    // Services
    ProcessMonitor* m_processMonitorService;                    // Infrastructure
    ProcessEventDispatcher* m_processEventDispatcherService;    // Application


    ConfigWindow* m_configWindow;
    QSystemTrayIcon* m_trayIcon;

    void createTrayIcon();
};

#endif // APPCONTROLLER_H