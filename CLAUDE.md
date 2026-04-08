# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Flameshot is a cross-platform (Linux, Windows, macOS) screenshot tool built with C++20 and Qt 6. It provides both a GUI capture interface and a CLI. Version 13.3.0.

## Build Commands

```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build
cmake --build build

# Debug build (capture GUI won't bypass window manager, useful for breakpoints)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DFLAMESHOT_DEBUG_CAPTURE=ON

# Run
./build/src/flameshot gui

# Run tests
ctest -C RelWithDebInfo --test-dir build

# Format code (required before committing)
clang-format -i $(git ls-files "*.cpp" "*.h")
```

## Key CMake Options

- `FLAMESHOT_DEBUG_CAPTURE` - Capture GUI won't go fullscreen, enabling debugger use
- `ENABLE_IMGUR` - Enable Imgur uploader
- `USE_WAYLAND_CLIPBOARD` - Use KF Gui Wayland Clipboard
- `USE_PORTABLE_CONFIG` - Config stored in app folder (auto-enabled on Windows)
- `DISABLE_UPDATE_CHECKER` - Disable update checking

## Architecture

**Two process modes:**
- **Daemon mode** (`flameshot` with no args): persistent background process with tray icon, hotkeys (Win/macOS), clipboard persistence (X11), update checker. Windows only supports daemon mode.
- **Single-action mode** (`flameshot gui`, `flameshot screen`, etc.): performs one action then exits.

**Core singletons:**
- `Flameshot` (`src/core/flameshot.h`) - High-level API mirroring CLI subcommands: `gui()`, `screen()`, `full()`, `launcher()`, `config()`, `history()`
- `FlameshotDaemon` (`src/core/flameshotdaemon.h`) - Daemon process management. Static methods transparently route calls: direct if current process is daemon, via D-Bus otherwise (Linux only)
- `ConfigHandler` (`src/utils/confighandler.h`) - Configuration singleton based on QSettings, ini format, auto-reloads on file change. Settings groups: General, Shortcuts

**Inter-process communication:**
- Linux: D-Bus (`FlameshotDBusAdapter`)
- Windows/macOS: Global hotkeys via QHotkey library

**Entry point:** `src/main.cpp` - Handles CLI parsing, creates `Flameshot::start()`, and conditionally `FlameshotDaemon::start()` for daemon mode.

## Key Source Directories

- `src/core/` - Flameshot, FlameshotDaemon, CaptureRequest
- `src/cli/` - CommandLineParser, CLI subcommand handling
- `src/tools/` - Modular capture tools (pencil, arrow, circle, text, etc.)
- `src/widgets/capture/` - CaptureWidget (main capture/annotation UI)
- `src/config/` - ConfigWindow and configuration UI tabs
- `src/utils/` - ConfigHandler, ScreenGrabber, FilenameHandler, ScreenshotSaver

## Adding a New Config Setting

1. Choose a name (valid C++ identifier for general, tool type name for shortcuts)
2. Add getter/setter in `ConfigHandler` using `CONFIG_GETTER_SETTER` macro (or `CONFIG_GETTER`/`CONFIG_SETTER` individually)
3. If custom validation needed, subclass `ValueHandler` (see `src/utils/valuehandler.h`)
4. Add UI widgets to appropriate `ConfigWindow` tab

## Code Conventions

- Use `&Class::signal` / `&Class::slot` syntax, not `SIGNAL()`/`SLOT()` macros
- Platform-specific code uses: `Q_OS_MACOS`, `Q_OS_WIN`, `Q_OS_UNIX`
- D-Bus code guarded with `#if !(defined(Q_OS_MACOS) || defined(Q_OS_WIN))`
- Formatting: Mozilla-based clang-format style, 4-space indent
- Clang-tidy enabled with warnings-as-errors

## External Dependencies (fetched via CMake FetchContent)

- Qt-Color-Widgets - Color picker UI
- KDSingleApplication - Single instance enforcement
- QHotkey - Global hotkeys (Windows/macOS only)
