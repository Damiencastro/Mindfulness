# Sequence Diagrams

This document illustrates key application workflows using sequence diagrams in Mermaid format.

## 1. Application Startup Sequence

This diagram shows the initialization process when the application is first launched. The `AppController` is responsible for creating all core components and starting the background monitoring thread.




```mermaid
sequenceDiagram
    participant User
    participant main()
    participant QApplication
    participant AppController
    participant GameList
    participant ProcessMonitor
    participant QThread
    participant GameSessionManager
    participant QSystemTrayIcon

    User-->main(): Executes Mindfulness.exe
    main()-->QApplication: new QApplication()
    main()-->AppController: new AppController()

    AppController-->GameList: new GameList()
    GameList-->GameList: load()
    GameList-->AppController: (Returns)

    AppController-->GameSessionManager: new GameSessionManager()
    AppController-->ProcessMonitor: new ProcessMonitor(gameList)
    AppController-->QThread: new QThread()
    AppController-->ProcessMonitor: moveToThread(thread)

    AppController-->QSystemTrayIcon: new QSystemTrayIcon()
    AppController-->QSystemTrayIcon: show()

    AppController-->AppController: connectSignals()
    AppController-->QThread: start()
    QThread-->ProcessMonitor: startMonitor() [Slot]

    main()-->QApplication: exec()
```


2. New Game Detection Sequence
This diagram shows the flow when the `ProcessMonitor` (running in its background thread) detects a process that is categorized as a "Game."

```mermaid
sequenceDiagram
    participant MonitorThread
    participant ProcessMonitor
    participant ProcessUtils
    participant GameList
    participant MainThread
    participant GameSessionManager
    participant GameSession
    participant TimeSetDialog

    loop 2-Second Timer
        MonitorThread->>ProcessMonitor: runMonitorLoop() [Slot]
        ProcessMonitor->>ProcessUtils: updateActiveProcessMap(map)
        ProcessMonitor->>GameList: isCategorized("csgo.exe")
        GameList-->>ProcessMonitor: (Returns true)
        ProcessMonitor->>GameList: isGame("csgo.exe")
        GameList-->>ProcessMonitor: (Returns true)
    end
    
    ProcessMonitor-->>MainThread: gameDetected(789, "csgo.exe") [Signal]
    
    MainThread->>GameSessionManager: onGameDetected(789, "csgo.exe") [Slot]
    GameSessionManager->>GameSession: new GameSession(7id, name)
    GameSessionManager->>GameSession: startSessionPrompt()
    
    GameSession->>TimeSetDialog: new TimeSetDialog()
    GameSession->>TimeSetDialog: exec() [Modal]
    
    TimeSetDialog-->>GameSession: timeSelected(30) [Signal]
    GameSession->>GameSession: onTimeSet(30) [Slot]
    GameSession->>GameSession: (Starts internal QTimer)
```

3. Uncategorized App Detection Sequence
This diagram shows the flow when the `ProcessMonitor` detects a new process that it does not recognize from the `GameList`.

```mermaid
sequenceDiagram
    participant MonitorThread
    participant ProcessMonitor
    participant ProcessUtils
    participant GameList
    participant MainThread
    participant AppController
    participant ConfigWindow

    loop 2-Second Timer
        MonitorThread->>ProcessMonitor: runMonitorLoop() [Slot]
        ProcessMonitor->>ProcessUtils: updateActiveProcessMap(map)
        ProcessMonitor->>GameList: isCategorized("new_app.exe")
        GameList-->>ProcessMonitor: (Returns false)
        ProcessMonitor->>ProcessMonitor: (Adds "new_app.exe" to flagged set)
    end
    
    ProcessMonitor-->>MainThread: uncategorizedAppDetected("new_app.exe") [Signal]
    
    MainThread->>AppController: onUncategorizedAppDetected("new_app.exe") [Slot]
    AppController->>AppController: onShowConfig()
    
    opt ConfigWindow is null
        AppController->>ConfigWindow: new ConfigWindow(gameList, processMonitor)
    end
    
    AppController->>ConfigWindow: highlightProcess("new_app.exe")
    AppController->>ConfigWindow: show()
    AppController->>ConfigWindow: activateWindow()
```

4. Game Session Lifecycle (Timer and Termination)


5. Early Game termination (User Quits Manually)