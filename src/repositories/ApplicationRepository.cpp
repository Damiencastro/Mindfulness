#include "ApplicationRepository.h"
#include "Application.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>
#include <QDir>

// Constructors

ApplicationRepository::ApplicationRepository()
    : m_dataPath(DEFAULT_DATA_FILE),
      m_isDirty(false)
{
    load();
}

ApplicationRepository::ApplicationRepository(const QString& dataPath)
    : m_dataPath(dataPath.isEmpty() ? DEFAULT_DATA_FILE : dataPath),
      m_isDirty(false)
{
    load();
}

ApplicationRepository::~ApplicationRepository()
{
    // Auto-save on destruction if there are unsaved changes
    if (m_isDirty) {
        saveAll();
    }
}

// Core CRUD Operations

Application* ApplicationRepository::find(const QString& processName) const
{
    QString normalized = normalizeProcessName(processName);
    auto it = m_applications.find(normalized);
    
    if (it != m_applications.end()) {
        return it->get();
    }
    
    return nullptr;
}

Application* ApplicationRepository::findOrCreate(const QString& processName)
{
    Application* existing = find(processName);
    if (existing) {
        return existing;
    }
    
    // Create new application
    QString normalized = normalizeProcessName(processName);
    auto app = std::make_unique<Application>(processName);
    Application* rawPtr = app.get();
    
    m_applications[normalized] = std::move(app);
    m_isDirty = true;
    
    qDebug() << "Created new application:" << processName;
    return rawPtr;
}

void ApplicationRepository::save(Application* app)
{
    if (!app) {
        qWarning() << "Cannot save null application";
        return;
    }
    
    QString normalized = normalizeProcessName(app->getProcessName());
    
    // Check if we already have this application
    auto it = m_applications.find(normalized);
    if (it != m_applications.end()) {
        // Update existing
        if (it->get() != app) {
            // Replace with new instance
            m_applications[normalized] = std::make_unique<Application>(*app);
        }
    } else {
        // Add new
        m_applications[normalized] = std::make_unique<Application>(*app);
    }
    
    m_isDirty = true;
}

bool ApplicationRepository::remove(const QString& processName)
{
    QString normalized = normalizeProcessName(processName);
    
    if (m_applications.remove(normalized) > 0) {
        m_isDirty = true;
        qDebug() << "Removed application:" << processName;
        return true;
    }
    
    return false;
}

// Query Methods

QList<Application*> ApplicationRepository::findAll() const
{
    QList<Application*> result;
    result.reserve(m_applications.size());
    
    for (const auto& pair : m_applications) {
        result.append(pair.get());
    }
    
    return result;
}

QList<Application*> ApplicationRepository::findByCategory(Application::Category category) const
{
    QList<Application*> result;
    
    for (const auto& pair : m_applications) {
        if (pair->getCategory() == category) {
            result.append(pair.get());
        }
    }
    
    return result;
}

int ApplicationRepository::count() const
{
    return m_applications.size();
}

bool ApplicationRepository::exists(const QString& processName) const
{
    QString normalized = normalizeProcessName(processName);
    return m_applications.contains(normalized);
}

// Persistence Operations

bool ApplicationRepository::saveAll()
{
    QJsonObject root;
    root["version"] = FILE_VERSION;
    root["lastModified"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonArray appsArray;
    for (const auto& pair : m_applications) {
        appsArray.append(pair->toJson());
    }
    root["applications"] = appsArray;
    
    QJsonDocument doc(root);
    
    QFile file(m_dataPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << m_dataPath;
        qWarning() << "Error:" << file.errorString();
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    m_isDirty = false;
    qDebug() << "Saved" << m_applications.size() << "applications to" << m_dataPath;
    return true;
}

bool ApplicationRepository::load()
{
    QFile file(m_dataPath);
    
    // If file doesn't exist, that's okay for first run
    if (!file.exists()) {
        qDebug() << "Data file does not exist, starting with empty repository:" << m_dataPath;
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << m_dataPath;
        qWarning() << "Error:" << file.errorString();
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON in data file:" << m_dataPath;
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Check version for future compatibility
    int version = root["version"].toInt(1);
    if (version > FILE_VERSION) {
        qWarning() << "Data file version" << version << "is newer than supported version" << FILE_VERSION;
    }
    
    // Clear existing data
    m_applications.clear();
    
    // Load applications
    QJsonArray appsArray = root["applications"].toArray();
    for (const QJsonValue& value : appsArray) {
        if (value.isObject()) {
            Application app = Application::fromJson(value.toObject());
            QString normalized = normalizeProcessName(app.getProcessName());
            m_applications[normalized] = std::make_unique<Application>(std::move(app));
        }
    }
    
    m_isDirty = false;
    qDebug() << "Loaded" << m_applications.size() << "applications from" << m_dataPath;
    return true;
}

void ApplicationRepository::clear()
{
    m_applications.clear();
    m_isDirty = true;
}

// Statistics Queries

QList<Application*> ApplicationRepository::findRecentlyUsed(int days) const
{
    QList<Application*> result;
    QDateTime cutoff = QDateTime::currentDateTime().addDays(-days);
    
    for (const auto& pair : m_applications) {
        if (pair->getLastSeen() >= cutoff) {
            result.append(pair.get());
        }
    }
    
    // Sort by last seen (most recent first)
    std::sort(result.begin(), result.end(), 
              [](const Application* a, const Application* b) {
                  return a->getLastSeen() > b->getLastSeen();
              });
    
    return result;
}

QList<Application*> ApplicationRepository::findFrequentlyUsed(int minSessions) const
{
    QList<Application*> result;
    
    for (const auto& pair : m_applications) {
        if (pair->getTotalSessions() >= minSessions) {
            result.append(pair.get());
        }
    }
    
    // Sort by total sessions (most used first)
    std::sort(result.begin(), result.end(),
              [](const Application* a, const Application* b) {
                  return a->getTotalSessions() > b->getTotalSessions();
              });
    
    return result;
}

// Helper Methods

QString ApplicationRepository::normalizeProcessName(const QString& processName) const
{
    return processName.toLower();
}

QJsonObject ApplicationRepository::toJson() const
{
    QJsonObject root;
    root["version"] = FILE_VERSION;
    root["lastModified"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonArray appsArray;
    for (const auto& pair : m_applications) {
        appsArray.append(pair->toJson());
    }
    root["applications"] = appsArray;
    
    return root;
}

void ApplicationRepository::fromJson(const QJsonObject& json)
{
    m_applications.clear();
    
    QJsonArray appsArray = json["applications"].toArray();
    for (const QJsonValue& value : appsArray) {
        if (value.isObject()) {
            Application app = Application::fromJson(value.toObject());
            QString normalized = normalizeProcessName(app.getProcessName());
            m_applications[normalized] = std::make_unique<Application>(std::move(app));
        }
    }
    
    m_isDirty = false;
}