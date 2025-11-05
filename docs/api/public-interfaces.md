# Public API Interfaces

This document defines the public-facing methods (including slots) and signals for the core components of the Mindfulness application. This API serves as the contract for inter-component communication.

# `GameList` Class
## Purpose: The single source of truth for loading, saving, and querying application categorizations.

## Public Methods
* bool isGame(const QString& processName) const
    * Description: Returns true if the process name is known and categorized as a "Game/Leisure" app. Returns false otherwise (including if the app is unknown).
    * Used By: ProcessMonitor
* bool isCategorized(const QString& processName) const
    * Description: Returns true if the process name exists in the master list, regardless of its categorization (game or not game).
    * Used By: ProcessMonitorQHash
* <QString, bool> getAllCategorizations() const
    * Description: Returns a copy of the entire in-memory map of process names to their "isGame" flag.
    * Used By: ConfigWindow
* void setCategorization(const QString& processName, bool isGame)
    * Description: Sets or updates the in-memory categorization for a given process name. This does not automatically save to disk.
    * Used By: ConfigWindow, CategorizeDialog
* void save()
    * Description: Manually triggers a save of the entire in-memory map to the games.txt file on disk.
    * Used By: ConfigWindow, CategorizeDialog
    
# `ProcessMonitor` Class

## Purpose: The single source of truth for the list of currently running processes. Runs in a background thread and emits signals based on its findings.

## Public Slots
These methods are designed to be called from other threads (specifically, the main AppController thread).
* void startMonitor()
    * Description: Starts the internal QTimer that drives the runMonitorLoop(). This is called by AppController after the QThread is started.
* void stopMonitor()
    * Description: Stops the internal QTimer, effectively pausing the monitoring loop. This is called by AppController during shutdown.
    
## Public Methods

* const QHash<DWORD, QString>& getActiveProcessMap() const
    * Description: Returns a const reference to the monitor's internal, persistent map of active processes (PID -> Name). This is a high-performance getter that avoids copying the map.
    * Used By: ConfigWindow, CategorizeDialog
    
## Signals
These signals are emitted by the ProcessMonitor (in its thread) and are connected to slots in other components (on the main thread).
* void gameDetected(DWORD pid, const QString& processName)
    * Description: Emitted when a newly launched process is found, and GameList identifies it as a known "Game/Leisure" app.
    * Connected To: GameSessionManager::onGameDetected
    
* void uncategorizedAppFound(const QString& processName)
    * Description: Emitted when a newly launched process is found that does not exist in the GameList. This signal is only emitted once per application name until the app is categorized.
    * Connected To: AppController::onUncategorizedAppFound