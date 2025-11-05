# Data - Documentation Strategy

## Folder Purpose:
- This folder defines the "STRUCTURE" of data.

## Contents:
- A specification for any data that is stored on disk or has a persistent in-memory structure.

## DRY Boundary:
- This document *only* defines the data's format. It does not care *who* creates it or *who* reads it (that's in `/architecture`).

## Files

- `file-formats.md`: Defines how data is encoded in non-volatile format.
- `in-memory-stored.md`: Defines how data is defined, who holds it, and who uses it.