#  ft_ncdu & clean42 / ntcl13
### *The Ultimate High-Performance macOS-Themed Cluster Storage Suite for 1337 & 42 Network*

[![Norminette](https://img.shields.io/badge/Norminette-100%25%20Passing-brightgreen.svg)](https://github.com/42School/norminette)
[![Language](https://img.shields.io/badge/Language-C99%20%7C%20POSIX-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Threads](https://img.shields.io/badge/Concurrency-16%20Threads-blueviolet.svg)](#)

A blazing-fast, native C terminal disk usage analyzer, automated cluster quota optimizer, and workstation healer designed specifically for **1337 Coding School** and **42 Network** students.

---

## ⚡ 1-Line Quick Install

Run this in your terminal on any 42 iMac or Linux workstation:

```bash
curl -fsSL https://raw.githubusercontent.com/laghzal49/ft_ncdu/main/install.sh | bash
```

*Auto-clones, compiles with `-O3 -march=native`, installs to `~/.local/bin/`, configures `$PATH`, and sets up shell aliases (`clean`, `space`, `heal`).*

---

## 🚀 Key Highlights & Capabilities

```
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

### 1. ⚡ 16-Thread Asynchronous Crawler
* Spawns 16 worker threads with strided indexing to scan 65,000+ files in milliseconds without freezing the UI.
* Built-in **POSIX `st_dev` mount boundary shielding** prevents crawler from traversing into `/proc`, `/sys`, or external network mounts.

### 2. 🧹 9-Tier Native C Fast Cleaner (`clean42` / `ntcl13`)
Re-engineered natively in C to execute in $<0.2\text{s}$ with before/after `statvfs()` disk space reporting:
* **Tier 1**: C/C++ build artifacts (`*.o`, `*.a`, `*.so`, `*.dSYM`, `core.*`).
* **Tier 2**: Francinette & Unit Testers (`~/francinette/temp`, `.francinette/logs`, `valgrind*.log`, `*.gcda`).
* **Tier 3**: AI & PyTorch models (`~/.cache/huggingface`, `~/.cache/torch`, `~/.cache/pip`).
* **Tier 4**: Node.js & Web (`node_modules`, `~/.npm`, `~/.yarn/cache`).
* **Tier 5**: Browser caches (Chrome, Chromium, Brave, Mozilla).
* **Tier 6**: IDE caches (VS Code cache, extensions cache, JetBrains).
* **Tier 7**: Docker & Containers (`docker system prune -a --volumes -f`).
* **Tier 8**: Norminette logs & temp state (`~/.norminette/logs`, `/tmp/*_${USER}`).
* **Tier 9**: Desktop Trash & Thumbnails (`~/.local/share/Trash`, `~/.cache/thumbnails`).

### 3. 🩹 Station-Hopping Healer (`H`) & Goinfre Engine
* `[s]` **Goinfre Symlinker**: Moves target project to `/goinfre/$USER/` and creates a symlink on `$HOME`.
* `[u]` **Goinfre Unlinker**: Restores data back from `/goinfre` to `$HOME` and deletes symlink.
* `[H]` **Station Healer**: Auto-detects broken `/goinfre` symlinks when changing physical iMacs/PCs and recreates missing target directories so applications don't crash.
* `[b]` **Auto-Bootstrapper**: Moves heavy caches (`.cargo`, `.rustup`, `.npm`, `.docker`) to `/goinfre`.
* `[Z]` **Quota Bypass Injector**: Injects persistent environment exports into `~/.zshrc`.

### 4. 🍏 macOS-Themed NCurses TUI
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
# Run 9-Tier Native C Fast Cleaner
clean42
ntcl13
ft_ncdu -c

# Run Station Healer in headless CLI
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
Developed with ❤️ by **laghzal** for **1337 Coding School & 42 Network** cadets worldwide.
