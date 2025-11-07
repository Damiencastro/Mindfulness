# Component Overview

This document defines the major logical components of the Mindfulness application, their architectural patterns, and their ownership boundaries. The architecture follows Domain-Driven Design (DDD) principles with clear separation between repositories, services, managers, and UI components.

## 1. High-Level Architectural Summary

### What it is:
This product serves as a utility to assist users in being mindful and deliberate about how they spend their time. It monitors the user's activity and detects when the user is playing games or other leisure activities. They are prompted to decide how long they wish to engage in the game/leisure activity, and are reminded at 15/10/5 minute intervals before terminating the program when their time elapses.

### Why:
This product is intended to serve as voluntary guidelines on how the user spends their time. The goal is to ensure the user is deliberate and mindful with how they spend their time.

## 2. Architectural Principles

### Single Source of Truth (SSOT)
Each piece of data has exactly one authoritative source. `ApplicationRepository` is the only source for application data and categorizations; `ProcessMonitor` is the only source for live processes. This prevents conflicting states and minimizes memory footprint.

### Event-Driven with Signals/Slots
The application uses Qt's Signal and Slots mechanism for loose coupling. Components emit signals without knowing their receivers, with `AppController` serving as the mediator that wires connections.

### Domain-Driven Design (DDD)
The system separates domain entities (`Application`, `GameSession`) from infrastructure concerns (repositories, services). Domain entities encapsulate business logic and behavior, not just data.


## The Key Design Principle

**"Query down, notify up"**
- Query dependencies (repositories, state holders) with direct calls
- Notify consumers (managers, UI) with signals

This creates a clean flow:
```
ProcessMonitor (emit) → Dispatcher (query+emit) → Managers (handle)
                              ↓
                        Repository (answers)
```

### Layered Architecture
The system follows a clear layered architecture:
- **Application Layer**: Top-level orchestration (`AppController`)
- **Service Layer**: Stateless workers that do two types of things:
  - **Infrastructure Services**: Interact with external systems (OS, Network, etc.)
  - **Application Services**: Orchestrate workflows, translate between layers, & apply business rules
- **Manager Layer**: Business logic orchestration (`GameSessionManager`, `CategorizationManager`)
  - Stateful orchestrators that manage lifecycle and state of related objects
- **Domain Layer**: Core business entities (`Application`, `GameSession`)
  - Objects that represent core business concepts with both data & behavior
- **Repository Layer**: Data persistence (`ApplicationRepository`)
  - Abstractions over persistence - the "file cabinet"
- **Presentation Layer**: UI components (dialogs and windows)

### Dependency Injection
Components receive dependencies through constructors, making dependencies explicit and enabling testing. For example, `ProcessMonitor` receives `ApplicationRepository*`, not creating its own instance.

### Threading Model
Background monitoring runs on a dedicated worker thread to maintain UI responsiveness. See `/architecture/threading-model.md` for details.

## 3. Component Patterns & Grouping
```
APPLICATION LAYER (Mediator Pattern)
└── AppController

SERVICE LAYER (Service Pattern)
├── ProcessMonitor (Infrastructure Service)
├── ProcessEventDispatcher (Application Service)
└── ProcessUtils (Utility Service)

MANAGER LAYER (Factory + Manager Pattern)
├── GameSessionManager
└── CategorizationManager

DOMAIN LAYER (Entity Pattern)
├── Application
└── GameSession

REPOSITORY LAYER (Repository Pattern)
└── ApplicationRepository

PRESENTATION LAYER (MVC View Pattern)
├── ConfigWindow
├── CategorizeDialog
├── TimeSetDialog
└── WarningDialog


```

## 4. Component Specifications




### Application Layer (Mediator Pattern)

#### `AppController`
* **Architectural Pattern:** Mediator
* **Purpose:** Central orchestrator that creates components and wires their communications. Does NOT handle business logic directly.
* **SSOT / Data Ownership:**
    * **Owns (creates and deletes):**
        * `ApplicationRepository` (repository - manual delete)
        * `ProcessMonitor` (service - via `deleteLater()`)
        * `ProcessEventDispatcher` (sevice - via `deleteLater()`)
        * `GameSessionManager` (manager - QObject child)
        * `CategorizationManager` (manager - QObject child)
        * `QSystemTrayIcon` (system integration)
* **Key Responsibilities:**
    * Component initialization in correct order (repositories → services → managers)
    * Signal/slot connection wiring
    * Application lifecycle management
    * System tray integration
* **Signal/Slot Connections:**
    * **Connects:** `ProcessMonitor::processStarted` → `ProcessEventDispatcher::onProcessStarted`
    * **Connects:** `ProcessMonitor::processTerminated` → `ProcessEventDispatcher::onProcessTerminated`
    * **Listens To:** `QSystemTrayIcon::activated` → `onTrayIconActivated()`
    * **Listens To:** System quit requests → `onQuit()`
* **Threading:** Main/GUI Thread





### Service Layer (Service Pattern)

#### `ProcessMonitor`
* **Architectural Pattern:** Service (Infrastructure)
* **Purpose:** Single Source of Truth for currently running processes. Provides continuous monitoring service.
* **SSOT / Data Ownership:**
    * **Owns:** `m_activeProcessMap` (QHash<DWORD, QString> - live processes)
    * **Owns:** `m_knownRunningPIDs` (QSet<DWORD> - tracking state)
* **Dependencies:** 
    * `ProcessUtils` (for system calls)
* **Signals:**
    * `processStarted(DWORD pid, QString& name)`
    * `processTerminated(DWORD pid)`
* **Threading:** Dedicated Worker Thread

#### `ProcessEventDispatcher`
* **Architectural Pattern:** Domain Event Translator / Mediator
* **Purpose:** Translate low-level infrastructure events (process started/stopped) into high-level domain events (game detected, uncategorized app found)
* **SSOT / Data Ownership:** 
    * **Owns:** Nothing - purely stateless translator
* **Dependencies (Injected):**
    * `ApplicationRepository*` - For identifying known applications (query only)
    * `CategorizationManager*` - For checking pending categorizations (query only)
* **Responsibilities:**
    * Receive raw process lifecycle events
    * Query repository to identify applications
    * Translate into appropriate domain events
    * Route events based on business rules
* **Slots:**
    * `onProcessStarted(DWORD pid, QString name)` - From ProcessMonitor
    * `onProcessTerminated(DWORD pid)` - From ProcessMonitor
* **Signals:**
    * `gameDetected(DWORD pid, QString name, Application* app)` - When known game starts
    * `workApplicationDetected(DWORD pid, QString name, Application* app)` - When work app starts
    * `uncategorizedAppDetected(QString name)` - When unknown app detected
    * `applicationTerminated(DWORD pid, Category category)` - When any app closes
* **Business Rules:**
    * Only signal uncategorizedAppDetected once per application name
    * Don't signal for already-pending categorizations
    * Route by category (Game → GameSessionManager, etc.)
* **Threading:** Main Thread (for repository access)

#### `ProcessUtils` (Namespace)
* **Architectural Pattern:** Utility/Helper
* **Purpose:** Stateless Windows API wrapper functions
* **Interface:**
    * `bool terminateProcess(DWORD pid)`
    * `QHash<DWORD,QString> getActiveProcesses()`
    * `void updateActiveProcessMap(QHash&)`
* **Threading:** Thread-safe (stateless)




### Manager Layer (Factory + Manager Patterns)

#### `GameSessionManager`
* **Architectural Pattern:** Managing Factory
* **Purpose:** Creates and manages lifecycle of all `GameSession` objects
* **SSOT / Data Ownership:**
    * **Owns:** `m_activeSessions` (QList<GameSession*>)
* **Dependencies:**
    * `ApplicationRepository*` (to update statistics)
* **Responsibilities:**
    * Factory: Creates configured `GameSession` instances with associated `Application`
    * Manager: Tracks active sessions
    * Lifecycle: Handles session cleanup
    * Statistics: Ensures application statistics are updated
* **Slots:**
    * `onGameDetected(DWORD, QString, Application*)` - Creates new session
    * `onProcessTerminated(DWORD)` - Handles early termination
    * `onSessionFinished()` - Cleanup and statistics update
* **Threading:** Main/GUI Thread

#### `CategorizationManager`
* **Architectural Pattern:** Managing Factory
* **Purpose:** Orchestrates the categorization workflow for uncategorized applications
* **SSOT / Data Ownership:**
    * **Owns:** `m_categorizationDialog` (created on-demand)
    * **Owns:** `m_pendingCategorizations` (QSet<QString>)
* **Dependencies:** 
    * `ApplicationRepository*` (for creating and saving new Applications)
* **Responsibilities:**
    * Creates and manages `CategorizeDialog`
    * Creates new `Application` entities for uncategorized processes
    * Tracks pending categorizations
    * Coordinates categorization workflow
* **Slots:**
    * `onUncategorizedAppDetected(QString)` - Creates Application and triggers workflow
* **Signals:**
    * `categorizationComplete(QString, Application*)` - Notifies with new entity
* **Threading:** Main/GUI Thread





### Domain Layer (Entity Pattern)

#### `Application`
* **Architectural Pattern:** Domain Entity
* **Purpose:** Rich domain object representing a monitored application with its configuration, categorization, and statistics
* **SSOT / Data Ownership:**
    * **Owns:** Application state (name, category, statistics, custom settings)
* **Core Properties:**
    * Process name and display name
    * Category (Game, Work, Social, Utility, etc.)
    * Statistics (first seen, last used, total sessions, total time)
    * Configuration (custom time limits, warning strategy)
    * Icon path (for future UI enhancement)
* **Domain Methods:**
    * `bool shouldPromptForTime()` - Business rule
    * `int getEffectiveTimeLimit()` - Returns app-specific or default limit
    * `void recordSessionStart()` - Update statistics
    * `void recordSessionEnd(int duration)` - Update statistics
    * `float getAverageSessionLength()` - Calculate metrics
* **Used By:** `ApplicationRepository` (persistence), `ProcessMonitor` (categorization checks), UI components (display)

#### `GameSession`
* **Architectural Pattern:** Domain Entity
* **Purpose:** Represents and manages a single gaming/leisure session's complete lifecycle
* **SSOT / Data Ownership:**
    * **Owns:** Session state (time remaining, warnings shown)
    * **Owns:** Internal timers (countdown, warning)
    * **Owns:** Reference to associated `Application` entity
* **Responsibilities:**
    * Display initial time-setting dialog (using app's custom limits if available)
    * Manage countdown timer
    * Display warnings at intervals
    * Handle time extensions
    * Update application statistics on completion
    * Terminate process when time expires
* **Dependencies:** 
    * `Application*` (for configuration and statistics)
    * `ProcessUtils` (for termination)
* **Signals:**
    * `sessionFinished()` - Notifies manager of completion
    * `sessionStatistics(int duration)` - Reports session data
* **Threading:** Main/GUI Thread






### Repository Layer (Repository Pattern)

#### `ApplicationRepository`
* **Architectural Pattern:** Repository
* **Purpose:** Single Source of Truth for all application data. Abstracts persistence and provides rich querying interface.
* **SSOT / Data Ownership:**
    * **Owns:** Collection of `Application*` entities
    * **Owns:** Exclusive access to `applications.json` file
* **Interface:**
    * `Application* find(QString processName)` - Find by exact name
    * `Application* findOrCreate(QString processName)` - Get or create new
    * `QList<Application*> findByCategory(Category)` - Query by category
    * `QList<Application*> findAll()` - Get all applications
    * `void save(Application*)` - Persist single entity
    * `void saveAll()` - Persist all changes to disk
    * `void load()` - Load from disk on startup
* **Implementation Details:**
    * Current: JSON file storage (`applications.json`)
    * Future: SQLite database (same interface)
* **Threading:** NOT thread-safe. Read from worker thread, written only from main thread.
* **Used By:** `ProcessMonitor` (read), `CategorizationManager` (read/write), `ConfigWindow` (read/write), `GameSession` (write for statistics)






### Presentation Layer (View Pattern)

#### `ConfigWindow`
* **Architectural Pattern:** View/Controller (MVC)
* **Purpose:** Main configuration interface for viewing and editing application settings and categorizations
* **Dependencies:** 
    * `ApplicationRepository*` (data source/sink)
    * `ProcessMonitor*` (live process data)
* **Responsibilities:**
    * Display applications grouped by category
    * Show application statistics and custom settings
    * Display running status with application icons
    * Allow category changes and custom limit settings
    * Save changes to repository
* **Threading:** Main/GUI Thread

#### `CategorizeDialog`
* **Architectural Pattern:** View/Controller (MVC)
* **Purpose:** Dialog to facilitate user-categorization of newly found application
* **Dependencies:** 
    * `ApplicationRepository*` (to save new Applications)
    * `ProcessMonitor*` (to get uncategorized apps)
* **Responsibilities:**
    * Display only uncategorized running processes
    * Provide quick categorization interface
    * Create new `Application` entities with initial category
    * Save and close on completion
* **Threading:** Main/GUI Thread

#### `TimeSetDialog`
* **Architectural Pattern:** View (Simple Dialog)
* **Purpose:** Modal dialog for setting game session time limit
* **Parent:** Created by `GameSession`
* **Enhancement:** Shows application's custom limit as default if available
* **Returns:** Selected minutes (int)
* **Threading:** Main/GUI Thread

#### `WarningDialog`
* **Architectural Pattern:** View (Simple Dialog)
* **Purpose:** Modal warning dialog with extension option
* **Parent:** Created by `GameSession`
* **Enhancement:** Can show application name and icon
* **Returns:** User choice (OK/Extend enum)
* **Threading:** Main/GUI Thread

## 5. Initialization Order

The `AppController` initializes components in this specific order to respect dependencies:
```cpp
1. Repository Layer
   - ApplicationRepository (loads from disk)

2. Service Layer
   - ProcessMonitor (needs repository reference)

3. Manager Layer
   - GameSessionManager (needs repository)
   - CategorizationManager (needs repository)

4. Wire Connections
   - Connect all signals/slots

5. Start Services
   - Move ProcessMonitor to thread
   - Start monitoring
```

## 6. Data Flow Examples

### New Application Detection
```
ProcessMonitor detects "newapp.exe"
→ Checks ApplicationRepository::find("newapp.exe") 
→ Not found
→ Emits uncategorizedAppDetected("newapp.exe")
→ CategorizationManager receives signal
→ Creates new Application("newapp.exe")
→ Shows CategorizeDialog
→ User categorizes as "Game"
→ ApplicationRepository::save(application)
→ Application now tracked with statistics
```

### Game Session with Statistics
```
ProcessMonitor detects "game.exe"
→ ApplicationRepository::find("game.exe") returns Application*
→ Emits gameDetected(pid, name, application)
→ GameSessionManager creates GameSession(application)
→ Session uses application->getEffectiveTimeLimit()
→ On session end, calls application->recordSessionEnd(duration)
→ ApplicationRepository::save(application) persists statistics
```

## 7. Memory Management

* **Domain Entities:** `Application` objects owned by `ApplicationRepository`
* **QObject Hierarchy:** Components with QObject parents are automatically deleted
* **Thread Objects:** Use `deleteLater()` for thread-safe deletion
* **Non-QObjects:** Manual deletion required (e.g., ApplicationRepository)
* **Dialogs:** Use `WA_DeleteOnClose` attribute for automatic cleanup

## 8. Future Enhancements Enabled

The `Application` entity and `ApplicationRepository` pattern enables:
* **Statistics Tracking:** Sessions per day, total time, patterns
* **Custom Limits:** Per-application time limits
* **Categories Beyond Binary:** Work, Social, Creative, Educational
* **Icon Support:** Visual identification in UI
* **Export/Import:** User data portability
* **Database Migration:** Simple interface swap when needed