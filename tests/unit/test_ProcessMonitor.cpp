#include <QtTest/QtTest>
#include "monitoring/ProcessMonitor.h"
#include "core/GameList.h"

// Mock process source for testing
class MockProcessSource {
public:
    QMap<DWORD, QString> mockProcesses;
    
    QMap<DWORD, QString> getProcesses() {
        return mockProcesses;
    }
    
    void addProcess(DWORD pid, QString name) {
        mockProcesses[pid] = name;
    }
    
    void removeProcess(DWORD pid) {
        mockProcesses.remove(pid);
    }
};

class TestProcessMonitor : public QObject {
    Q_OBJECT
    
private slots:
    void test_signal_on_new_game() {
        GameList list;
        list.setCategorization("game.exe", true);
        
        ProcessMonitor monitor(&list);
        QSignalSpy gameSpy(&monitor, &ProcessMonitor::gameDetected);
        
        // Simulate finding a game - you'll need to adapt this
        // based on your actual implementation
        
        QCOMPARE(gameSpy.count(), 0);  // No signals yet
    }
    
    void test_signal_on_uncategorized() {
        GameList list;
        ProcessMonitor monitor(&list);
        
        QSignalSpy uncategorizedSpy(&monitor, &ProcessMonitor::uncategorizedAppFound);
        
        // Test logic here
        
        QCOMPARE(uncategorizedSpy.count(), 0);  // Adjust based on test
    }
    
    void test_no_duplicate_signals() {
        GameList list;
        list.setCategorization("game.exe", true);
        
        ProcessMonitor monitor(&list);
        QSignalSpy gameSpy(&monitor, &ProcessMonitor::gameDetected);
        
        // Simulate same game detected twice
        // Should only signal once for new detection
    }
};

QTEST_MAIN(TestProcessMonitor)
#include "test_ProcessMonitor.moc"