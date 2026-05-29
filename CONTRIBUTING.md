# Contributing to CleanerQt

Thank you for your interest in contributing! CleanerQt is a Windows-only Qt 6 disk analyzer — please keep platform scope in mind.

## Development setup

1. Install **Qt 6.8** (or 6.5+) with MSVC 2022 64-bit modules: Core, Quick, Qml, Widgets, Concurrent, Network, Test
2. Install **CMake 3.16+** and **Visual Studio 2022**
3. Clone and configure:

```powershell
git clone https://github.com/th000cw02-afk/cleaner_qt.git
cd cleaner_qt
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2022_64" -DCLEANER_QT_BUILD_TESTS=ON -DCLEANER_QT_FETCH_SPDLOG=ON
cmake --build build --config Release
```

## Running tests

```powershell
.\scripts\test.ps1
```

Or manually:

```powershell
cd build
ctest -C Release --output-on-failure
```

## Code style

- Match existing C++17 and QML conventions in the file you edit
- Keep changes focused — avoid unrelated refactors in the same PR
- Add or update Qt Test cases when fixing bugs or adding behavior

## Pull requests

1. Fork the repo and create a feature branch from `main`
2. Ensure tests pass locally
3. Open a PR with a clear description of **what** changed and **why**
4. Link related issues when applicable

## Reporting bugs

Use the [bug report template](.github/ISSUE_TEMPLATE/bug_report.yml). Include Windows version, whether you ran as Administrator, Qt build type, and steps to reproduce.

## Feature requests

Use the [feature request template](.github/ISSUE_TEMPLATE/feature_request.yml).

## Security issues

Do **not** open public issues for vulnerabilities. See [SECURITY.md](SECURITY.md).
