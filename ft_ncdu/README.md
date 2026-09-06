# ft_ncdu & clean42 / ntcl13
### The Ultimate High-Performance Cluster Storage Suite for 1337 & 42 Network

![Norminette](https://img.shields.io/badge/Norminette-Passing-success)
![Language](https://img.shields.io/badge/Language-C99%20%7C%20POSIX%20%7C%20Python-blue)
![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Linux-lightgrey)
![Threads](https://img.shields.io/badge/Threads-16-orange)
![License](https://img.shields.io/badge/License-MIT-green)

`ft_ncdu` (and its aliases `clean42` / `ntcl13`) is an ultra-fast, robust, and feature-rich cluster storage management suite specifically engineered for the 42 Network and 1337 Coding School environments. Born out of the necessity to manage limited NFS quotas, it empowers students to instantly analyze disk usage, effortlessly purge gigabytes of cached junk across 9 intelligent tiers, and seamlessly migrate massive directories to local `/goinfre` storage while keeping configurations intact.

---

## ⚡ Quick Install

Get up and running instantly with our one-liner curl installation script:

```bash
curl -fsSL https://raw.githubusercontent.com/laghzal49/ft_ncdu/main/install.sh | bash
```

The interactive installer prompts you to install the TUI, GUI, or both. Each
interface has a stable command: `ft_ncdu-tui` and `ft_ncdu-gui`. The `ft_ncdu`
command starts the interface selected as the default during installation.

For automated installations, set `FT_NCDU_MODE` to `tui`, `gui`, or `both`.
When installing both, set `FT_NCDU_DEFAULT` to `tui` or `gui`:

```bash
FT_NCDU_MODE=both FT_NCDU_DEFAULT=gui \
  bash -c "$(curl -fsSL https://raw.githubusercontent.com/laghzal49/ft_ncdu/main/install.sh)"
```

The installation process compiles the C core natively, deploys the Python GUI securely, and installs all binaries and aliases directly into your `~/.local/bin/` directory.

---

## 🖥️ Three Interfaces, One Suite

### Terminal UI (TUI)
- **Native C ncurses interface** for blazing-fast, lightweight operation over SSH or local terminal.
- **16-thread async scanning** maps your entire home directory in milliseconds.
- **Key features:** Deep navigation, dynamic sorting, live search, advanced inspector panel, and a real-time storage gauge.

```text
┌─ ft_ncdu ───────────────────────────────────────────────┐
│ /home/user/1337-space                       [ 45.2 GB ] │
├─────────────────────────────────────────────────────────┤
│   12.4 GB [##########          ] /docker-volumes        │
│    8.2 GB [######              ] /.cache                │
│    5.1 GB [####                ] /projects              │
│    2.0 GB [#                   ] /node_modules          │
└─────────────────────────────────────────────────────────┘
```

### Desktop GUI (Python/Tk)
- **Modern dark-themed Tkinter application** providing a rich desktop experience.
- **Full feature parity with TUI (NEW!)** ensuring you never miss out on functionality.
- **Features:** Intuitive menu bar, persistent inspector panel, one-click cleaning presets, visual goinfre management, bootstrap wizard, station healer, inline file preview, report export, and batch operations.
- Run with: `ft_ncdu-gui` or `python3 scripts/ft_ncdu_gui.py`

### Headless CLI
- **Zero-UI batch operations** for rapid cleanup directly from your shell prompt.
- **Commands:** `clean42`, `ntcl13`, `ft_ncdu -c`, `--heal`, `--bootstrap`, `--report`
- **Dry-run mode:** Test the waters safely with `--dry-run` or `-n`.

---

## 🚀 Feature Matrix

| Feature | TUI | GUI | CLI |
|---------|-----|-----|-----|
| 16-Thread Async Scanner | ✅ | ✅ | — |
| 9-Tier Cleaning Presets | ✅ | ✅ | ✅ |
| Goinfre Symlink/Restore | ✅ | ✅ | — |
| Bootstrap to Goinfre | ✅ | ✅ | ✅ |
| Station Healer | ✅ | ✅ | ✅ |
| File Preview | ✅ | ✅ | — |
| Report Export | ✅ | ✅ | ✅ |
| Batch Mark/Delete | ✅ | ✅ | — |
| Inspector Panel | ✅ | ✅ | — |
| Storage Gauge | ✅ | ✅ | — |
| Protected Path Guards | ✅ | ✅ | — |
| Audit Trail Logging | ✅ | ✅ | ✅ |
| Dry-Run Mode | — | — | ✅ |
| Search/Filter | ✅ | ✅ | — |

---

## 🧹 9-Tier Native Cleaner

Free up gigabytes safely with specialized cleaning tiers tailored for developers:

1. **42 C/C++ Dev Output** (`*.o`, `*.a`, `*.dSYM`, `vgcore.*`, `clangd`) - Purge compiled objects and debugging symbols.
2. **Francinette & Testers** (`francinette/temp`, `logs`, `gcda/gcno`) - Clear out heavy automated tester remnants.
3. **AI & Python ML Caches** (`__pycache__`, `huggingface`, `torch`, `pip`, `ollama`) - Reclaim space from massive AI models and pip caches.
4. **Web & Transcendence** (`node_modules`, `npm`, `yarn`, `.next`, `.turbo`) - Wipe out endless JS dependency trees.
5. **Browser & Electron** (`Chrome`, `Chromium`, `Brave`, `Firefox`, `VSCode`, `Discord`, `Slack`) - Safely flush cache from hungry GUI apps.
6. **Docker Cluster Prune** (`docker system prune -a --volumes -f`) - Annihilate unused containers, images, and volumes.
7. **Norminette & Shell Logs** (`norminette cache`, `zcompdump`, `xsession-errors`) - Clean up system and linter debris.
8. **Desktop Trash Bin** (`Trash`, `.Trash`) - Empty your OS recycle bin.
9. **Nuclear 1337 Wipe** - The ultimate purge. Executes all tiers simultaneously for maximum quota recovery.

---

## 🩹 Station-Hopping Healer & Goinfre

Embrace the **living manifest paradigm**. In cluster environments, your NFS home follows you, but `/goinfre` is strictly local to the machine.

- **The Manifest:** Symlinks in your `$HOME` act as a manifest of relocated tools (e.g., Docker, Code, Caches).
- **Healer (`H` key or `--heal`):** Instantly repairs broken `/goinfre` symlinks when you log into a new workstation by recreating the destination structures locally.
- **Bootstrap (`b` key or `--bootstrap`):** Intelligently relocates standard heavy directories (like `~/.cache`, `~/.docker`, `~/.vscode-server`) to `/goinfre` and sets up the symlinks automatically.
- **ZSH Integration:** Automatically injects necessary `export` overrides into your `.zshrc` to ensure tools respect the new `/goinfre` paths.

---

## 🛡️ Safety & Security

- **Protected Path Guards:** Hardcoded safeguards prevent accidental deletion of critical system directories (`/`, `/home`, `/tmp`, etc.).
- **Shell Argument Escaping:** Robust sanitization of filenames before they hit any system call.
- **Audit Trail Logging:** Every deletion and migration is logged to `~/.ft_ncdu_cleanup.log` for transparency.
- **Cross-Filesystem Mount Boundary Protection:** POSIX `st_dev` checks ensure recursive operations don't bleed into external mounts (like USBs or other NFS shares).
- **Dry-Run Mode:** See exactly what *would* be deleted before committing.
- **Ownership Validation:** Strict permission checks during the bootstrap process ensure structural integrity.

---

## 🎮 Keyboard Shortcuts (TUI)

| Key | Action | Description |
|-----|--------|-------------|
| `↑` / `k` | Up | Move cursor up |
| `↓` / `j` | Down | Move cursor down |
| `Enter` / `l` | Enter | Open directory |
| `Backspace` / `h` | Back | Go up to parent directory |
| `q` | Quit | Exit the application |
| `/` | Search | Live filter current directory |
| `s` | Sort | Toggle sorting by Size / Name / Date |
| `c` | Clean Menu | Open the 9-Tier cleaning preset menu |
| `g` | Goinfre Menu | Manage symlinking current dir to /goinfre |
| `H` | Healer | Run the Station Healer to fix broken links |
| `b` | Bootstrap | Run standard /goinfre bootstrap wizard |
| `m` | Mark | Mark/unmark current file for batch action |
| `M` | Mark All | Mark/unmark all files in directory |
| `d` | Delete | Delete marked files (or current if none marked) |
| `p` | Preview | Quick peek at text file contents |
| `e` | Editor | Open file in $EDITOR (or vim) |
| `t` | Terminal | Spawn a shell in the current directory |
| `r` | Report | Generate a comprehensive storage report |
| `?` | Help | Show all keyboard shortcuts |

---

## 🖱️ GUI Quick Reference

The Desktop GUI provides intuitive drop-down menus for all features:

- **File Menu:** Refresh current view, open new paths, quit the application.
- **Edit Menu:** Mark selected, mark all, clear marks.
- **View Menu:** Toggle hidden files, change sorting mode, switch size display (Bytes/Human).
- **Actions Menu:** Execute deletions, run 9-tier cleaning presets, nuke junk files, trigger Docker prune.
- **Cluster Menu:** Symlink to goinfre, run the Station Healer, trigger the Bootstrap wizard, empty trash, manage zshrc exports.
- **Tools Menu:** Open file previews, launch system editor, open terminal here, export storage reports.

---

## 💻 CLI Commands

For the power users who live in the shell:

```bash
# Run the selected default interface
ft_ncdu

# Run a specific interface
ft_ncdu-tui [directory]
ft_ncdu-gui

# Run the Desktop GUI
ft_ncdu-gui

# Run standard clean preset immediately (no UI)
clean42 
# or
ntcl13 
# or 
ft_ncdu -c

# Run standard clean but just show what would happen (Dry Run)
clean42 --dry-run
# or
ft_ncdu -n

# Run the Station Healer (fix goinfre links)
ft_ncdu --heal

# Run the full Goinfre Bootstrap
ft_ncdu --bootstrap

# Generate a storage report for a specific path
ft_ncdu --report [path]
```

---

## 🏗️ Architecture

```text
+-------------------+
|      main.c       | (Entry point, args parsing)
+---------+---------+
          |
+---------v---------+
|    Core Engine    | -> 16 worker threads, strided indexing
| (Scanner, Nav)    | -> Fixed 65,536 entry dual-buffer
+---------+---------+ -> POSIX st_dev mount shielding
          |
+---------+---------+-------+-------------------+
|                   |       |                   |
v                   v       v                   v
UI Subsystem     Action    Utils              CLI
(ncurses TUI,    Subsystem (Security,        (Batch,
 HUD, Events)    (Delete,   Format,           Healer,
                 Goinfre)   Memory)           Presets)
```

- **16 Worker Threads:** Utilizes a strided indexing model to prevent lock contention while recursively reading directories.
- **Fixed 65,536 Entry Dual-Buffer:** Ensures highly predictable memory footprint.
- **Zero Dynamic Heap Allocation Per File:** Drastically reduces `malloc` overhead during deep scans.
- **Mutex-Protected Shared State:** Thread-safe state transitions across the engine lifecycle.

---

## 🔨 Building from Source

To compile and install manually:

```bash
git clone https://github.com/laghzal49/ft_ncdu.git
cd ft_ncdu
make
make install
```

**Dependencies:** a C compiler, `make`, ncurses development headers, and
Python 3.6 or newer with Tk for the GUI. The installer supports apt, dnf/yum,
pacman, zypper, apk, and Homebrew on macOS.

---

## 📚 Documentation

Dive deeper into the internals in the `docs/` directory:
- `01_ARCHITECTURE.md` - Core engine and threading model
- `02_FILE_BY_FILE_GUIDE.md` - Complete source code map
- `03_HOW_TO_BUILD_THIS.md` - Build system and CI/CD
- `04_NORMINETTE_PATTERNS.md` - How we bypassed 42's strict rules

---

## 📜 Norminette

```bash
make norm
```
100% passing across all C source files according to the latest 42 Network V3 Norme.

---

## 👤 Author

Developed by **tlaghzal** for 1337 Coding School & 42 Network.  
GitHub: [https://github.com/laghzal49/ft_ncdu](https://github.com/laghzal49/ft_ncdu)
