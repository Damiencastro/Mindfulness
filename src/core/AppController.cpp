#include "AppController.h"
#include "GameList.h"
#include "ProcessMonitor.h"
#include "GameSessionManager.h"
#include "ConfigWindow.h"

#include <QSystemTrayIcon>
#include <QApplication>
#include <QMenu>
#include <QIcon>
#include <QThread>

AppController::AppController(QObject *parent)
    : QObject(parent),
      m_gameList(nullptr),
      m_processMonitor(nullptr),
      m_sessionManager(nullptr),
      m_configWindow(nullptr),
      m_trayIcon(nullptr)
{
    // --- 1. Create Core Components ---

    // GameList is just data, it's created and lives here
    m_gameList = new GameList();

    // GameSessionManager is a QObject, so we'll make it a child
    // so it gets cleaned up by Qt if AppController is destroyed.
    m_sessionManager = new GameSessionManager(this);

    // --- 2. Create Process Monitor and its Thread ---
    
    // The ProcessMonitor must have no parent (parent = nullptr)
    // so it can be moved to a different thread.
    m_processMonitor = new ProcessMonitor(m_gameList);

    // Create the thread, making it a child of AppController.
    QThread* monitorThread = new QThread(this);
    
    // Move the monitor object *to* the new thread.
    m_processMonitor->moveToThread(monitorThread);

    // --- 3. Create Tray Icon and Menu ---

    m_trayIcon = new QSystemTrayIcon(this);
    
    // We'll need to create a Qt Resource file for this icon to work.
    // For now, it might show a blank space, which is fine.
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

    
    // --- 4. Connect Signals and Slots ---

    // When monitor finds a game, tell session manager
    connect(m_processMonitor, &ProcessMonitor::gameDetected, 
            m_sessionManager, &GameSessionManager::onGameDetected);

    // When monitor finds unknown app, tell controller to show config
    connect(m_processMonitor, &ProcessMonitor::uncategorizedAppFound,
            this, &AppController::onUncategorizedAppFound);

    // When the thread starts, tell the monitor to start its timer loop
    connect(monitorThread, &QThread::started,
            m_processMonitor, &ProcessMonitor::startMonitor);

    // When onQuit is called, it will tell the thread to quit.
    // When the thread *actually* finishes, we tell the whole app to quit.
    connect(monitorThread, &QThread::finished,
            qApp, &QApplication::quit);

    // When the thread is finished, shcedule the monitor and thread for deletion.
    connect(monitorThread, &QThread::finished,
            m_processMonitor, &QObject::deleteLater);
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
    delete m_gameList; // m_gameList is not a QObject
    
    // m_configWindow is a widget. If it's still open, delete it.
    // We set WA_DeleteOnClose, but this is a final fallback.
    delete m_configWindow; 
}

void AppController::onShowConfig()
{
    // Check if the window instance already exists
    if(!m_configWindow){
        // If not, create it
        m_configWindow = new ConfigWindow(m_gameList);

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
    }

    // If it does already exist, just bring it to the front.
    m_configWindow->activateWindow();
    m_configWindow->raise();
}

void AppController::onUncategorizedAppFound(const QString& processName)
{
    // 1. Ensure the config window is open.
    onShowConfig();

    // 2. Tell the window to highlight the new app.
    // (We check if m_configWindow is valid just in case)
    if (m_configWindow){
        m_configWindow->highlightProcess(processName);
    }
}

void AppController::onQuit()
{
    // Hide icon immediately
    m_trayIcon->hide();

    // Tell the monitor to stop its internal QTimer
    m_processMonitor->stopMonitor();

    // Tell the monitor's thread event loop to stop
    // This will eventually trigger the thread's "finished" signal.
    if(m_processMonitor->thread()){
        m_processMonitor->thread()->quit();
    }

    // Our constructor connects the thread's finished() signal
    // to qApp->quit(), so the application will now exit gracefully.
}