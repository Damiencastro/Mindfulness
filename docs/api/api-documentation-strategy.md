# API - Documentation Strategy

## Folder Purpose:
- This folder defines the "PUBLIC" parts of our code. It serves as the "developer's manual" for using the various components.

## Contents:
- Each component will have a file that describes the public-facing signals, slots, and methods.

## DRY Boundary:
- This folder describes the technical implementation of public-facing signals, slots, and methods. It does not contain any private implementation detail or the component's purpose.

## Files

### Core Components
- `AppController.md`
- `GameList.md`
- `GameSession.md`
- `GameSessionManager.md`

### Monitoring Components
- `ProcessMonitor.md`
- `ProcessUtils.md`

### UI Components
- `ConfigWindow.md`
- `CategorizeDialog.md`
- `TimeSetDialog.md`
- `WarningDialog.md`