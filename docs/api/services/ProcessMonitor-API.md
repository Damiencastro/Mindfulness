# ProcessMonitor API Contract

## Component Overview
**Type:** Infrastructure Service  
**Pattern:** Observer/Monitor  
**Thread:** Worker Thread  
**Purpose:** Continuously monitor Windows processes and report lifecycle changes

## Responsibilities
- Poll Windows API for running processes every 2 seconds
- Detect when new processes start
- Detect when processes terminate
- Maintain internal state for differential comparison
- Emit signals for process lifecycle events

## What This Component Does NOT Do
- ❌ No business logic about application categories
- ❌ No database/repository access
- ❌ No UI interaction
- ❌ No decision-making about what constitutes a "game"
- ❌ No file I/O

## Public Interface

### Constructor
```cpp
explicit ProcessMonitor(QObject* parent = nullptr)
```
- **Parameters:** Standard QObject parent (should be nullptr for thread moving)
- **Postconditions:** Timer created but not started

### Public Slots

#### `startMonitor()`
```cpp
void startMonitor()
```
- **Purpose:** Begin the monitoring loop
- **Preconditions:** Object has been moved to worker thread
- **Postconditions:** Internal timer starts with 2-second interval
- **Thread Safety:** Must be called from the worker thread
- **Typical Caller:** Connected to QThread::started signal

#### `stopMonitor()`
```cpp
void stopMonitor()
```
- **Purpose:** Stop the monitoring loop
- **Preconditions:** Monitor is running
- **Postconditions:** Timer stopped, no more signals emitted
- **Thread Safety:** Thread-safe via queued connection
- **Typical Caller:** AppController during shutdown

### Signals

#### `processStarted`
```cpp
void processStarted(DWORD pid, const QString& processName)
```
- **Emitted When:** A process is detected that wasn't running in the previous scan
- **Parameters:**
  - `pid`: Windows Process ID (unique identifier)
  - `processName`: Executable name (e.g., "chrome.exe"), lowercase normalized
- **Frequency:** Varies, typically 0-10 per minute
- **Thread Context:** Emitted from worker thread
- **Guarantees:** 
  - Each PID is signaled exactly once per process lifetime
  - Process name is never empty

#### `processTerminated`
```cpp
void processTerminated(DWORD pid)
```
- **Emitted When:** A previously detected process is no longer running
- **Parameters:**
  - `pid`: Windows Process ID of the terminated process
- **Frequency:** Varies, typically 0-50 per minute
- **Thread Context:** Emitted from worker thread
- **Guarantees:**
  - Only emitted for PIDs previously announced via processStarted
  - PID can be reused by Windows after termination

### Private Members

#### Data Members
```cpp
private:
    QTimer* m_monitorTimer;                    // Drives the polling loop
    QHash<DWORD, QString> m_activeProcessMap;  // Current snapshot
    QSet<DWORD> m_knownRunningPIDs;           // Previous snapshot PIDs
```

#### Private Methods
```cpp
private slots:
    void runMonitorLoop();  // Called by timer, performs scan and comparison
```

## Dependencies

### Compile-Time Dependencies
- `ProcessUtils` namespace (for Windows API calls)
- `<windows.h>` for DWORD type
- `<QTimer>`, `<QHash>`, `<QSet>` for data structures

### Runtime Dependencies
- None - this is a leaf component

### Injected Dependencies
- None - pure infrastructure service

## Usage Example

```cpp
// In AppController
QThread* monitorThread = new QThread(this);
ProcessMonitor* monitor = new ProcessMonitor(); // No parent!

monitor->moveToThread(monitorThread);

// Connect lifecycle
connect(monitorThread, &QThread::started, 
        monitor, &ProcessMonitor::startMonitor);
connect(monitorThread, &QThread::finished,
        monitor, &QObject::deleteLater);

// Connect to domain event dispatcher
connect(monitor, &ProcessMonitor::processStarted,
        dispatcher, &ProcessEventDispatcher::onProcessStarted);
connect(monitor, &ProcessMonitor::processTerminated,
        dispatcher, &ProcessEventDispatcher::onProcessTerminated);

monitorThread->start();
```

## State Diagram

```
    ┌─────────────┐
    │  Created    │
    └─────┬───────┘
          │ startMonitor()
    ┌─────▼───────┐
    │  Running    │◄────┐
    │             │     │ Timer (2s)
    │ - Poll API  ├─────┘
    │ - Compare   │
    │ - Emit      │
    └─────┬───────┘
          │ stopMonitor()
    ┌─────▼───────┐
    │  Stopped    │
    └─────────────┘
```

## Error Handling

| Error Condition | Response | Recovery |
|----------------|----------|----------|
| EnumProcesses fails | Log warning, skip cycle | Retry next timer tick |
| OpenProcess fails | Skip that specific process | Continue with others |
| Timer already running | Ignore startMonitor() | No action needed |

## Performance Characteristics

- **Memory Usage:** UNMEASURED
- **CPU Usage:** UNMEASURED
- **Scan Duration:** UNMEASURED
- **Scan Frequency:** Every 2000ms
- **Typical Process Count:** UNMEASURED

## Thread Safety

- **Thread-Safe Operations:**
  - Signal emission (Qt handles queuing)
  - Destruction via deleteLater()
  
- **Non-Thread-Safe Operations:**
  - Direct method calls from other threads
  - Accessing m_activeProcessMap from outside

## Testing Considerations

```cpp
class TestableProcessMonitor : public ProcessMonitor {
    // Expose internals for testing
public:
    void injectProcessList(const QHash<DWORD, QString>& processes);
    QSet<DWORD> getKnownPIDs() const { return m_knownRunningPIDs; }
};
```

## Future Considerations

- Could add process CPU/memory stats without changing interface
- Could add filtering (ignore system processes) via configuration
- Could make scan interval configurable
- Interface remains stable regardless of implementation changes
```