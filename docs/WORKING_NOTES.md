## Current Session: [11/4/2025]
### Discovered
- Documentation is important! Today is the second day of working on Mindfulness, and while I've finished implementing around half of the classes, I hit a hiccup when
  I tried to implement the process categorization and monitoring features. There was some issue with the data flow, and I realized that while I had dedicated some 
  initial time to creating a design, I didn't create any documentation to record that design and all of the various pieces of pertinent information that I've stumbled
  on during the process. As such, the rest of today is all about catching the documentation up to the current implementation stage.

- While I have a decent conceptual understanding of the various components and the overall architecture, it's already grown to a size that I cannot keep entirely in my head.
  An important part of this documentation process will be making sure all of the relevant details that I've already gone over and understood are reflected properly within docs.

### Decided
- I decided to pause on implementation and focus on documentation and ironing out the overall design/architecture to deal with any problems ahead of time.

### TODO Document
- [ ] Create all doc files for `/api`
  - [x] Create Document Content Strategy
  - [x] Decide what information should be contained within /api
  - [ ] Write API contracts for `/src/core/*`
    - [ ] Write API contract for `/api/AppController.md`
    - [ ] Write API contract for `/api/GameList.md`
    - [ ] Write API contract for `/api/GameSession.md`
    - [ ] Write API contract for `/api/GameSessionManager.md`
  - [ ] Write API contracts for `/src/monitoring/*`
    - [ ] Write API contract for `/api/ProcessMonitor.md`
    - [ ] Write API contract for `/api/ProcessUtils.md`
  - [ ] Write API contracts for `/src/ui/*`
    - [ ] Write API contract for `/api/ConfigWindow.md`
    - [ ] Write API contract for `/api/CategorizeDialog.md`
    - [ ] Write API contract for `/api/TimeSetDialog.md`
    - [ ] Write API contract for `/api/WarningDialog.md`
- [x] Create all doc files for `/architecture`
  - [ ] Write Component Breakdown for `/src/core/*`
    - [ ] Write Component Breakdown for `AppController`
    - [ ] Write Component Brekdown for `GameList`
    - [ ] Write Component Breakdown for `GameSession`
    - [ ] Write Component Breakdown for `GameSessionManager`
  - [ ] Write Component Breakdown for `/src/monitoring/*`
    - [ ] Write Component Breakdown for `ProcessMonitor`
    - [ ] Write Component Breakdown for `ProcessUtils`
  - [ ] Write Component Breakdown for `/src/ui/*`
    - [ ] Write Component Breakdown for `ConfigWindow`
    - [ ] Write Component Breakdown for `CategorizeDialog`
    - [ ] Write Component Breakdown for `TimeSetDialog`
    - [ ] Write Component Breakdown for `WarningDialog`
  - [ ] Create Document Content Strategy for `/architecture/data-flow-and-ssot.md`
  - [ ] Create Document Content Strategy for `/architecture/sequence-diagrams.md`
  - [ ] Create Document Content Strategy for `/architecture/threading-model.md`
- [x] Create all doc files for `/data`
  - [ ] Create Document Content Strategy for `/data/file-formats.md`
  - [ ] Create Document Content Strategy for `/data/in-memory-stores.md`

## Next Session: [11/5/2024]
### Discovered
- 

### Decided

### TODO Document

- [x] Create all doc files for `/design`
  - [ ] Create Document Content Strategy for `/design/game-sesion-logic.md`
  - [ ] Create Document Content Strategy for `/design/ui-flow-and-ux.md`
  - [ ] Create Document Content Strategy for `/design/use-cases.md`
  - [ ] Create Document Content Strategy for `/design/requirements.md`
- [x] Create all doc files for `/implementation`
  - [ ] Create Document Content Strategy for `/implementation/build-setup.md`
  - [ ] Create Document Content Strategy for `/implementation/coding-standards.md`
  - [ ] Create Document Content Strategy for `/implementation/testing-strategy.md`
- [ ] Create all doc files for `/adr`
  - [ ] Create Document for ADR 001
  - [ ] Create Document for ADR 002
  - [ ] Create Document for ADR 003
  - [ ] Create Document for ADR 004
  - [ ] Create Document for ADR 005
  - [ ] Create Document for ADR 006