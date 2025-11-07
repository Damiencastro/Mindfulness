#ifndef PROCESSEVENTDISPATCHER_H
#define PROCESSEVENTDISPATCHER_H

#include <QObject>
#include <QString>
#include <windows.h> // For DWORD

// Forward declarations
class Application;
class ApplicationRepository;
class CategorizationManager;

/**
 * @brief Translates low-level process events into domain-specific business events
 * 
 * This Application Service acts as a translator between the infrastructure layer
 * (ProcessMonitor) and the business layer (Managers). It queries repositories
 * to identify applications and emits appropriate domain events based on
 * business rules.
 * 
 * This component is stateless and purely functional.
 */
class ProcessEventDispatcher : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Process Event Dispatcher
     * @param appRepo Repository for application lookups (required, not null)
     * @param catManager Manager for categorization state (required, not null)
     * @param parent Standard QObject parent
     */
    explicit ProcessEventDispatcher(ApplicationRepository* appRepo,
                                   CategorizationManager* catManager,
                                   QObject* parent = nullptr);
    
    ~ProcessEventDispatcher() override = default;

public slots:
    /**
     * @brief Handle infrastructure notification of process start
     * @param pid Windows Process ID
     * @param processName Executable name (e.g., "chrome.exe")
     */
    void onProcessStarted(DWORD pid, const QString& processName);
    
    /**
     * @brief Handle infrastructure notification of process termination
     * @param pid Windows Process ID of terminated process
     */
    void onProcessTerminated(DWORD pid);

signals:
    /**
     * @brief Emitted when a known game or leisure application starts
     * @param pid Process ID
     * @param processName Executable name
     * @param app Application entity (never null)
     */
    void gameDetected(DWORD pid, const QString& processName, Application* app);
    
    /**
     * @brief Emitted when a known work/productivity application starts
     * @param pid Process ID
     * @param processName Executable name
     * @param app Application entity (never null)
     */
    void workApplicationDetected(DWORD pid, const QString& processName, Application* app);
    
    /**
     * @brief Emitted when an uncategorized application is detected
     * @param processName Executable name
     */
    void uncategorizedAppDetected(const QString& processName);
    
    /**
     * @brief Emitted when any tracked process terminates
     * @param pid Process ID of terminated application
     */
    void applicationTerminated(DWORD pid);

private:
    /**
     * @brief Identify application and emit appropriate domain event
     * @param pid Process ID
     * @param processName Executable name
     */
    void identifyAndDispatch(DWORD pid, const QString& processName);

    // Dependencies (not owned)
    ApplicationRepository* m_appRepository;
    CategorizationManager* m_categorizationManager;
};

#endif // PROCESSEVENTDISPATCHER_H