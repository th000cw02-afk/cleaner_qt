# Changelog

All notable changes to CleanerQt are documented here.

## [2.0.0] - 2026-05-30

### Added

- Complete v2 rewrite: Qt 6 QML UI with Material theme (dark/light)
- NTFS MFT fast scan with parallel directory walk fallback
- Directory tree + squarified treemap with extension coloring
- All Files virtual list (`FileIndexModel`)
- Cleanup Hub with 12 Scour-style scanners
- Duplicates page, regex search, batch delete
- Scan snapshots (`.cqtscan`) save/load
- CLI: `--scan`, `--output`, `--format csv|html`, `--mft`, `--import-csv`, `--verbose`
- Logical vs allocated size, hard-link registry
- Settings: exclusions, MFT preference, portable mode, logging
- GitHub Actions CI (build + ~20 Qt Test cases)
- Auto-update checker (GitHub Releases)

### Known limitations

- Broken links, duplicate archives, and orphaned app data scanners return empty results (stubs)
- File Watcher tab is a UI placeholder

[2.0.0]: https://github.com/th000cw02-afk/cleaner_qt/releases/tag/v2.0.0
