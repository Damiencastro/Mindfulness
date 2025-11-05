#ifndef GAMELIST_H
#define GAMELIST_H

#include <QString>
#include <QHash>

class GameList
{
public:
    GameList();
    ~GameList();

    bool isGame(const QString& processName) const;
    bool isCategorized(const QString& processName) const;
    void setCategorization(const QString& processName, bool isGame);
    
    QHash<QString, bool> getAllCategorizations() const;
    void save();

private:
    void load();

    QHash<QString, bool> m_categorizations;
    QString m_filePath;
};

#endif // GAMELIST_H