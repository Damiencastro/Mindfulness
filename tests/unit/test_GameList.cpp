#include <QtTest/QtTest>
#include "core/GameList.h"
#include <QFile>

class TestGameList : public QObject {
    Q_OBJECT

private slots:
    // Called before each test
    void init() {
        QFile::remove("games.txt");  // Clean state
    }
    
    // Basic functionality tests
    void test_empty_list() {
        GameList list;
        QVERIFY(!list.isCategorized("anything.exe"));
        QVERIFY(!list.isGame("anything.exe"));
    }
    
    void test_add_and_retrieve() {
        GameList list;
        list.setCategorization("game.exe", true);
        list.setCategorization("work.exe", false);
        
        QVERIFY(list.isCategorized("game.exe"));
        QVERIFY(list.isGame("game.exe"));
        QVERIFY(list.isCategorized("work.exe"));
        QVERIFY(!list.isGame("work.exe"));
    }
    
    void test_case_insensitive() {
        GameList list;
        list.setCategorization("GAME.EXE", true);
        
        QVERIFY(list.isGame("game.exe"));
        QVERIFY(list.isGame("GaMe.ExE"));
        QVERIFY(list.isGame("GAME.EXE"));
    }
    
    void test_save_and_load() {
        // Save in one scope
        {
            GameList list1;
            list1.setCategorization("csgo.exe", true);
            list1.setCategorization("chrome.exe", false);
            list1.save();
        }
        
        // Load in new scope
        {
            GameList list2;  // Constructor calls load()
            QVERIFY(list2.isGame("csgo.exe"));
            QVERIFY(!list2.isGame("chrome.exe"));
        }
    }
    
    void test_overwrite_categorization() {
        GameList list;
        list.setCategorization("app.exe", false);
        QVERIFY(!list.isGame("app.exe"));
        
        list.setCategorization("app.exe", true);
        QVERIFY(list.isGame("app.exe"));
    }
};

// Generate test main function
QTEST_MAIN(TestGameList)
#include "test_GameList.moc"