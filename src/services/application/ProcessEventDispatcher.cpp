#include "ProcessEventDispatcher.h"
#include "Application.h"
#include "ApplicationRepository.h"
#include "CategorizationManager.h"
#include <QDebug>

ProcessEventDispatcher::ProcessEventDispatcher(ApplicationRepository* appRepo,
                                               CategorizationManager* catManager,
                                               QObject* parent)
    : QObject(parent),
      m_appRepository(appRepo),
      m_categorizationManager(catManager)
{
    // TODO: Validate that dependencies are not null
    Q_ASSERT(appRepo != nullptr);
    Q_ASSERT(catManager != nullptr);
    
    // Log initialization for debugging
    qDebug() << "ProcessEventDispatcher initialized";
}

void ProcessEventDispatcher::onProcessStarted(DWORD pid, const QString& processName)
{
    // TODO: Log the event for debugging
    qDebug() << "Process started:" << processName << "PID:" << pid;
    
    // TODO: Delegate to private method for clarity
    identifyAndDispatch(pid, processName);
}

void ProcessEventDispatcher::onProcessTerminated(DWORD pid)
{
    // Log the termination
    qDebug() << "Process terminated: PID" << pid;
    
    // TODO: Simply forward the termination event
    // Domain consumers will handle based on their tracked PIDs
    // emit applicationTerminated(pid);
}

void ProcessEventDispatcher::identifyAndDispatch(DWORD pid, const QString& processName)
{
    // Query the repository for this application
    Application* app = m_appRepository->find(processName);
    
    // If application not found, check for uncategorized handling
    if (!app) {
        // Check if already pending categorization
        if (!m_categorizationManager->isAwaitingCategorization(processName)) {
            qDebug() << "Uncategorized application found:" << processName;
            emit uncategorizedAppDetected(processName);
        }
        return;
    }
    
    // Route based on application category
    switch (app->getCategory()) {
        case Application::Category::Game:
        case Application::Category::Leisure:
            qDebug() << "Game detected:" << processName;
            emit gameDetected(pid, processName, app);
            break;
            
        case Application::Category::Work:
        case Application::Category::Productivity:
            qDebug() << "Work application detected:" << processName;
            emit workApplicationDetected(pid, processName, app);
            break;
            
        case Application::Category::Social:
            // TODO: Future implementation for social apps
            qDebug() << "Social application detected:" << processName;
            break;
            
        case Application::Category::Utility:
        case Application::Category::System:
            // System utilities typically don't need tracking
            qDebug() << "System/Utility detected, ignoring:" << processName;
            break;
            
        default:
            qWarning() << "Unknown category for application:" << processName;
            break;
    }
}