#include <QApplication>
#include "AppController.h"
#include "EventLogger.h"

int main(int argc, char *argv[])
{
    EventLogger::install();

    QApplication app(argc, argv);

    // Set so app doesn't quit when last window is closed
    app.setQuitOnLastWindowClosed(false); 

    AppController controller;
    
    return app.exec();
}