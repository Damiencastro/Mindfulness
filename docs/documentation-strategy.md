# Documentation Categories
1. `/adr` (Architecture Decision Records)
    - What goes here: Significant technical decisions with lasting impact
    - When to write one:
      - You debate between two approaches
      - You discover a non-obvious requirement
      - You change direction from original plan
      - You choose a library/framework/pattern
    - The 5-minutes ADR habit:
      - Write an ADR immediately when you ask yourself, "Why did we do it this way?"

2. `/architecture`
    - What goes here: How components fit together, data flow, system design
    - When to update:
      - Adding a new major component
      - Changing how components communicate
      - Discovering a data flow issue
      - Establishing a new pattern/principle

3. `/api`
    - What goes here: Public interfaces, method signatures, contracts between components
    - When to update:
      - After implementing a class's public interface
      - When adding signals/slots
      - Before someone else uses your component
  
    - The "Interface First habit:
      - Write the public documentation BEFORE implmementation:
        ```cpp
            // Document first:
            // GameList::isGame(QString) - Returns true if process is categorized as game
            // Thread-safe: Yes
            // Throws: No
            bool isGame(const QString& processName) const;
        ```
4. `/data`
    - What goes here: File formats, database schemas, data structures, protocols
    - When to update:
      - Defining a new file format
      - Creating a persistent data structure 
      - Establishing a communication protocol
    - Template for data docs:
    
    ## Format: [Name]
    Version: 1.0
    Encoding: UTF-8

    ### Structure
    [Describe Structure]

    ### Example
    [Show real example]

    ### Migration Notes
    [How to handle version changes]

5. `/design`
    - What goes here: Requirements, use cases, UI mockups, user stories
    - When to update:
      - Project start (requirements)
      - Before implementing new feature
      - After user feeback changes requirements
    - Keep it high-level: Focus on WHAT and WHY, not HOW

6. `/implementation`
    - What goes here: Build instructions, development setup, coding standards
    - When to update:
      - Adding new dependency
      - Changing build process
      - Establishing code pattern
      - After painful debugging session (add to troubleshooting?)

7. `/operations`
    - Deployment Procedures
    - Monitoring Setup
    - Backup/restore Procedures
    - Performance Benchmarks

8. `/examples`
    - Code Snippets
    - Configuration Examples
    - Common Usage Patterns

9. `/changelog`
    - Version History
    - Breaking Changes
    - Migration Guides





# Active Documentation Habits

## Daily Triggers:
- [ ] New class created -> Update `/api`
- [ ] Bug fixed -> Check if it needs `/implementation/troubleshooting.md`
- [ ] "Hmm, that's weird" moment -> Consider an ADR

## Weekly Triggers:
- [ ] Review git commits -> Update `/changelog/WEEKLY_NOTES.md`
- [ ] Clean up working notes -> Update `/WORKING_NOTES.md`
- [ ] New dependencies -> Update `/implementation/build-setup.md`

## Per Feature:
- [ ] Before Starting -> Update `/design` with use cases
- [ ] After interface design -> Update `/api`
- [ ] After implementation -> Update `/architecture` if needed
- [ ] After testing -> Update `/implementation/testing-strategy.md`


