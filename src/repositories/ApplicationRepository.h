#ifndef APPLICATIONREPOSITORY_H
#define APPLICATIONREPOSITORY_H

#include "Application.h"
#include <QString>
#include <QHash>
#include <QList>
#include <memory>

// Forward declaration
class Application;
class QJsonObject;

/**
 * @brief Repository for managing Application entity persistence
 * 
 * This class serves as the Single Source of Truth for all application data.
 * It abstracts the storage mechanism (currently JSON file, future: database)
 * and provides a clean interface for querying and persisting Application entities.
 * 
 * Thread Safety: This class is NOT thread-safe. Read operations can be called
 * from any thread, but write operations must only be called from the main thread.
 */
class ApplicationRepository
{
public:
    ApplicationRepository();
    explicit ApplicationRepository(const QString& dataPath);
    ~ApplicationRepository();
    
    // Core CRUD Operations
    
    /**
     * @brief Find an application by process name
     * @param processName The executable name (e.g., "chrome.exe")
     * @return Pointer to Application or nullptr if not found
     */
    Application* find(const QString& processName) const;
    
    /**
     * @brief Find or create an application
     * @param processName The executable name
     * @return Pointer to existing or newly created Application
     */
    Application* findOrCreate(const QString& processName);
    
    /**
     * @brief Save or update an application
     * @param app The application to persist
     */
    void save(Application* app);
    
    /**
     * @brief Remove an application from the repository
     * @param processName The executable name
     * @return true if removed, false if not found
     */
    bool remove(const QString& processName);
    
    // Query Methods
    
    /**
     * @brief Get all applications
     * @return List of all Application pointers
     */
    QList<Application*> findAll() const;
    
    /**
     * @brief Find all applications by category
     * @param category The category to filter by
     * @return List of matching Application pointers
     */
    QList<Application*> findByCategory(Application::Category category) const;
    
    /**
     * @brief Get count of all applications
     * @return Total number of applications
     */
    int count() const;
    
    /**
     * @brief Check if an application exists
     * @param processName The executable name
     * @return true if exists, false otherwise
     */
    bool exists(const QString& processName) const;
    
    // Persistence Operations
    
    /**
     * @brief Save all changes to persistent storage
     * @return true if successful, false otherwise
     */
    bool saveAll();
    
    /**
     * @brief Load all data from persistent storage
     * @return true if successful, false otherwise
     */
    bool load();
    
    /**
     * @brief Clear all data (in memory only)
     */
    void clear();
    
    // Statistics Queries (Future Enhancement)
    
    /**
     * @brief Get recently used applications
     * @param days Number of days to look back
     * @return List of recently used applications
     */
    QList<Application*> findRecentlyUsed(int days = 7) const;
    
    /**
     * @brief Get frequently used applications
     * @param minSessions Minimum number of sessions
     * @return List of frequently used applications
     */
    QList<Application*> findFrequentlyUsed(int minSessions = 10) const;

private:
    /**
     * @brief Internal storage of applications
     * Key: process name (lowercase), Value: Application pointer
     */
    QHash<QString, std::shared_ptr<Application>> m_applications;
    
    /**
     * @brief Path to the data file
     */
    QString m_dataPath;
    
    /**
     * @brief Track if there are unsaved changes
     */
    bool m_isDirty;
    
    // Helper Methods
    
    /**
     * @brief Normalize process name for consistent storage
     * @param processName The process name to normalize
     * @return Lowercase version of the process name
     */
    QString normalizeProcessName(const QString& processName) const;
    
    /**
     * @brief Convert repository to JSON for persistence
     * @return JSON representation of all applications
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Load repository from JSON
     * @param json The JSON data to load
     */
    void fromJson(const QJsonObject& json);
    
    // Constants
    static constexpr const char* DEFAULT_DATA_FILE = "applications.json";
    static constexpr int FILE_VERSION = 1;
};

#endif // APPLICATIONREPOSITORY_H