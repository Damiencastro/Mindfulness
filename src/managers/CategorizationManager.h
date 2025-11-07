#ifndef CATEGORIZATIONMANAGER_H
#define CATEGORIZATIONMANAGER_H

#include <QObject>
#include <QString>


// --- UPDATED FOR NEW ARCHITECTURE ---
// Forward declarations for dependencies and owned UI
class ApplicationRepository; // Replaced GameList
class ConfigWindow;
class CategorizeDialog;
// --- END UPDATE ---

/**
 * @class CategorizationManager
 * @brief Manages the application's categorization logic and UI.
 *
 * This class is responsible for the "categorization" domain. It listens for
 * signals about new, uncategorized applications and manages the display
 * of both the main ConfigWindow (the "archive") and the
 * CategorizeDialog (the "inbox").
 */
class CategorizationManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param appRepo A pointer to the application's "Single Source of Truth"
     * (the ApplicationRepository) for categorizations.
     * @param parent The parent QObject (typically AppController).
     */
    explicit CategorizationManager(ApplicationRepository* appRepo, QObject *parent = nullptr);

    /**
     * @brief Destructor. Manages cleanup of any owned UI windows.
     */
    ~CategorizationManager();

    bool isAwaitingCategorization(const QString& name);

public slots:
    /**
     * @brief Slot to handle a new, uncategorized process.
     * Connected to ProcessEventDispatcher::uncategorizedAppDetected.
     * Triggers the CategorizeDialog.
     */
    void onUncategorizedAppDetected(const QString& processName);

    /**
     * @brief Slot to show the main configuration window.
     * Connected to a signal from AppController (triggered by the tray icon).
     */
    void onShowConfigWindow();

private slots:
    /**
     * @brief Clears the pointer to the ConfigWindow when it is closed.
     * Ensures we can create a new window next time.
     */
    void onConfigWindowClosed();

    /**
     * @brief Clears the pointer to the CategorizeDialog when it is closed.
     */
    void onCategorizeDialogClosed();

private:
    /**
     * @brief Private helper to create/show the CategorizeDialog.
     * Contains logic to prevent opening multiple dialogs.
     */
    void showCategorizeDialog(const QString& processName);

    // --- Dependencies (Injected) ---
    ApplicationRepository* m_appRepository; // The master list (not owned)

    // --- State ---
    QList<QString> m_awaitingCategorization;

    // --- Owned UI Components ---
    ConfigWindow* m_configWindow;         // The main "archive" window
    CategorizeDialog* m_categorizeDialog; // The "inbox" popup dialog
};

#endif // CATEGORIZATIONMANAGER_H