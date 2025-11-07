## Current Session: [11/6/2025]

### Discovered:
- We were still missing implementation for several components and had old code using the GameList component. Started updating those components
  but it's an ongoing process. Will create TODO items for each file that needs to be reviewed and cleaned up.
- As we have lots of components to review and update to the new architecture, I'm going to attack it by following the execution logic through
  AppController, making sure that the logic flows properly.
- Logging is not being handled intentionally, there is a need to create a logging service
- I'm close to getting the v1.0.1 ready for commit. I'll need to go through each change and write a section in the commit message for posterity


### Decided:
- I've decided to create a logging service that will log general events as well as particular errors.


### Commit Message
- 


### TODO Document
- [ ] CategorizationManager.h & CategorizationManager.cpp
- [ ] CategorizationDialog.h & CategorizationDialog.cpp
- [ ] Application.h & Application.cpp
- [ ] ApplicationRepository.h & ApplicationRepository.cpp
- [ ] Create Test Cases
  - [ ] Update test_ApplicationRepository.cpp
- [ ] Clean up header declarations
- [ ] Change CategorizeDialog to display a list of apps to categorize
- [ ] 