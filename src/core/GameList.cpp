#include "GameList.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

GameList::GameList() 
    : m_filePath("games.txt")
{
    // TODO: Implement
    load();
}

GameList::~GameList()
{
    // No action should be needed as saving is handled
    // explicitly by the ConfigWindow calling save().
}

bool GameList::isGame(const QString& processName) const
{
    // Use .toLower() for a case-insensitive lookup
    // .value() returns the value for the key, or 'false' if the key doesn't exist
    return m_categorizations.value(processName.toLower(), false);
}

bool GameList::isCategorized(const QString& processName) const
{
    // Use .toLower() for a case-insensitive lookup
    return m_categorizations.contains(processName.toLower());
}

void GameList::setCategorization(const QString& processName, bool isGame)
{
    // Insert/update the value in the hash map.
    m_categorizations.insert(processName.toLower(), isGame);
}

QHash<QString, bool> GameList::getAllCategorizations() const
{
    return m_categorizations;
}

void GameList::save()
{
    QFile file(m_filePath);

    // Open in WriteOnly (creates if nonexistent), Truncate (clears file), Text (handles newlines)
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Could not open " << m_filePath << " for writing: " << file.errorString();
        return;
    }

    QTextStream out(&file);

    // Get a QList of all keys (process names) stored in the hash
    QList<QString> keys = m_categorizations.keys();

    // Now, we iterate over that list of keys,
    for(const QString& key : keys){
        
        // Look up the value (the bool) for the current key
        bool value = m_categorizations.value(key);

        // Write the line in our "key,flag" format
        out << key << "," << (value ? "1" : "0") << "\n";
    }

    // File should be closed automatically by QFile's destructor
}

void GameList::load()
{
    QFile file(m_filePath);

    // If the file doesn't exist yet, that's fine
    if(!file.exists()){
        return;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open" << m_filePath << "for reading:" << file.errorString();
        return;
    }

    // Clear any existing data
    m_categorizations.clear();

    QTextStream in(&file);
    while (!in.atEnd()){
        QString line = in.readLine();
        QStringList parts = line.split(',');

        // Ensure the line is valid (e.g., "csgo.exe,1")
        if(parts.size() == 2){
            QString processName = parts[0].trimmed();
            bool isGame = (parts[1].trimmed() == "1"); // Convert "1" to true

            if(!processName.isEmpty()){
                m_categorizations.insert(processName.toLower(), isGame);
            }
        }
    }
}