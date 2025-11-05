# ADR-007: Critical Design Discoveries During Implementation

## Context
During implementation, we discovered several non-obvious design requirements that weren't apparent in initial planning.

## Discovery 1: Transient Process Spam
**Problem**: Windows processes like `dllhost.exe` constantly spawn with new PIDs, causing ConfigWindow to pop up repeatedly.

**Solution**: Track both PIDs (for uniqueness) AND flagged process names (to prevent re-notification).

**Implementation**: Added `m_flaggedUncategorizedApps` set to remember which names we've already notified about.

## Discovery 2: ConfigWindow Data Source Violation
**Problem**: ConfigWindow was calling `ProcessUtils::getActiveProcesses()` directly, creating a second system scan and violating single-source-of-truth.

**Solution**: ConfigWindow must get running process data from ProcessMonitor (the owner), not ProcessUtils.

**Lesson**: UI components should never directly access utility/system functions - they should get data from the designated owner component.

## Discovery 3: Null Pointer on Window Creation
**Problem**: ConfigWindow crashed when accessing `m_gameList` in constructor-called `populateList()`.

**Root Cause**: AppController wasn't creating GameList before creating ConfigWindow.

**Lesson**: Always use defensive programming with null checks in UI components, but more importantly, ensure initialization order is correct in AppController.

## Consequences
These discoveries led to:
- More complex but correct state tracking in ProcessMonitor
- Stricter rules about data access patterns
- Better understanding of component lifecycle dependencies
