#include <QTest>
#include <QObject>
#include <QSignalSpy>

// Include your actual source file
#include "../../src/domain/GameSession.h"

class TestGameSession : public QObject {
    Q_OBJECT
    
private slots:
    void initTestCase() {
        // Called before first test
    }
    
    void test_session_creation() {
        GameSession session(1234, "game.exe");
        QVERIFY(true);  // Simple test to start
    }
    
    void cleanupTestCase() {
        // Called after last test
    }
};

// Don't include the .moc file yet - let CMake handle it
QTEST_MAIN(TestGameSession)
#include "test_GameSession.moc"  // This line can be removed if causing issues