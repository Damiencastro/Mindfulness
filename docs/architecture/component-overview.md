# Component Overview

This document defines the major logical components of the Mindfulness application, their single responsibilities, and their ownership boundaries. The architecture is designed around the "Single Source of Truth" (SSOT) principle.

## 1. High-Level Architectural Summary

### What it is:
This product will serve as a utility to assist users in being mindful and deliberate about how they spend their time. It will monitor the user's activity and detect any time the user is playing games or other such leisure activities. They will be prompted to decide on how long they wish to engage in the game/leisure activity, and will be reminded at 15/10/5 minute intervals before terminating the program when their time elapses.

### Why:
This product is intended to serve as voluntary guidelines on how the user spends their time. The goal is to ensure the user is deliberate and mindful with how they spend their time.

## 2. Architectural Principles

This architecture follows several key principles:

### Single Source of Truth (SSOT)
As a background application, it's vital that the program does not have a substantial resource footprint. To minimize memory resources and prevent conflicting states, information will adhere to an SSOT pattern (e.g., `GameList` is the *only* source for categorizations, `ProcessMonitor` is the *only* source for live processes).

### Event-Driven with Signals/Slots
As a background application that needs to trigger under certain circumstances, the logic is defined using Qt's Signal and Slots features to implement an event-driven pattern. This decouples components (e.g., `ProcessMonitor` *emits* a signal, but doesn't know *who* receives it). That being said, an intermediary must connect the signal to the slot, which will be `AppController`'s responsibility.

### Background Worker Thread
As this needs to run unobtrusively in the background, the application logic is split. The core monitoring (`ProcessMonitor`) runs on a dedicated worker thread, ensuring the UI (popups, config window) remains responsive. See `/architecture/threading-model.md` for details.

### Dependency Injection
Components receive pointers to their dependencies via their constructor (e.g., `ConfigWindow` receives `GameList*` and `ProcessMonitor*`). This makes dependencies explicit and maintains loose coupling.

### Memory Management
Memory is managed by Qt's parent-child hierarchy (QObjects with parents are auto-deleted) and explicit `deleteLater()` calls for objects moved to threads. Non-QObjects (like `GameList`) require manual deletion by their owner (`AppController`).

## 3. Component Diagram

[TODO: Insert new Mermaid component diagram here, showing all 10 components and their relationships.]



[Image of the Mindfulness application component architecture diagram]


## 4. Component Breakdown

### Core Components

#### `AppController`
* **Purpose:** The central "brain" and entry point of the application. It owns all core components and routes the initial signals/slots.
* **SSOT / Data Ownership:**
    * **Owns (creates and deletes):**
        * `GameList` (manual `delete` required)
        * `ProcessMonitor` (via `deleteLater()` when thread finishes)
        * `GameSessionManager` (QObject child)
        * `ConfigWindow` (created on-demand, `WA_DeleteOnClose`)
        * `CategorizeDialog` (created on-demand, `WA_DeleteOnClose`)
        * `QSystemTrayIcon` (QObject child)
* **Key Collaborators:**
    * **Talks To:** 
      * `ProcessMonitor` (to start/stop), 
      * `ConfigWindow` (to show), 
      * `CategorizeDialog` (to show).
    * **Listens To:** `QSystemTrayIcon` (for `onShowConfig`, `onQuit` triggers), `ProcessMonitor` (for `uncategorizedAppFound` signal).
* * **Signal/Slot Connections (Interface):**
  * **Slot Connections:** (^^ = emits)
    * Process Monitor:
      * Process monitor will emit a 'game detected' signal, which `AppController` will connect to
      * `GameSessionManager` which offers a slot for that signal.
    * Process Monitor:
      * Process monitor will emit a 'uncategorized app found' signal, which `AppController` connects to
      * `
  * **Listens To (Slots):** `onShowConfig()`, `onQuit()` (from `QSystemTrayIcon`), `onUncategorizedAppFound(QString)` (from `ProcessMonitor`).
  * **Emits (Signals):** None.
* **Threading Note:** Runs on the **Main/GUI Thread**.

#### `GameList`
* **Purpose:** The "Single Source of Truth" for all user-defined application categorizations.
* **SSOT / Data Ownership:**
    * **Owns:** `m_categorizations` (the `QHash<QString, bool>` master list).
    * **Owns:** All read/write access to the `games.txt` file.
* **Key Collaborators & Data Flow:**
    * **Provides Data To:** `ProcessMonitor` (read-only), `ConfigWindow` (read/write), `CategorizeDialog` (read/write).
* **Threading:** This class is *not* thread-safe. It is designed to be read from the Worker Thread (`ProcessMonitor`) and written to *only* from the Main Thread (UI components).
* **Signal/Slot Connections (Interface):** None. This is a passive, non-QObject data class.
* **Threading Note:** This class is *not* thread-safe. It is read from the Worker Thread (`ProcessMonitor`) and written to *only* from the Main Thread (UI components).


#### `GameSessionManager`
* **Purpose:** A "factory" that creates, owns, and manages the lifecycle of all active `GameSession` objects.
* **SSOT / Data Ownership:**
    * **Owns:** `m_activeSessions` (a `QList` of all active `GameSession*` objects).
* **Key Collaborators & Data Flow:**
    * **Listens To:** `ProcessMonitor` (for `gameDetected` and `processTerminated` signals), each `GameSession` (for `sessionFinished` signal).
    * **Talks To:** `GameSession` (to create it).
* **Signal/Slot Connections (Interface):**
    * **Listens To (Slots):** `onGameDetected(DWORD, QString)`, `onProcessTerminated(DWORD)`, `onSessionFinished()`.
    * **Emits (Signals):** None.
* **Threading Note:** Runs on the **Main/GUI Thread**.


#### `GameSession`
* **Purpose:** Game Session serves as the command center for each individual "gaming session." It is responsible for
              * tracking the time allotted, time elapsed, sending warning signals, and managing overtime requests. It
              * should eventually track the metrics of each session for future review.
* **SSOT / Data Ownership:**
    * 
* **Key Collaborators & Data Flow:**
    * [TODO: Fill in. (e.g., Creates `TimeSetDialog` and `WarningDialog`. Calls `ProcessUtils::terminateProcess`. Emits `sessionFinished` to `GameSessionManager`.)]

### Monitoring Components

#### `ProcessMonitor`
* **Purpose:** The "Single Source of Truth" for all *currently running* processes. Runs on a background thread to scan the system.
* **SSOT / Data Ownership:**
    * **Owns:** `m_activeProcessMap` (the `QHash<DWORD, QString>` live process list).
    * **Owns:** `m_knownRunningPIDs` (the `QSet` for state tracking).
    * **Owns:** `m_flaggedUncategorizedApps` (the `QSet` for spam prevention).
* **Key Collaborators & Data Flow:**
    * **Talks To:** `ProcessUtils` (calls `updateActiveProcessMap`), `GameList` (read-only calls to `isCategorized`/`isGame`).
    * **Emits Signals To:** `AppController` (`uncategorizedAppFound`), `GameSessionManager` (`gameDetected`).
* **Threading:** Runs on a dedicated **Worker Thread**.

#### `ProcessUtils` (Namespace)
* **Purpose:** A stateless utility library providing helper functions for interacting with the Windows API.
* **SSOT / Data Ownership:** None. This component is stateless.
* **Key Collaborators & Data Flow:**
    * **Called By:** `ProcessMonitor` (to update the live map), `GameSession` (to terminate a process).
    * **Talks To:** Windows API (`EnumProcesses`, `GetModuleBaseNameW`, `TerminateProcess`).

### UI Components

#### `ConfigWindow`
* **Purpose:** [TODO: Fill in. (e.g., The main configuration "archive." Allows users to view and manage all *already categorized* applications (both games and non-games).)]
* **SSOT / Data Ownership:** None. This is a pure UI *view* and *controller*.
* **Key Collaborators & Data Flow:**
    * [TODO: Fill in. (e.g., Created by `AppController`. Reads from `GameList` (Master List) and `ProcessMonitor` (Live List) to build its UI. Writes changes back to `GameList`.)]

#### `CategorizeDialog`
* **Purpose:** [TODO: Fill in. (e.g., The "inbox" dialog that pops up *only* to categorize new, unknown applications.)]
* **SSOT / Data Ownership:** None.
* **Key Collaborators & Data Flow:**
    * [TODO: Fill in. (e.g., Created by `AppController`. Reads from `ProcessMonitor` to find uncategorized apps. Writes changes back to `GameList`.)]

#### `TimeSetDialog`
* **Purpose:** [TODO: Fill in. (e.g., A modal dialog that prompts the user to set a time limit for a new game session.)]
* **SSOT / Data Ownership:** None.
* **Key Collaborators & Data Flow:**
    * [TODO: Fill in. (e.g., Created and shown by `GameSession`. Returns the selected time (int) to the `GameSession`.)]

#### `WarningDialog`
* **Purpose:** [TODO: Fill in. (e.g., A modal dialog that warns the user their time is ending and offers an extension.)]
* **SSOT / Data Ownership:** None.
* **Key Collaborators & Data Flow:**
    * [TODO: Fill in. (e.g., Created and shown by `GameSession`. Returns a `UserChoice` enum (OK/Extend) to the `GameSession`.)]