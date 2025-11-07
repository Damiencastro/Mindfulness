#ifndef GAMESESSION_H
#define GAMESESSION_H

#include <QObject>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations
class QTimer;

class GameSession : public QObject
{
    Q_OBJECT

public:
    explicit GameSession(DWORD pid, const QString& processName, QObject *parent = nullptr);
    ~GameSession();

    void startSessionPrompt();

signals:
    void sessionFinished();

private slots:
    void onTimeSet(int minutes);
    void updateTimer();
    void showWarning();
    void onWarningDialogClosed();

private:
    void terminateGame();

    DWORD m_pid;
    QString m_processName;
    QTimer* m_countdownTimer;
    QTimer* m_warningTimer; // Or just calculate from countdown
    int m_remainingSeconds;
    int m_totalTimeSeconds;
};

#endif // GAMESESSION_H