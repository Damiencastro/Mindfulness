#include "ConfigWindow.h"

#include <QListWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QListWidgetItem>

ConfigWindow::ConfigWindow(
    QList<Application*>& games, 
    QList<Application*>& otherApps, 
    QWidget *parent
)
    : QWidget(parent),
      m_gameListWidget(nullptr),
      m_appListWidget(nullptr),
      m_gameBox(nullptr),
      m_appBox(nullptr)
{
    // --- 1. Create UI Elements ---

    // Create the two main groups
    m_gameBox = new QGroupBox("Games & Leisure");
    m_appBox = new QGroupBox("Other Applications (Work, Utility, etc.)");

    // Create the two list widgets
    m_gameListWidget = new QListWidget();
    m_appListWidget = new QListWidget();

    // --- 2. Set up Layouts ---

    // Create a layout for the "Games" group box
    QVBoxLayout* gameLayout = new QVBoxLayout();
    gameLayout->addWidget(m_gameListWidget);
    m_gameBox->setLayout(gameLayout);

    // Create a layout for the "Other Apps" group box
    QVBoxLayout* appLayout = new QVBoxLayout();
    appLayout->addWidget(m_appListWidget);
    m_appBox->setLayout(appLayout);

    // Create the main window layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this); // 'this' sets layout on window
    mainLayout->addWidget(m_gameBox);
    mainLayout->addWidget(m_appBox);

    // --- 3. Populate Lists with Data ---
    // Call the helper function to fill the two list widgets
    populateList(m_gameListWidget, games);
    populateList(m_appListWidget, otherApps);

    // --- 4. Set Window Properties ---
    setLayout(mainLayout);
    setWindowTitle("Mindfulness - Application Configuration");
    resize(500, 600); // Set a reasonable default size
}

ConfigWindow::~ConfigWindow()
{
    // All UI elements are children of this QWidget and will be
    // automatically deleted by Qt. No manual cleanup needed.
}

/**
 * @brief Private helper to populate a QListWidget with Application data.
 */
void ConfigWindow::populateList(
    QListWidget* listWidget, 
    QList<Application*>& applications
)
{
    listWidget->clear();

    for (const Application* appPtr : applications) 
    {
        auto app = *appPtr;
        // Use the Display Name, but fall back to the process name
        QString displayName = app.getDisplayName();
        if (displayName.isEmpty()) {
            displayName = app.getProcessName();
        }

        // Create the list item
        QListWidgetItem* item = new QListWidgetItem(displayName);

        // Add a detailed tooltip with stats from the Application object
        QString tooltip = QString(
            "Process: %1\n"
            "Category: %2\n"
            "Total Use: %3 minutes (%4 sessions)\n"
            "First Seen: %5"
        )
        .arg(app.getProcessName())
        .arg(Application::categoryToString(app.getCategory()))
        .arg(app.getTotalMinutesUsed())
        .arg(app.getTotalSessions())
        .arg(app.getFirstSeen().toString(Qt::ISODate));
        
        item->setToolTip(tooltip);

        // Per the "passive component" requirement, items are read-only.
        // We ensure the checkable flag is OFF.
        item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);

        listWidget->addItem(item);
    }
}
