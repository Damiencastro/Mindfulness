#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager::GameSessionManager(QObject *parent)
    : QObject(parent)
{
    // TODO: Implement
}

GameSessionManager::~GameSessionManager()
{
    // TODO: Implement
    // Delete all remaining sessions
    qDeleteAll(m_activeSessions);
}

void GameSessionManager::onGameDetected(DWORD pid, const QString& processName)
{
    // TODO: Implement
    // 1. Check if a session for this PID or Name already exists
    // 2. If not, create a new one:
    // GameSession* session = new GameSession(pid, processName);
    // connect(session, &GameSession::sessionFinished, 
    //         this, &GameSessionManager::onSessionFinished);
    // m_activeSessions.append(session);
    // session->startSessionPrompt(); // New method to show dialog
}

void GameSessionManager::onSessionFinished()
{
    // TODO: Implement
    // GameSession* session = qobject_cast<GameSession*>(sender());
    // if (session) {
    //     m_activeSessions.removeOne(session);
    //     session->deleteLater();
    // }
}