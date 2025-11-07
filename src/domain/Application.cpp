#include "Application.h"
#include <QJsonDocument>
#include <QDebug>

// Constructors

Application::Application()
    : m_processName(""),
      m_displayName(""),
      m_category(Category::Uncategorized),
      m_firstSeen(QDateTime::currentDateTime()),
      m_lastSeen(QDateTime::currentDateTime()),
      m_totalSessions(0),
      m_totalMinutesUsed(0),
      m_longestSession(0),
      m_customTimeLimit(-1),
      m_warningStrategy(WarningStrategy::Standard),
      m_requiresPrompt(true)
{
}

Application::Application(const QString& processName)
    : m_processName(processName.toLower()),
      m_displayName(processName),
      m_category(Category::Uncategorized),
      m_firstSeen(QDateTime::currentDateTime()),
      m_lastSeen(QDateTime::currentDateTime()),
      m_totalSessions(0),
      m_totalMinutesUsed(0),
      m_longestSession(0),
      m_customTimeLimit(-1),
      m_warningStrategy(WarningStrategy::Standard),
      m_requiresPrompt(true)
{
}

Application::Application(const QString& processName, Category category)
    : m_processName(processName.toLower()),
      m_displayName(processName),
      m_category(category),
      m_firstSeen(QDateTime::currentDateTime()),
      m_lastSeen(QDateTime::currentDateTime()),
      m_totalSessions(0),
      m_totalMinutesUsed(0),
      m_longestSession(0),
      m_customTimeLimit(-1),
      m_warningStrategy(WarningStrategy::Standard),
      m_requiresPrompt(true)
{
}

// Core Properties

QString Application::getProcessName() const
{
    return m_processName;
}

QString Application::getDisplayName() const
{
    return m_displayName.isEmpty() ? m_processName : m_displayName;
}

Application::Category Application::getCategory() const
{
    return m_category;
}

void Application::setCategory(Category category)
{
    m_category = category;
    
    // Adjust defaults based on category
    if (category == Category::System || category == Category::Utility) {
        m_requiresPrompt = false;
    }
}

void Application::setDisplayName(const QString& displayName)
{
    m_displayName = displayName;
}

// Statistics

QDateTime Application::getFirstSeen() const
{
    return m_firstSeen;
}

QDateTime Application::getLastSeen() const
{
    return m_lastSeen;
}

int Application::getTotalSessions() const
{
    return m_totalSessions;
}

int Application::getTotalMinutesUsed() const
{
    return m_totalMinutesUsed;
}

float Application::getAverageSessionLength() const
{
    if (m_totalSessions == 0) {
        return 0.0f;
    }
    return static_cast<float>(m_totalMinutesUsed) / m_totalSessions;
}

int Application::getLongestSession() const
{
    return m_longestSession;
}

// Configuration

int Application::getCustomTimeLimit() const
{
    return m_customTimeLimit;
}

void Application::setCustomTimeLimit(int minutes)
{
    m_customTimeLimit = minutes;
}

Application::WarningStrategy Application::getWarningStrategy() const
{
    return m_warningStrategy;
}

void Application::setWarningStrategy(WarningStrategy strategy)
{
    m_warningStrategy = strategy;
}

// Business Rules

bool Application::shouldPromptForTime() const
{
    // System and utility apps don't need prompts
    if (!m_requiresPrompt) {
        return false;
    }
    
    // Games and leisure always prompt
    if (m_category == Category::Game || m_category == Category::Leisure) {
        return true;
    }
    
    // Work apps with custom limits prompt
    if (m_category == Category::Work && m_customTimeLimit > 0) {
        return true;
    }
    
    return false;
}

bool Application::requiresTermination() const
{
    // Only games and leisure apps are forcefully terminated
    return (m_category == Category::Game || m_category == Category::Leisure);
}

int Application::getEffectiveTimeLimit() const
{
    // Custom limit takes precedence
    if (m_customTimeLimit > 0) {
        return m_customTimeLimit;
    }
    
    // Category defaults
    switch (m_category) {
        case Category::Game:
            return DEFAULT_GAME_TIME_LIMIT;
        case Category::Leisure:
            return DEFAULT_LEISURE_TIME_LIMIT;
        default:
            return 60; // 1 hour default for other categories
    }
}

bool Application::isFrequentlyUsed() const
{
    return m_totalSessions >= FREQUENT_USE_THRESHOLD;
}

bool Application::isProductivityApp() const
{
    return (m_category == Category::Work || 
            m_category == Category::Productivity ||
            m_category == Category::Educational);
}

// Session Tracking

void Application::recordSessionStart()
{
    m_totalSessions++;
    updateLastSeen();
}

void Application::recordSessionEnd(int durationMinutes)
{
    m_totalMinutesUsed += durationMinutes;
    
    if (durationMinutes > m_longestSession) {
        m_longestSession = durationMinutes;
    }
    
    updateLastSeen();
}

void Application::updateLastSeen()
{
    m_lastSeen = QDateTime::currentDateTime();
}

// Serialization

QJsonObject Application::toJson() const
{
    QJsonObject json;
    json["processName"] = m_processName;
    json["displayName"] = m_displayName;
    json["category"] = categoryToString(m_category);
    json["firstSeen"] = m_firstSeen.toString(Qt::ISODate);
    json["lastSeen"] = m_lastSeen.toString(Qt::ISODate);
    json["totalSessions"] = m_totalSessions;
    json["totalMinutesUsed"] = m_totalMinutesUsed;
    json["longestSession"] = m_longestSession;
    json["customTimeLimit"] = m_customTimeLimit;
    json["warningStrategy"] = static_cast<int>(m_warningStrategy);
    json["requiresPrompt"] = m_requiresPrompt;
    return json;
}

Application Application::fromJson(const QJsonObject& json)
{
    Application app;
    app.m_processName = json["processName"].toString();
    app.m_displayName = json["displayName"].toString();
    app.m_category = categoryFromString(json["category"].toString());
    app.m_firstSeen = QDateTime::fromString(json["firstSeen"].toString(), Qt::ISODate);
    app.m_lastSeen = QDateTime::fromString(json["lastSeen"].toString(), Qt::ISODate);
    app.m_totalSessions = json["totalSessions"].toInt();
    app.m_totalMinutesUsed = json["totalMinutesUsed"].toInt();
    app.m_longestSession = json["longestSession"].toInt();
    app.m_customTimeLimit = json["customTimeLimit"].toInt(-1);
    app.m_warningStrategy = static_cast<WarningStrategy>(json["warningStrategy"].toInt());
    app.m_requiresPrompt = json["requiresPrompt"].toBool(true);
    return app;
}

// Utility

QString Application::categoryToString(Category category)
{
    switch (category) {
        case Category::Uncategorized: return "Uncategorized";
        case Category::Game: return "Game";
        case Category::Leisure: return "Leisure";
        case Category::Work: return "Work";
        case Category::Productivity: return "Productivity";
        case Category::Social: return "Social";
        case Category::Educational: return "Educational";
        case Category::Utility: return "Utility";
        case Category::System: return "System";
        default: return "Unknown";
    }
}

Application::Category Application::categoryFromString(const QString& str)
{
    if (str == "Game") return Category::Game;
    if (str == "Leisure") return Category::Leisure;
    if (str == "Work") return Category::Work;
    if (str == "Productivity") return Category::Productivity;
    if (str == "Social") return Category::Social;
    if (str == "Educational") return Category::Educational;
    if (str == "Utility") return Category::Utility;
    if (str == "System") return Category::System;
    return Category::Uncategorized;
}