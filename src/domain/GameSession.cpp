#include "GameSession.h"
#include "TimeSetDialog.h"
#include "WarningDialog.h"
#include "services/utils/ProcessUtils.h"
#include <QTimer>

GameSession::GameSession(DWORD pid, const QString& processName, QObject *parent)
    : QObject(parent),
      m_pid(pid),
      m_processName(processName),
      m_countdownTimer(nullptr),
      m_warningTimer(nullptr),
      m_remainingSeconds(0),
      m_totalTimeSeconds(0)
{
    // TODO: Implement
    m_countdownTimer = new QTimer(this);
    // connect(m_countdownTimer, &QTimer::timeout, this, &GameSession::updateTimer);
}

GameSession::~GameSession()
{
    // TODO: Implement
}

void GameSession::startSessionPrompt()
{
    // TODO: Implement
    // TimeSetDialog* dialog = new TimeSetDialog();
    // connect(dialog, &TimeSetDialog::timeSelected, this, &GameSession::onTimeSet);
    // dialog->exec(); // Show modal
}

void GameSession::onTimeSet(int minutes)
{
    // TODO: Implement
    // m_remainingSeconds = minutes * 60;
    // m_totalTimeSeconds = m_remainingSeconds;
    // m_countdownTimer->start(1000); // Tick every second
}

void GameSession::updateTimer()
{
    // TODO: Implement
    // m_remainingSeconds--;
    // 
    // // Check for warning times
    // if (m_remainingSeconds == 15 * 60 ||
    //     m_remainingSeconds == 10 * 60 ||
    //     m_remainingSeconds == 5 * 60) {
    //     showWarning();
    // }
    //
    // // Check for end of time
    // if (m_remainingSeconds <= 0) {
    //     m_countdownTimer->stop();
    //     terminateGame();
    //     emit sessionFinished();
    // }
}

void GameSession::showWarning()
{
    // TODO: Implement
    // WarningDialog* dialog = new WarningDialog(m_remainingSeconds / 60);
    // connect(dialog, &WarningDialog::finished, this, &GameSession::onWarningDialogClosed);
    // dialog->exec();
}

void GameSession::onWarningDialogClosed()
{
    // TODO: Implement
    // Check if user requested extension
}

void GameSession::terminateGame()
{
    // TODO: Implement
    // ProcessUtils::terminateProcess(m_pid);
}