#include "ConfigWindow.h"
#include "GameList.h"
#include "ProcessUtils.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHash>
#include <QList>
#include <QSet>

#include <QDebug>

ConfigWindow::ConfigWindow(GameList* gameList, QWidget *parent)
    : QWidget(parent),
      m_gameList(gameList),
      m_listWidget(nullptr),
      m_saveButton(nullptr),
      m_refreshButton(nullptr)
{
    // 1. Create the UI Widgets
    m_listWidget = new QListWidget();
    m_saveButton = new QPushButton("Save and Close");
    m_refreshButton = new QPushButton("Refresh");
    
    // 2. Create a layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_refreshButton);
    layout->addWidget(m_listWidget);
    layout->addWidget(m_saveButton);
    
    // 3. Set the layout for the window
    setLayout(layout);

    // 4. Connect the button signals to our slots
    connect(m_saveButton, &QPushButton::clicked, this, &ConfigWindow::onSave);
    connect(m_refreshButton, &QPushButton::clicked, this, &ConfigWindow::onRefresh);
    
    // 5. Set window properties
    setWindowTitle("Mindfulness - Configuration");
    resize(400, 500);

    // 6. Fill the list with data from GameList
    populateList();
}

ConfigWindow::~ConfigWindow()
{
    // No code needed. Qt's parent-child system
    // should delete m_listWidget, m_saveButton, etc.
}

void ConfigWindow::highlightProcess(const QString& processName)
{
    // 1. Search for the item
    // We use .toLower() for a case-insensitive match
    QList<QListWidgetItem*> items = m_listWidget->findItems(processName, Qt::MatchFixedString);

    if(!items.isEmpty()){
        // 2. Foudn it. Highlight and scroll to it
        QListWidgetItem* item = items.first();
        m_listWidget->setCurrentItem(item);
        m_listWidget->scrollToItem(item);
    } else{
        // 3. Not found. This is a new, uncategorized app.
        // Add it to the list so the user can categorize it.
        QListWidgetItem* newItem = new QListWidgetItem(processName);
        newItem->setFlags(newItem->flags() | Qt::ItemIsUserCheckable);
        newItem->setCheckState(Qt::Unchecked);

        m_listWidget->addItem(newItem);

        // Now highlight and scroll to the new item.
        m_listWidget->setCurrentItem(newItem);
        m_listWidget->scrollToItem(newItem);
    }
}

void ConfigWindow::onSave()
{
    // --- Defensive Check 1 ---
    if(m_listWidget == nullptr){
        qWarning() << "ConfigWindow::onSave() failed: m_listWidget is null.";
        return;
    }

    // --- Defensive Check 2 ---
    if(m_gameList == nullptr){
        qWarning() << "ConfigWindow::onSave() failed: m_gameList is null.";
        return; 
    }


    // 1. Loop through all items in the list
    for (int i = 0; i < m_listWidget->count(); ++i){
        QListWidgetItem* item = m_listWidget->item(i);

        if(item == nullptr) continue;

        // 2. Get the item's text and check state
        QString name = item->text();
        bool isGame = (item->checkState() == Qt::Checked);

        // 3. Update the GameList's in-memory map
        m_gameList->setCategorization(name, isGame);
    }

    // 4. Tell GameList to write all changes to "games.txt"
    m_gameList->save();

    // 5. Close the window
    close();
}

void ConfigWindow::onRefresh()
{
    // 1. Re-run the populate logic
    populateList();

    // TODO: Later, we will add logic here to scan for 
    // new running processes and add them to the list
}

void ConfigWindow::populateList()
{
    // 0. Clear the list to prevent duplicates
    m_listWidget->clear();

    // 1. Get all *categorized* apps from our save file
    QHash<QString, bool> categorizations = m_gameList->getAllCategorizations();
    QHash<DWORD, QString> runningProcs = ProcessUtils::getActiveProcesses();

    // TODO: Need to revisit this logic and make sure that values are being copied performantly, might cause some issues

    QList<QString> runningProcsValues = runningProcs.values();
    QSet<QString> runningAppNames = QSet<QString>(runningProcsValues.begin(), runningProcsValues.end());
    
    // 3. Create a unified set of all process names (saved + running)
    QList<QString> categorizedAppNames = categorizations.keys();
    //    QSet automatically handles duplicates.
    QSet<QString> allAppNames(categorizedAppNames.begin(), categorizedAppNames.end());
    
    allAppNames.unite(runningAppNames);

    // 4. Convert the set to a list and sort it
    QList<QString> sortedAppNames = allAppNames.values();
    sortedAppNames.sort(Qt::CaseInsensitive);

    // 5. Loop through the *unified*, sorted list
    for (const QString& appName : sortedAppNames){
        
        // 6. Get the value (is it a game?)
        //    Use .value() which defaults to 'false' if the app
        //    is running but not in our categorized list.
        bool isGame = categorizations.value(appName, false);

        // 7. Create the list item
        QListWidgetItem* item = new QListWidgetItem(appName);

        // 8. Make it checkable
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

        // 9. Set its check state
        item->setCheckState(isGame ? Qt::Checked : Qt::Unchecked);

        // 10. Add it to the list widget
        m_listWidget->addItem(item);
    }
}