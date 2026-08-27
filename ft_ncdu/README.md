#  ft_ncdu & clean42 / ntcl13
### *The Ultimate High-Performance macOS-Themed Cluster Storage Suite for 1337 & 42 Network*

[![Norminette](https://img.shields.io/badge/Norminette-100%25%20Passing-brightgreen.svg)](https://github.com/42School/norminette)
[![Language](https://img.shields.io/badge/Language-C99%20%7C%20POSIX-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-macOS%20(Darwin)%20%7C%20Linux-orange.svg)](#)
[![Threads](https://img.shields.io/badge/Concurrency-16%20Threads-blueviolet.svg)](#)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A blazing-fast, native C terminal disk usage analyzer, automated cluster quota optimizer, and workstation healer designed specifically for **1337 Coding School** and **42 Network** students across **macOS (iMacs / Apple Silicon)** and **Linux**.

---

## ⚡ 1-Line Quick Install

Run this single command in your terminal on any 42 workstation:

```bash
curl -fsSL https://raw.githubusercontent.com/laghzal49/ft_ncdu/main/install.sh | bash
```

*Auto-detects OS (macOS / Linux), compiles with `-O3`, installs to `~/.local/bin/`, configures `$PATH`, adds shell aliases (`clean`, `space`, `heal`), and enables the login station-healer hook.*

---

## 🚀 Key Highlights & Capabilities

```text
 ● ● ●    1337 | 42  ft_ncdu v2.0.0  📁 /home/student/Projects/ft_ncdu
╭─┤ 💾 HOME QUOTA ├──────╮╭─┤ 📊 INODES ├─────────╮╭─┤ ⚡ GOINFRE NVMe ├──────╮
│ [████████░░░░]  52.4%  ││ 14,290 Inodes Used    ││ 148.2 GB Free (⚡ Pool)  │
╰────────────────────────╯╰───────────────────────╯╰──────────────────────────╯
╭─┤  FINDER EXPLORER ├───────────────────────╮╭─┤ ⌘ GET INFO ├───────────────╮
│ ST  TYPE     SIZE     ALLOCATION %   NAME   ││ 📁 Name  : minishell         │
│  ❯   DIR   1.2 GB    [████████]     src/    ││ 📦 Size  : 1.2 GB (1,294 MB) │
│      FILE  42.0 KB   [█░░░░░░░]     Makefile││ 🔒 Perms : rwxr-xr-x (0755)  │
│      LINK   0.0 B    [░░░░░░░░]     goinfre ││ 🔗 Link  : -> /goinfre/...   │
│                                             │├─  macOS SHORTCUTS ──────────┤
│                                             ││ [s] Link Goinfre  [u] Home   │
│                                             ││ [p] Quick Look    [T] Trash  │
╰─────────────────────────────────────────────╯╰──────────────────────────────╯
  FINDER  28 items, 11.0 GB used │ Marked: 0 │ Sort: Size▼ │ [F / ?] Features
```

### 1. ⚡ 16-Thread Asynchronous Scanner
* Spawns 16 worker threads with strided indexing to scan 65,000+ files in milliseconds without freezing the UI.
* Built-in **POSIX `st_dev` mount boundary shielding** prevents the crawler from traversing into `/proc`, `/sys`, or external network drives.

### 2. 🧹 9-Tier Native C Fast Cleaner (`clean42` / `ntcl13`)
Re-engineered natively in C to execute in $<0.2\text{s}$ with before/after `statvfs()` disk space reporting:
* **Tier 1**: C/C++ build artifacts (`*.o`, `*.a`, `*.so`, `*.dSYM`, `core.*`, `vgcore.*`).
* **Tier 2**: Francinette & Unit Testers (`~/francinette/temp`, `.francinette/logs`, `valgrind*.log`, `*.gcda`).
* **Tier 3**: AI & PyTorch models (`~/.cache/huggingface`, `~/.cache/torch`, `~/.cache/pip`, `__pycache__`).
* **Tier 4**: Node.js & Web (`node_modules`, `~/.npm`, `~/.yarn/cache`, `.next`, `.turbo`).
* **Tier 5**: Browser & Electron caches (Chrome, Chromium, Brave, Firefox, VS Code, Discord, Slack).
* **Tier 6**: Docker & Containers (`docker system prune -a --volumes -f`).
* **Tier 7**: Norminette logs & temp state (`~/.norminette/logs`, `/tmp/*_${USER}`).
* **Tier 8**: Desktop Trash & Thumbnails (`~/.local/share/Trash`, `~/.Trash`).
* **Tier 9**: Nuclear 1337 Wipe (One-shot deep purge across all cache tiers).

### 3. 🛡️ Safe Dry-Run Simulation & Audit Trail
* **Dry-Run Mode (`-n` / `--dry-run`)**: Preview what caches would be removed without modifying or deleting any files:
  ```bash
  clean42 --dry-run
  ```
* **Audit Trail (`~/.ft_ncdu_cleanup.log`)**: Automatically logs every purge, deletion, and healing action with exact timestamps and target paths.

### 4. 🩹 Station-Hopping Healer (`H`) & Living Manifest
* **How it works**: Since `$HOME` is stored on NFS network storage and follows you to every physical workstation, **the symlinks in `$HOME` act as the living manifest**.
* When changing workstations, press **`H`** (or run `clean42 --heal`). It uses `readlink()` to inspect all symlinks pointing to `/goinfre/$USER/*` and automatically `mkdir -p`s the missing folder paths on the new machine so VS Code, Docker, and Francinette never crash.
* `[s]` **Goinfre Symlinker**: Moves target project to `/goinfre/$USER/` and creates a symlink in `$HOME` (uses 0 KB of your strict quota).
* `[u]` **Goinfre Unlinker**: Restores real data back from `/goinfre` to `$HOME` and removes the symlink.
* `[Z]` **Quota Bypass Injector**: Injects persistent environment exports into `~/.zshrc`.

### 5. 🍏 macOS-Themed NCurses TUI
* **Traffic Lights & Header**: Styled with `● ● ●` window controls and Finder breadcrumbs.
* **⌘ Get Info Inspector**: Live display of item permissions (`rwxr-xr-x`), exact bytes, and symlink health.
* **Quick Look (`p`)**: In-terminal scrollable file preview with line numbering and binary detection.
* **Spotlight Search (`/`)**: Instant real-time substring filtering.
* **3-Tab Feature Matrix (`f` / `?`)**: Interactive multi-tab command cheatsheet.

---

## 🎮 Keyboard Shortcuts Reference

| Key | Action | Description |
| :--- | :--- | :--- |
| `f` / `?` | **Feature Matrix** | Open interactive 3-tab Command & Feature cheatsheet |
| `j` / `k` / `↑` `↓` | **Navigate** | Move cursor up / down |
| `g` / `G` | **Jump** | Jump to top / bottom of list |
| `PgUp` / `PgDn` | **Scroll** | Fast multi-row viewport scroll |
| `l` / `Enter` / `h` | **Traversal** | Open highlighted folder / Step out to parent directory |
| `~` / `P` / `:` | **Teleport** | Jump directly to `$HOME` or enter custom path |
| `/` | **Spotlight** | Real-time substring filter (`ESC` clears) |
| `o` | **Sort Cycle** | Cycle: Size (Desc) $\to$ Size (Asc) $\to$ Name (A-Z) $\to$ Date |
| `p` | **Quick Look** | macOS Quick Look in-terminal file preview |
| `s` / `u` | **Goinfre** | Move to `/goinfre` & symlink / Restore back to `$HOME` |
| `H` | **Healer** | Repair broken `/goinfre` symlinks on workstation switch |
| `C` / `K` | **Cleaners** | Open 9-Tier Cleaning Presets Menu / One-Shot Junk Nuke |
| `T` / `Z` | **Storage** | Purge Desktop Trash / Inject Quota Bypass in `~/.zshrc` |
| `e` / `t` / `!` | **Tools** | Open `$EDITOR` / Interactive Subshell / Exec bash command |
| `Space` / `v` / `U` | **Batch** | Mark item / Invert marks / Clear all marked items |
| `d` / `x` | **Safe Delete** | Safe deletion with confirmation modal and protected file guard |
| `E` | **Report** | Export top 30 disk consumers audit to `quota_report.md` |
| `q` | **Quit** | Exit `ft_ncdu` cleanly |

---

## 💻 Headless CLI Commands

```bash
# Run 9-Tier Native C Fast Cleaner (<0.2s)
clean42
ntcl13
ft_ncdu -c

# Dry-run simulation (No files removed)
clean42 --dry-run
ft_ncdu -c --dry-run
ft_ncdu -n

# Run Station Healer in headless CLI
clean42 --heal
ft_ncdu --heal

# Relocate heavy toolchains to /goinfre
ft_ncdu --bootstrap

# Print formatted quota report to stdout
ft_ncdu --report
```

---

## 📚 Architectural Masterclass Guides (`docs/`)

The repository includes a 4-part systems programming curriculum designed for 42 students:
* 🏛️ **[01_ARCHITECTURE.md](docs/01_ARCHITECTURE.md)**: Concurrency model, Pthread worker pool, POSIX system calls.
* 📂 **[02_FILE_BY_FILE_GUIDE.md](docs/02_FILE_BY_FILE_GUIDE.md)**: Exhaustive line-by-line anatomy of every source file.
* 🎓 **[03_HOW_TO_BUILD_THIS.md](docs/03_HOW_TO_BUILD_THIS.md)**: Step-by-step tutorial on building a systems utility from scratch.
* 📐 **[04_NORMINETTE_PATTERNS.md](docs/04_NORMINETTE_PATTERNS.md)**: Advanced design patterns for 100% 42 Norminette compliance.

---

## 📜 42 Norminette Standard Verification

Every single line of C code strictly adheres to **Norminette v3.3.59**:
```bash
make norm
```
*Output: Zero errors across all 19 `.c` files and 3 `.h` headers!*

---

## 👤 Author
Developed with ❤️ by **tlaghzal** for **1337 Coding School & 42 Network** cadets worldwide.
GitHub: [https://github.com/laghzal49/ft_ncdu](https://github.com/laghzal49/ft_ncdu)
