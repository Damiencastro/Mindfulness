## Previous Session: [11/5/2025]
### Discovered
- A new anti-pattern in how signal/slots were being handled. `AppController` was wearing two hats as it was
  delegating game session signals to a dedicated manager, but handling the signal from `ProcessMonitor` itself
  rather than delegating to a dedicated manager. It's supposed to be just an orchestrator, not handling the 
  business logic of process monitoring.

- Architectural components weren't classified properly, I've done some research and identified a Domain-Driven design pattern
  This means we have Repositories that handle persistence, Services that handle infrastructure, Facotries that create complex objects,
  Entities representing business concepts, and finally application services orchestration. We need to make some refinements in our
  ProcessMonitor by extracting the business logic into a separate `ProcessEventDispatcher` component. The `ProcessMonitorService` will
  become a purer service.

- I've discovered a need for a new domain entity, that of an `Application`. It will contain information about the process name, the date
  it was first seen, when it was last seen, how many sessions it's been played, total time played, custome time limits, toggling session
  prompts, and alternate warning strategy/schemas. I will also be creating an `ApplicationRepository` class which will be an abstracted 
  file storage class, but later can be refactored to add database support.





### Decided
- I've decided to create a new class, Categorization Manager, and will be working on fitting that into the 
  architecture and working out the interfaces.

- Decided to refactor our architecture using a Domain-Driven Design pattern, separating our ProcessMonitor into a `ProcessMonitorService` and a `ProcessEventDispatcher` which handles the actual process business logic.

### TODO
- [x] Create Component Grouping for features
- [ ] UI/UX Flow and HCI design of windows/dialogs
- [x] Change all occurrences of `onUncategorizedAppDetected -> onUncategorizedAppDetected` for consistent naming
- [ ] Look into `GameSessionManager::onProcessTerminated(DWORD)` slot logic and orchestrate safe session cleanup
- [ ] Add categorizationComplete connection and handling logic to create new Application instance
- [ ] Create appli8cation data flows to accompany the example within `component-overview.md` - 6 ### Game Session w/
- [ ] Write ADR: `008-domain-driven-design.md`

- Questions to ask Claude:
  - There is an inconsistency in `component-overview-new.md` Process Monitor owns m_flaggedUncategorizedApps, but Categorization Manager
    also says that it owns `m_pendingCategorization` which violates the single truth principle as they are both QSet<QString>s, presumably
    holding the same information
  - Is there computational overhead in the signal emission and slot execution? I'm just thinking about how information would get from the
    repository into the Factory Manager, `CategorizationManager`, might be solved with new, new doc, let's see...
  - What slot receives `categorizationComplete(QString)` which is emitted from `CategorizationManager`?
    - Candidate answer: `ApplicationRepository::onCategorizationComplete()` to save the new data into the repository
  - Is there a way to retrieve an icon from a process to add to the `Application` entity?
  - What information must Application hold, and what information should GameSession hold? How should we link those respective entities?
    If you think about it, half of the stuff we said for Application is really just extrapolated from the GameSessions. What if we just
    tracked the GameSessions, instead? Food for thought...
    - Candidate Answer: We could track both independently. We would need to do three things: 
                            1. Create a new `SessionRepository` which tracks the user's sessions and could display a sequential list of sessions for the user to review.
                            2. Create a `GameSessionManager` slot for `onGameSessionComplete()` which adds the session play-time to the 
                                corresponding application, and saves both in the repository (need to look into best push/pull saving schema)
                            3. Saves both the `GameSession` and the updated `Application` to their respective repositories
  - What's the best way to handle push/pull of data with regards to the repository in DDD?
  - *NOTE* Make sure to make the data gathering and saving fairly minimal. We don't want extensive statistics yet.


### Commit Message

- Categorization Manager added, identified architectural anti-pattern in which the `AppController` was handling
both signal/slot orchestration, *and* categorization business logic. Abstracting that out to a new 
`CategorizationManager` component will allow de-coupling of business logic and signal/slot orchestration

- AppController, and probably most others, need to be organized into logical grouping of statements, rather
  than sequential 'steps' as it is now. Will work on that change, slowly but surely.


##




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



