# ProcessEventDispatcher API Contract

## Component Overview
**Type:** Application Service  
**Pattern:** Domain Event Translator  
**Thread:** Main Thread  
**Purpose:** Translate infrastructure process events into domain-specific business events

## Responsibilities
- Receive raw process lifecycle events from ProcessMonitor
- Query ApplicationRepository to identify known applications
- Check with CategorizationManager for pending categorizations
- Emit appropriate domain events based on business rules
- Maintain no state (purely functional translator)

## What This Component Does NOT Do
- ❌ No data storage or caching
- ❌ No direct process monitoring
- ❌ No UI interaction
- ❌ No entity creation or modification
- ❌ No workflow management (just event translation)

## Public Interface

### Constructor
```cpp
ProcessEventDispatcher(ApplicationRepository* appRepo,
                      CategorizationManager* catManager,
                      QObject* parent = nullptr)
```
- **Parameters:**
  - `appRepo`: Repository for querying application data (not null)
  - `catManager`: Manager for checking pending categorizations (not null)
  - `parent`: Standard QObject parent
- **Preconditions:** Both dependencies must be valid pointers
- **Postconditions:** Ready to receive and translate events

### Public Slots

#### `onProcessStarted`
```cpp
void onProcessStarted(DWORD pid, const QString& processName)
```
- **Purpose:** Handle notification of a new process starting
- **Source:** ProcessMonitor::processStarted signal
- **Business Logic:**
  1. Query ApplicationRepository for the process
  2. If found: Emit categorized event based on application type
  3. If not found: Check if already pending categorization
  4. If not pending: Emit uncategorizedAppDetected
- **Thread Safety:** Must run on main thread (repository access)

#### `onProcessTerminated`
```cpp
void onProcessTerminated(DWORD pid)
```
- **Purpose:** Handle notification of process termination
- **Source:** ProcessMonitor::processTerminated signal
- **Business Logic:**
  1. Emit applicationTerminated with PID
  2. Let listeners handle cleanup based on PID
- **Thread Safety:** Must run on main thread

### Signals

#### `gameDetected`
```cpp
void gameDetected(DWORD pid, const QString& processName, Application* app)
```
- **Emitted When:** A known game or leisure application starts
- **Parameters:**
  - `pid`: Windows Process ID
  - `processName`: Executable name
  - `app`: Pointer to Application entity (never null)
- **Consumers:** GameSessionManager
- **Business Rule:** Only for Category::Game or Category::Leisure

#### `workApplicationDetected`
```cpp
void workApplicationDetected(DWORD pid, const QString& processName, Application* app)
```
- **Emitted When:** A known work/productivity application starts
- **Parameters:** Same as gameDetected
- **Consumers:** Future: ProductivityTracker
- **Business Rule:** Only for Category::Work

#### `uncategorizedAppDetected`
```cpp
void uncategorizedAppDetected(const QString& processName)
```
- **Emitted When:** Unknown application detected for first time
- **Parameters:**
  - `processName`: Executable name of uncategorized app
- **Consumers:** CategorizationManager
- **Business Rule:** Only if not already pending categorization

#### `applicationTerminated`
```cpp
void applicationTerminated(DWORD pid)
```
- **Emitted When:** Any tracked process ends
- **Parameters:**
  - `pid`: Process ID of terminated application
- **Consumers:** GameSessionManager, future managers
- **Note:** Consumers must track their own PID→Session mappings

## Dependencies

### Required Dependencies (Constructor Injected)
- `ApplicationRepository*` - For application lookups
- `CategorizationManager*` - For pending categorization checks

### No Runtime Dependencies
- Stateless service - no owned objects

## Usage Example

```cpp
// In AppController
m_processEventDispatcher = new ProcessEventDispatcher(
    m_appRepository,
    m_categorizationManager,
    this
);

// Wire infrastructure to translator
connect(m_processMonitor, &ProcessMonitor::processStarted,
        m_processEventDispatcher, &ProcessEventDispatcher::onProcessStarted);

// Wire translator to domain
connect(m_processEventDispatcher, &ProcessEventDispatcher::gameDetected,
        m_gameSessionManager, &GameSessionManager::onGameDetected);
```

## Business Rules

1. **One Signal Per Process Start:** Each PID triggers exactly one domain event
2. **Category Routing:** 
   - Game/Leisure → gameDetected
   - Work → workApplicationDetected
   - Unknown → uncategorizedAppDetected (once only)
3. **Pending Check:** Never emit uncategorizedAppDetected if already being categorized
4. **Null Safety:** Never emit signals with null Application pointers

## Error Handling

|         Scenario        |            Response             |
|-------------------------|---------------------------------|
| Repository returns null |     Check for uncategorized     |
| Category not recognized |      Log warning, no signal     |
| Null dependency passed  | Assert in debug, log in release |

## Performance Characteristics
- **Processing Time:** UNMEASURED
- **Memory:** Minimal - no state storage
- **Frequency:** Matches ProcessMonitor (0-50 events/minute)

## Testing Strategy

```cpp
class TestProcessEventDispatcher : public QObject {
    void test_game_detection() {
        MockRepository repo;
        MockCategorizationManager catMgr;
        ProcessEventDispatcher dispatcher(&repo, &catMgr);
        
        Application gameApp("game.exe", Category::Game);
        repo.setReturnValue(&gameApp);
        
        QSignalSpy spy(&dispatcher, &ProcessEventDispatcher::gameDetected);
        dispatcher.onProcessStarted(123, "game.exe");
        
        QCOMPARE(spy.count(), 1);
    }
};
```