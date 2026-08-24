# 📂 File-by-File Technical Guide & Codebase Anatomy

A complete breakdown of every file in **`ft_ncdu`**, explaining its role, key functions, design choices, and 42 Norminette structures.

---

## 📑 Directory Map

```
ft_ncdu/
├── includes/
│   ├── config.h            # Static limits, app version, thread counts
│   ├── ft_ncdu.h           # Data structures, global state, prototypes
│   └── presets.h           # Preset structures & extern array declarations
├── src/
│   ├── main.c              # Entrypoint, argument parsing, signals, runtime init
│   ├── core/
│   │   ├── scanner.c       # Directory reading & async thread dispatcher
│   │   ├── worker.c        # Recursive size calculation & symlink health
│   │   ├── nav.c           # Navigation jumps, steps, and folder traversal
│   │   ├── events.c        # Main TUI event loop & search input handler
│   │   └── dispatch.c      # Action, tool, and shortcut dispatcher
│   ├── actions/
│   │   ├── cli.c           # Headless CLI operations (ntcl13, clean42, heal, report)
│   │   ├── delete.c        # Safe deletion engine & batch mark operations
│   │   ├── goinfre.c       # Goinfre link & unlink relocation logic
│   │   ├── healer.c        # Station-switch symlink repair & .zshrc injector
│   │   ├── presets.c       # Interactive preset selector modal
│   │   ├── peek.c          # Scrollable file preview modal with line numbers
│   │   ├── tools.c         # Goto path, $EDITOR, interactive subshell, custom exec
│   │   └── report.c        # Markdown quota export & sort mode cycler
│   ├── ui/
│   │   ├── colors.c        # NCurses color pairs & unicode rounded borders
│   │   ├── hud.c           # Telemetry cards (Quota, Inodes, Goinfre NVMe)
│   │   ├── table.c         # File explorer table & allocation percentage bars
│   │   ├── inspector.c     # Metadata inspector & command deck
│   │   ├── draw.c          # Frame coordinator, header bar, and status footer
│   │   └── modals.c        # Confirmation modal & keyboard cheatsheet modal
│   └── utils/
│       ├── presets_data.c  # 9 cleanup presets & shell bypass exports
│       ├── security.c      # Shell argument escaping & protected path guard
│       ├── format.c        # Byte sizes, symbolic permissions, breadcrumbs
│       ├── memory.c        # Memory pool allocation & cleanup
│       └── filter.c        # Sorting comparator & substring search filter
```

---

## 🔍 Detailed Breakdown by Module

### 1. `includes/`
- **`config.h`**: Holds core operational constants (`MAX_ENTRIES 65536`, `SCAN_THREADS 16`, `PATH_MAX_LEN 4096`, `TICK_DELAY_MS 50`).
- **`ft_ncdu.h`**: The primary header. Defines `t_file_entry`, `t_app_state`, `t_rect`, `t_sort_mode`, `t_size_mode`, and all internal function prototypes.
- **`presets.h`**: Declares `g_clean_presets[PRESET_COUNT]`, `g_bootstrap_targets`, and `g_shell_exports`.

---

### 2. `src/core/` (The Engine)
- **`main.c`**:
  - Sets up signal handlers (`SIGINT`, `SIGTERM`, `SIGWINCH`).
  - Checks `argv[0]` for `ntcl13` or `clean42` to instantly trigger headless cleaner.
  - Parses CLI flags (`-c`, `--clean`, `--heal`, `--report`, `--bootstrap`, `-v`, `-h`).
  - Initializes memory pool, queries user login, and boots NCurses.
- **`scanner.c`**:
  - `start_async_scan(path)`: Resets counters, captures root `st_dev`, and spawns the background orchestrator.
  - `async_scan_orchestrator()`: Reads direct children with `opendir`/`readdir`, triggers initial filter/render, then joins worker threads.
- **`worker.c`**:
  - `calculate_dir_recursive(path, out_s, out_d)`: Deep directory tree walker. Checks `st.st_dev == g_state.root_dev` to avoid crossing mount points.
  - `check_symlink_health(entry)`: Uses `readlink()` and `stat()` to determine if a symlink is broken (`[DEAD LINK]`) or points to `/goinfre`.
  - `scan_thread_worker()`: Worker thread entry function; grabs directory items using strided indexing.
- **`nav.c`**:
  - Handles vertical cursor movement (`j`, `k`, `KEY_UP`, `KEY_DOWN`), jumping to top/bottom (`g`, `G`, `Home`, `End`), pagination (`PageUp`, `PageDown`), and stepping into/out of directories (`l`, `h`, `Enter`, `Backspace`).
- **`events.c`**:
  - `run_event_loop()`: Central event loop calling `draw_ui()` and `getch()`.
  - `handle_search_input(ch)`: Real-time substring query buffer with backspace and escape handling.
- **`dispatch.c`**:
  - Routes single keystrokes to action handlers (`s` for Goinfre, `u` for Unlink, `H` for Healer, `T` for Trash, `Z` for .zshrc, `C` for Presets, `d`/`x` for Delete, `p` for Peek, `o` for Sort, `e` for Editor, `t` for Subshell).

---

### 3. `src/actions/` (The 42 Storage Suite)
- **`cli.c`**:
  - `run_cli_clean()`: Native C headless cleaner. Measures `statvfs()` before/after, runs 9 cleanup tiers, and prints freed space.
  - `run_cli_heal()`: Headless symlink repair for station switches.
  - `run_cli_bootstrap()`: Headless relocation of toolchains to `/goinfre`.
  - `run_cli_report()`: Pretty-prints quota stats to stdout.
- **`delete.c`**:
  - `action_delete()`: Handles single target or multi-item batch deletion with safety confirmation modal and protected file checks.
- **`goinfre.c`**:
  - `action_symlink_goinfre()`: Moves target to `/goinfre/$USER/target` and creates symlink.
  - `action_unlink_goinfre()`: Pulls real data from `/goinfre` back to `$HOME` and deletes symlink.
- **`healer.c`**:
  - `action_heal_symlinks()`: Recreates missing directories in `/goinfre` for dangling symlinks when switching stations.
  - `action_inject_zshrc()`: Injects quota bypass exports (`HF_HOME`, `CARGO_HOME`, `DOCKER_CONFIG`) into `~/.zshrc`.
- **`presets.c`**:
  - `action_cleaning_presets()`: Renders interactive popup menu showing all 9 presets.
  - `action_nuke_junk()`: Quick one-shot cache purger.
- **`peek.c`**:
  - `action_file_peek()`: In-terminal file viewer with line numbering, binary detection, and scroll controls.
- **`tools.c`**:
  - `action_goto_path()`: Teleport to any path.
  - `action_edit()`: Suspends NCurses, opens `$EDITOR`, and restores TUI.
  - `action_shell()`: Drops user into an interactive subshell at current folder.
  - `action_custom_command()`: Prompts for shell command to execute on target.
- **`report.c`**:
  - `action_export_report()`: Exports Markdown table of top 30 disk consumers to `quota_report.md`.
  - `action_cycle_sort_mode()`: Cycles sort modes and calls `qsort()`.

---

### 4. `src/ui/` (The TUI Experience)
- **`colors.c`**: Sets up 15 high-contrast color pairs and draws rounded box borders (`╭`, `─`, `╮`, `│`, `╰`, `╯`).
- **`hud.c`**: Renders top cards (Home Quota gauge, used Inodes, and Goinfre NVMe status).
- **`table.c`**: Renders directory rows with badges (`[DIR]`, `[FILE]`, `[LINK]`, `[DEAD]`), size, allocation bar, and name.
- **`inspector.c`**: Displays metadata for highlighted item and the 42 Command Deck.
- **`draw.c`**: Master frame coordinator; draws header, HUD, split table/inspector, and powerline footer.
- **`modals.c`**: Renders interactive `[Y]/[N]` confirmation and Keyboard Reference modals.

---

### 5. `src/utils/` (Helpers & Safety)
- **`presets_data.c`**: Static tables containing the 9 cleaning presets, bootstrap directories, protected paths, and shell exports.
- **`security.c`**: `shell_escape()` prevents command injection on filenames with spaces/quotes; `is_protected_target()` prevents deleting `.ssh`, `.zshrc`, `.gitconfig`, etc.
- **`format.c`**: Human-readable byte formatter (`format_size`), symbolic permissions (`format_permissions`), and path breadcrumb truncation (`format_breadcrumbs`).
- **`memory.c`**: Allocates the 65,536-entry heap arrays (`entries` and `filtered`) and handles clean shutdown.
- **`filter.c`**: Implements `compare_entries` (for `qsort`), `apply_filter` (for substring search), and `count_marked_items`.
