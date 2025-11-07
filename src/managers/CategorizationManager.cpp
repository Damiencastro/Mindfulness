#include "CategorizationManager.h"
#include "ConfigWindow.h"
#include "CategorizeDialog.h"      // <-- We'll need to create this new dialog
#include "ApplicationRepository.h"
#include "Application.h"
#include <QDebug>
#include <windows.h>



CategorizationManager::CategorizationManager(ApplicationRepository* appRepo, QObject *parent)
    : QObject(parent),
      m_appRepository(appRepo), // Correctly storing the injected dependency
      m_configWindow(nullptr),
      m_categorizeDialog(nullptr)
{
    // Constructor's job is just to initialize pointers.
    // All work is done in slots.

}

CategorizationManager::~CategorizationManager()
{
    // Fallback cleanup.
    // Note: This should be unnecessary if WA_DeleteOnClose works,
    // but it's good practice.
    delete m_configWindow;
    delete m_categorizeDialog;
}

/**
 * @brief Query function to test whether uncategorized app is awaiting categorization
 * @returns True when process is awaiting categorization
 */
bool CategorizationManager::isAwaitingCategorization(const QString& name){
    if(m_awaitingCategorization.contains(name)){
        return true;
    }
    return false;
}

/**
 * @brief Slot to handle a new, uncategorized process.
 */
void CategorizationManager::onUncategorizedAppDetected(const QString& processName)
{
    // This is the "inbox" flow.
    // We call our private helper to show the dedicated dialog.
    showCategorizeDialog(processName);
}

/**
 * @brief Slot to show the main configuration window.
 */
void CategorizationManager::onShowConfigWindow()
{
    // This is the "archive" flow.
    
    // 1. Check if window already exists.
    if(m_configWindow) {
        m_configWindow->activateWindow();
        m_configWindow->raise();
        return;
    }

    // --- 2. Prepare the data for the "passive" UI ---
    // This is the logic you were trying to place in AppController.
    // It now lives here, in the correct manager.

    QList<Application*> gameApps;
    QList<Application*> otherApps;

    // Get the master list ONCE.
    QList<Application*> allApps = m_appRepository->findAll();

    // Sort the list into two buckets in a single O(N) pass.
    for (Application* app : allApps)
    {
        if (app == nullptr) continue;

        Application::Category cat = app->getCategory();

        if (cat == Application::Category::Game || cat == Application::Category::Leisure) {
            gameApps.append(app);
        } else {
            // Includes "Work", "Productivity", "System", "Uncategorized", etc.
            otherApps.append(app);
        }
    }

    // --- 3. Create the window ---
    
    // We pass the sorted lists to the passive ConfigWindow constructor.
    m_configWindow = new ConfigWindow(gameApps, otherApps, nullptr);
    m_configWindow->setAttribute(Qt::WA_DeleteOnClose);

    // Connect the window's destroyed signal to our cleanup slot.
    connect(m_configWindow, &ConfigWindow::destroyed,
            this, &CategorizationManager::onConfigWindowClosed);

    m_configWindow->show();
    m_configWindow->activateWindow();
    m_configWindow->raise();
}

/**
 * @brief Clears the pointer to the ConfigWindow when it is closed.
 */
void CategorizationManager::onConfigWindowClosed()
{
    m_configWindow = nullptr;
}

/**
 * @brief Clears the pointer to the CategorizeDialog when it is closed.
 */
void CategorizationManager::onCategorizeDialogClosed()
{
    m_categorizeDialog = nullptr;
}

/**
 * @brief Private helper to create/show the CategorizeDialog.
 */
void CategorizationManager::showCategorizeDialog(const QString& processName)
{
    m_awaitingCategorization.append(processName);

    // 1. Check if the "inbox" dialog already exists.
    if (m_categorizeDialog) {
        // TODO: We need to tell the existing dialog to
        // add the new processName to its list.
        
        m_categorizeDialog->activateWindow();
        m_categorizeDialog->raise();
        return;
    }

    // --- 2. Create the dialog ---
    // NOTE: This assumes a constructor for CategorizeDialog exists
    // that takes the repository. We will need to design this dialog.
    
    m_categorizeDialog = new CategorizeDialog(processName, m_appRepository, nullptr);
    m_categorizeDialog->setAttribute(Qt::WA_DeleteOnClose);
    
    connect(m_categorizeDialog, &CategorizeDialog::destroyed,
            this, &CategorizationManager::onCategorizeDialogClosed);

    m_categorizeDialog->show();
    m_categorizeDialog->activateWindow();
    m_categorizeDialog->raise();

    // For now, let's just log it so we know the signal is working.
    qDebug() << "Signal received: Uncategorized app detected:" << processName;
}