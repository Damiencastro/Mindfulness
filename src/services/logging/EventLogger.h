#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

#include <QtGlobal>

/**
 * @class EventLogger
 * @brief A static utility class to install a global, thread-safe message handler.
 *
 * This class redirects all Qt logging output (qDebug, qWarning, etc.)
 * to both the console and a persistent log file ("mindfulness.log").
 */
class EventLogger
{
public:
    /**
     * @brief Installs the custom message handler.
     * This should be called *once* at the very beginning of main().
     */
    static void install();

private:
    // This is the function that will actually handle all log messages
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // LOGGER_H