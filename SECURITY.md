# Security Policy

## Supported versions

| Version | Supported |
|---------|-----------|
| 2.0.x   | Yes       |
| < 2.0   | No        |

## Reporting a vulnerability

CleanerQt can delete files, scan the entire filesystem, and request UAC elevation for NTFS MFT access. If you discover a security issue, please report it responsibly.

**Do not** open a public GitHub issue for security vulnerabilities.

Instead, email **th000cw02@gmail.com** with:

- A description of the issue
- Steps to reproduce
- Impact assessment (e.g., arbitrary file deletion, privilege escalation)
- Affected version

We aim to acknowledge reports within 7 days and provide a fix or mitigation timeline when possible.

## Scope

In scope:

- Unintended file deletion or corruption
- Privilege escalation beyond documented UAC behavior
- Path traversal in scan/export/import
- Remote code execution via network features (auto-update checker)

Out of scope:

- Deletion of files the user explicitly selected for removal
- Behavior that requires running as Administrator when admin mode is documented
