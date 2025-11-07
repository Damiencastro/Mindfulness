#include <QtTest/QtTest>
#include "repositories/ApplicationRepository.h"
#include "domain/Application.h" // Include the new Application class
#include <QFile>
#include <QDebug>

/**
 * @class TestApplicationRepository
 * @brief Unit tests for the ApplicationRepository.
 *
 * This test class validates the core logic of the ApplicationRepository:
 * 1. Loading from a non-existent file.
 * 2. Creating a new Application.
 * 3. Finding an existing Application.
 * 4. Persisting changes (Save/Load).
 * 5. Case-insensitivity of lookups.
 * 6. Correctly overwriting data.
 */
class TestApplicationRepository : public QObject
{
    Q_OBJECT

public:
    TestApplicationRepository()
        : m_testDbPath("test_apps.json") // Use a dedicated test database
    {
    }

private:
    QString m_testDbPath;

private slots:
    // --- Test Hooks ---

    /**
     * @brief Called before *each* test function.
     * Ensures a clean state by deleting the test database file.
     */
    void init() {
        QFile::remove(m_testDbPath);
    }

    /**
     * @brief Called after *each* test function.
     * Cleans up the test database file.
     */
    void cleanup() {
        QFile::remove(m_testDbPath);
    }

    // --- Test Cases ---

    /**
     * @brief Tests that a new repository is empty and that
     * findOrCreateByProcessName creates a new, uncategorized app.
     */
    void test_empty_repository_and_create() {
        ApplicationRepository repo(m_testDbPath); // Constructor loads (nothing)
        
        // 1. Test finding a non-existent app (should return nullptr)
        QVERIFY(repo.findOrCreate("anything.exe") == nullptr);
        
        // 2. Test creating a new app
        Application* app = repo.findOrCreate("anything.exe");
        QVERIFY(app != nullptr);
        
        // 3. New apps should be Uncategorized by default
        QCOMPARE(app->getCategory(), Application::Category::Uncategorized);
        
        // 4. Test that the app is now in the repository
        QVERIFY(repo.findOrCreate("anything.exe") != nullptr);
        QCOMPARE(repo.findOrCreate("anything.exe"), app); // Should be same pointer
    }
    
    /**
     * @brief Tests setting categories on multiple apps.
     */
    void test_add_and_retrieve() {
        ApplicationRepository repo(m_testDbPath);
        
        Application* gameApp = repo.findOrCreate("game.exe");
        gameApp->setCategory(Application::Category::Game);
        
        Application* workApp = repo.findOrCreate("work.exe");
        workApp->setCategory(Application::Category::Work);
        
        // Test that the pointers in memory have the correct category
        QCOMPARE(repo.findOrCreate("game.exe")->getCategory(), Application::Category::Game);
        QCOMPARE(repo.findOrCreate("work.exe")->getCategory(), Application::Category::Work);
    }
    
    /**
     * @brief Tests that repository lookups are case-insensitive.
     */
    void test_case_insensitive() {
        ApplicationRepository repo(m_testDbPath);
        
        // Create with one case
        Application* app1 = repo.findOrCreate("GAME.EXE");
        app1->setCategory(Application::Category::Game);

        // Find with different cases
        Application* app2 = repo.findOrCreate("game.exe");
        Application* app3 = repo.findOrCreate("GaMe.ExE");
        
        // All find operations should return the *same object pointer*
        QVERIFY(app1 != nullptr);
        QCOMPARE(app1, app2);
        QCOMPARE(app1, app3);
        
        // The category set on app1 should be reflected on all lookups
        QCOMPARE(app2->getCategory(), Application::Category::Game);
    }
    
    /**
     * @brief Tests the full Save/Load persistence logic.
     */
    void test_save_and_load() {
        // --- Save in first scope ---
        {
            ApplicationRepository repo1(m_testDbPath);
            Application* gameApp = repo1.findOrCreate("csgo.exe");
            gameApp->setCategory(Application::Category::Game);
            
            Application* workApp = repo1.findOrCreate("chrome.exe");
            workApp->setCategory(Application::Category::Work);
            
            // We assume the ApplicationRepository's destructor saves all changes
        } // repo1 is destroyed here, saving "test_apps.json"

        // --- Load in new scope ---
        {
            QVERIFY(QFile::exists(m_testDbPath)); // Verify the file was created
            
            ApplicationRepository repo2(m_testDbPath); // Constructor calls load()
            
            Application* gameApp = repo2.findOrCreate("csgo.exe");
            Application* workApp = repo2.findOrCreate("chrome.exe");
            
            // Verify the apps were loaded back from JSON
            QVERIFY(gameApp != nullptr);
            QVERIFY(workApp != nullptr);
            
            // Verify their data was preserved
            QCOMPARE(gameApp->getCategory(), Application::Category::Game);
            QCOMPARE(workApp->getCategory(), Application::Category::Work);
        }
    }
    
    /**
     * @brief Tests that an application's category can be updated.
     */
    void test_overwrite_categorization() {
        ApplicationRepository repo(m_testDbPath);
        
        Application* app = repo.findOrCreate("app.exe");
        
        // Set category to Work
        app->setCategory(Application::Category::Work);
        QCOMPARE(repo.findOrCreate("app.exe")->getCategory(), Application::Category::Work);
        
        // Now overwrite it as a Game
        app->setCategory(Application::Category::Game);
        QCOMPARE(repo.findOrCreate("app.exe")->getCategory(), Application::Category::Game);
    }
};

// Generate test main function
QTEST_MAIN(TestApplicationRepository)
// This must match the class name
#include "test_ApplicationRepository.moc"