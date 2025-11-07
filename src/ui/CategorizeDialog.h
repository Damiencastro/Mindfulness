#ifndef CATEGORIZEDIALOG_H
#define CATEGORIZEDIALOG_H

#include <QDialog>
#include "Application.h" // For the Application class and Category enum

// Forward declarations
class ApplicationRepository;
class Application;
class QLabel;
class QComboBox;
class QPushButton;

/**
 * @class CategorizeDialog
 * @brief A modal dialog to categorize a single, newly detected application.
 *
 * This is the "inbox" UI. It is created by the CategorizationManager
 * when an uncategorized process is detected. It handles getting
 * the Application object from the repository, prompting the user,
 * and saving the new category.
 */
class CategorizeDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the dialog.
     * @param processName The executable name of the app to categorize.
     * @param appRepo A pointer to the ApplicationRepository.
     * @param parent The parent widget.
     */
    explicit CategorizeDialog(const QString& processName, ApplicationRepository* appRepo, QWidget *parent = nullptr);
    ~CategorizeDialog();



private slots:
    /**
     * @brief Called when the "Save" button is clicked.
     * Saves the new category to the repository and closes the dialog.
     */
    void onSave();

private:
    // --- Injected Dependencies ---
    ApplicationRepository* m_appRepository; // Not owned

    // --- State ---
    Application* m_application; // The Application object we are editing (not owned)

    // --- UI Elements ---
    QLabel* m_promptLabel;
    QComboBox* m_categoryComboBox;
    QPushButton* m_saveButton;
};

#endif // CATEGORIZEDIALOG_H