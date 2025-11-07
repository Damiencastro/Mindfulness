#include "CategorizeDialog.h"
#include "ApplicationRepository.h"
#include "Application.h"

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>

CategorizeDialog::CategorizeDialog(const QString& processName, ApplicationRepository* appRepo, QWidget *parent)
    : QDialog(parent),
      m_appRepository(appRepo),
      m_application(nullptr),
      m_promptLabel(nullptr),
      m_categoryComboBox(nullptr),
      m_saveButton(nullptr)
{
    // --- 1. Get or Create the Application Object ---
    // We get the object from the repository. This is the "Single Source of Truth".
    m_application = m_appRepository->findOrCreate(processName);

    if (!m_application) {
        qWarning() << "CategorizeDialog: Could not find or create Application object for" << processName;
        // Don't create the dialog if the app object is null
        reject(); // Close immediately
        return;
    }

    // --- 2. Create UI Elements ---
    m_promptLabel = new QLabel(QString("What category does '%1' belong to?").arg(processName));
    m_promptLabel->setWordWrap(true);

    m_categoryComboBox = new QComboBox();
    
    // Populate the combo box with all categories *except* Uncategorized
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Game), QVariant::fromValue(Application::Category::Game));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Leisure), QVariant::fromValue(Application::Category::Leisure));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Work), QVariant::fromValue(Application::Category::Work));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Productivity), QVariant::fromValue(Application::Category::Productivity));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Social), QVariant::fromValue(Application::Category::Social));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Educational), QVariant::fromValue(Application::Category::Educational));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::Utility), QVariant::fromValue(Application::Category::Utility));
    m_categoryComboBox->addItem(Application::categoryToString(Application::Category::System), QVariant::fromValue(Application::Category::System));
    
    // Set the default selection
    m_categoryComboBox->setCurrentIndex(m_categoryComboBox->findData(QVariant::fromValue(Application::Category::Utility)));

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    m_saveButton = buttonBox->addButton("Save", QDialogButtonBox::AcceptRole);

    // --- 3. Set up Layouts ---
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_promptLabel);
    mainLayout->addWidget(m_categoryComboBox);
    mainLayout->addWidget(buttonBox);
    
    // --- 4. Connect Signals and Slots ---
    connect(m_saveButton, &QPushButton::clicked, this, &CategorizeDialog::onSave);
    
    // --- 5. Window Properties ---
    setLayout(mainLayout);
    setWindowTitle("New Application Detected");
    setModal(true);
}

CategorizeDialog::~CategorizeDialog()
{
    // All UI elements are children of this QWidget
    // and will be deleted by Qt.
}

void CategorizeDialog::onSave()
{
    // 1. Get the selected category from the combo box
    QVariant data = m_categoryComboBox->currentData();
    Application::Category selectedCategory = data.value<Application::Category>();

    // 2. Update the Application object
    if (m_application) {
        m_application->setCategory(selectedCategory);

        // 3. Tell the repository to save this object
        // (We assume the repo has a method to save a single app)
        m_appRepository->save(m_application);
        
        qDebug() << "Categorized" << m_application->getProcessName() << "as" << Application::categoryToString(selectedCategory);
    }

    // 4. Close the dialog
    accept();
}