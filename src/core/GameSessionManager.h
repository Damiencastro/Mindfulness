#ifndef GAMESESSIONMANAGER_H
#define GAMESESSIONMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations
class GameSession;

class GameSessionManager : public QObject
{
    Q_OBJECT

public:
    explicit GameSessionManager(QObject *parent = nullptr);
    ~GameSessionManager();

public slots:
    void onGameDetected(DWORD pid, const QString& processName);

private slots:
    void onSessionFinished();

private:
    QList<GameSession*> m_activeSessions;
};

#endif // GAMESESSIONMANAGER_H