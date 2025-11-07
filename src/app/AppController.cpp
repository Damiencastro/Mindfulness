#include "AppController.h"
#include "../services/infrastructure/ProcessMonitor.h"
#include "ApplicationRepository.h"
#include "CategorizationManager.h"
#include "../services/infrastructure/ProcessMonitor.h"
#include "../services/application/ProcessEventDispatcher.h"
#include "GameSessionManager.h"
#include "ConfigWindow.h"

#include <QSystemTrayIcon>
#include <QApplication>
#include <QMenu>
#include <QIcon>
#include <QThread>

AppController::AppController(QObject *parent)
    : QObject(parent),
      m_appRepository(nullptr),
      m_processMonitorService(nullptr),
      m_processEventDispatcherService(nullptr),
      m_sessionManager(nullptr),
      m_configWindow(nullptr),
      m_trayIcon(nullptr)
{
    // --- Core Components ---

    // Repositories
    m_appRepository = new ApplicationRepository();

    // Managers
    m_sessionManager = new GameSessionManager(this);
    m_categorizationManager = new CategorizationManager(m_appRepository, this);

    // Services

    // The ProcessMonitor must have no parent (parent = nullptr)
    // so it can be moved to a different thread.
    m_processMonitorService = new ProcessMonitor();  // Infrastructure
    m_processEventDispatcherService = new ProcessEventDispatcher(m_appRepository, m_categorizationManager, this);

    // --- 2. Create Process Monitor and its Thread ---
    QThread* monitorThread = new QThread(this);     // Create the thread, making it a child of AppController.
    m_processMonitorService->moveToThread(monitorThread);  // Move the monitor object *to* the new thread.

    // --- 3. Create Tray Icon and Menu ---
    createTrayIcon();

    
    // --- 4. Connect Signals and Slots ---

    // Connect ProcessMonitor's signals with ProcessEventDispatcher's slots

    connect (m_processMonitorService, &ProcessMonitor::processStarted,
            m_processEventDispatcherService, &ProcessEventDispatcher::onProcessStarted);

    // When monitor finds a game, tell session manager
    connect(m_processMonitorService, &ProcessMonitor::processTerminated, 
            m_processEventDispatcherService, &ProcessEventDispatcher::onProcessTerminated);

    // When monitor finds unknown app, tell controller to show config
    connect(m_processEventDispatcherService, &ProcessEventDispatcher::gameDetected,
            m_sessionManager, &GameSessionManager::onGameDetected);

    connect(m_processEventDispatcherService, &ProcessEventDispatcher::uncategorizedAppDetected,
            m_categorizationManager, &CategorizationManager::onUncategorizedAppDetected);

    // When the thread starts, tell the monitor to start its timer loop
    connect(monitorThread, &QThread::started,
            m_processMonitorService, &ProcessMonitor::startMonitor);

    // When onQuit is called, it will tell the thread to quit.
    // When the thread *actually* finishes, we tell the whole app to quit.
    connect(monitorThread, &QThread::finished,
            qApp, &QApplication::quit);

    // When the thread is finished, shcedule the monitor and thread for deletion.
    connect(monitorThread, &QThread::finished,
            m_processMonitorService, &QObject::deleteLater);
    connect(monitorThread, &QThread::finished,
            monitorThread, &QObject::deleteLater);

    // --- 5. Start the Thread ---
    monitorThread->start();
}

AppController::~AppController()
{
    // The QThread and ProcessMonitor are cleaned up via deleteLater().
    // The QSystemTrayIcon and GameSessionManager are children and cleaned up by Qt.
    
    // We must manually delete the components we created
    // that are not QObject children.
    delete m_appRepository; // m_gameList is not a QObject
    
    // m_configWindow is a widget. If it's still open, delete it.
    // We set WA_DeleteOnClose, but this is a final fallback.
    delete m_configWindow; 
}

void AppController::onShowConfig()
{
    // Check if the window instance already exists
    if(m_configWindow){
        m_configWindow->activateWindow();
        m_configWindow->raise();
        return;
    }

    QList<Application*> gameApps;
    QList<Application*> otherApps;

    QList<Application*> allApps = m_appRepository->findAll();

    for(Application* app : allApps){
        if(app == nullptr) continue;

        Application::Category cat = app->getCategory();

        if(cat == Application::Category::Game || cat == Application::Category::Leisure){
            gameApps.append(app);
        }else{
            otherApps.append(app);
        }
    }

        


    
    m_configWindow = new ConfigWindow(gameApps, otherApps, nullptr);

    // This attribute tells Qt to delete the widget
    // when it's closed (e.g., user clicks the 'X').
    m_configWindow->setAttribute(Qt::WA_DeleteOnClose);

    // CRITICAL: When the window is deleted, we must
    // set our pointer back to nullptr so we know to create
    // a new one next time.
    connect(m_configWindow, &ConfigWindow::destroyed, this, [this](){
        m_configWindow = nullptr;
    });

    m_configWindow->show();
    m_configWindow->activateWindow();
    m_configWindow->raise();
}

void AppController::onQuit(){
    // Hide icon immediately
    m_trayIcon->hide();

    // Tell the monitor to stop its internal QTimer
    m_processMonitorService->stopMonitor();

    // Tell the monitor's thread event loop to stop
    // This will eventually trigger the thread's "finished" signal.
    if(m_processMonitorService->thread()){
        m_processMonitorService->thread()->quit();
    }

    // Our constructor connects the thread's finished() signal
    // to qApp->quit(), so the application will now exit gracefully.
}

void AppController::createTrayIcon(){
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/app.ico"));
    m_trayIcon->setToolTip("Mindfulness is running");

    // Create the right-click menu
    QMenu* trayMenu = new QMenu();
    trayMenu->addAction("Configuration", this, &AppController::onShowConfig);
    trayMenu->addSeparator();

    // Add the "Quit" action and connect it directly
    QAction* quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, this, &AppController::onQuit);

    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();
}