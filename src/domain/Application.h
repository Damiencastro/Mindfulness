#ifndef APPLICATION_H
#define APPLICATION_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Domain entity representing a monitored application
 * 
 * This class encapsulates all data and business logic related to
 * an application that Mindfulness tracks. It includes categorization,
 * usage statistics, and custom configuration.
 */
class Application
{
public:
    /**
     * @brief Application categories for classification
     */
    enum class Category {
        Uncategorized = 0,
        Game,
        Leisure,
        Work,
        Productivity,
        Social,
        Educational,
        Utility,
        System
    };
    
    /**
     * @brief Warning strategy for session limits
     */
    enum class WarningStrategy {
        Standard,    // 15, 10, 5 minute warnings
        Aggressive,  // 30, 20, 10, 5 minute warnings  
        Gentle,      // 5 minute warning only
        None         // No warnings
    };

    // Constructors
    Application();
    explicit Application(const QString& processName);
    Application(const QString& processName, Category category);
    
    // Core Properties
    QString getProcessName() const;
    QString getDisplayName() const;
    Category getCategory() const;
    void setCategory(Category category);
    void setDisplayName(const QString& displayName);
    
    // Statistics
    QDateTime getFirstSeen() const;
    QDateTime getLastSeen() const;
    int getTotalSessions() const;
    int getTotalMinutesUsed() const;
    float getAverageSessionLength() const;
    int getLongestSession() const;
    
    // Configuration
    int getCustomTimeLimit() const;  // -1 means use default
    void setCustomTimeLimit(int minutes);
    WarningStrategy getWarningStrategy() const;
    void setWarningStrategy(WarningStrategy strategy);
    
    // Business Rules
    bool shouldPromptForTime() const;
    bool requiresTermination() const;
    int getEffectiveTimeLimit() const;
    bool isFrequentlyUsed() const;
    bool isProductivityApp() const;
    
    // Session Tracking
    void recordSessionStart();
    void recordSessionEnd(int durationMinutes);
    void updateLastSeen();
    
    // Serialization
    QJsonObject toJson() const;
    static Application fromJson(const QJsonObject& json);
    
    // Utility
    static QString categoryToString(Category category);
    static Category categoryFromString(const QString& str);

private:
    // Core Identity
    QString m_processName;      // e.g., "chrome.exe"
    QString m_displayName;      // e.g., "Google Chrome"
    Category m_category;
    
    // Statistics
    QDateTime m_firstSeen;
    QDateTime m_lastSeen;
    int m_totalSessions;
    int m_totalMinutesUsed;
    int m_longestSession;       // in minutes
    
    // Configuration
    int m_customTimeLimit;      // -1 for default, otherwise minutes
    WarningStrategy m_warningStrategy;
    bool m_requiresPrompt;      // Some apps might not need prompting
    
    // Constants
    static constexpr int DEFAULT_GAME_TIME_LIMIT = 45;     // minutes
    static constexpr int DEFAULT_LEISURE_TIME_LIMIT = 30;  // minutes
    static constexpr int FREQUENT_USE_THRESHOLD = 10;      // sessions
};

#endif // APPLICATION_H
