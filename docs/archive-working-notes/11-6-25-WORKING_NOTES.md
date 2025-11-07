## Current Session: [11/6/2025]

### Discovered:

- Documentation commit was not finished
- Our ProcessMonitor was not following the single-responsibility principle

### Decided:

- Today's focus will be finishing and commiting the new documentation
- We are creating ProcessEventDispatcher to hold business logic related to process starting/terminating.


### Commit Message

- `docs/architecture/component-overview.md` has been updated to reflect a change in the application architecture.
  A new Application Service has been added to handle the business logic and inter-layer orchestration in a state-less
  way. This service, `ProcessEventDispatcher` will have an API document written and header file created.


### TODO Document
- [x] Update `component-overview.md`
- [x] Create `docs/api/services/ProcessMonitor-API.md`
- [x] Create `docs/api/services/ProcessEventDispatcher-API.md`
- [x] Fix CMake to include new architectural components
- [x] Update AppController for updated signal/slots
- [ ] Next Components to work on:
  - [x] `Application`, `ApplicationRepository`
  - [x] `GameSession`, `GameSessionManager`
  - [x] `CategorizationManager`
- [ ] Figure out UI architectural implications and how it fits into everything
- [x] Figure out how to pass ApplicationRepository to ProcessEventDispatcher properly (*? &? Any syntax?)