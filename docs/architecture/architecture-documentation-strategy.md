# Architecture - Documentation Strategy

## Folder Purpose:
- This folder holds the "HOW." It's the 30,000ft view of the system.

## Contents:
- High-level diagrams and documents explaining how components fit together, the rules of their interaction, and the core principles.

## DRY Boundary:
- This folder shows *that* components interact, but it doesn't detail the specific function calls (that's `/api`) or the user's journey (that's `/design`).

## Files

- `component-overview.md`: This document lists each component and it's responsibility. It briefly describes it's relationship to other components
- `data-flow-and-ssot.md`: This documents the "Single Source of Truth" principle and how it is applied within the overall system design
- `threading-model.md`: This documents the usage of threads and how it impacts the system.
- `sequence-diagrams.md`: This documents key interactions and how the various components interact to facilitate certain functionality