#include "EventLogger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

// We need a persistent file handle
// A static variable in the .cpp file is a simple way to achieve this
static QFile g_logFile;

void EventLogger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 1. Format the log message
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString level;
    switch (type) {
        case QtDebugMsg:    level = "DEBUG"; break;
        case QtInfoMsg:     level = "INFO"; break;
        case QtWarningMsg:  level = "WARN"; break;
        case QtCriticalMsg: level = "CRITICAL"; break;
        case QtFatalMsg:    level = "FATAL"; break;
    }

    // [LEVEL] [Timestamp] [File:Line] Message
    QString formattedMsg = QString("[%1] [%2] [%3:%4] %5\n")
                               .arg(level)
                               .arg(timestamp)
                               .arg(context.file ? context.file : "N/A")
                               .arg(context.line)
                               .arg(msg);

    // 2. Write to the console (standard output)
    fprintf(stdout, "%s", formattedMsg.toLocal8Bit().constData());
    fflush(stdout);

    // 3. Write to the log file
    if (g_logFile.isOpen()) {
        QTextStream out(&g_logFile);
        out << formattedMsg;
    }
}

void EventLogger::install()
{
    // 1. Set the log file path
    g_logFile.setFileName("mindfulness.log");
    
    // 2. Open the file
    // We use WriteOnly | Append so we don't erase the log on every start
    if (!g_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << g_logFile.errorString();
    }

    // 3. Install our custom handler
    // qInstallMessageHandler is the Qt function that does all the work
    qInstallMessageHandler(EventLogger::messageHandler);

    qInfo() << "--- EventLogger installed. Application starting. ---";
}