# 🚀 ft_ncdu — 42 & 1337 Cluster Storage Suite

[![42 Network](https://img.shields.io/badge/42-Network-black?style=for-the-badge&logo=42)](https://42.fr)
[![1337 Coding School](https://img.shields.io/badge/1337-Coding%20School-00babc?style=for-the-badge)](https://1337.ma)
[![Language](https://img.shields.io/badge/C-C99%20%7C%20POSIX-blue?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![NCurses](https://img.shields.io/badge/TUI-NCurses%20%26%20Pthreads-green?style=for-the-badge)](https://invisible-island.net/ncurses/)
[![License](https://img.shields.io/badge/License-MIT-purple?style=for-the-badge)](LICENSE)

> **The ultimate high-performance NCurses disk analyzer, cluster quota manager, and cache optimizer tailored specifically for 42 Network and 1337 Coding School cadets.**

---

## ⚡ The Problem: 42 & 1337 Storage Traps

At 42 / 1337 campuses (Benguerir, Khouribga, Med, Paris, Lyon, etc.), students face strict quota limits (usually **5 GB to 30 GB** roaming NFS profiles). Exceeding this quota leads to:

- ❌ **Account Lockouts** & inability to log in to display managers.
- ❌ **VS Code & Browser Crashes** (`No space left on device`).
- ❌ **Compilation Errors** during C projects, Norminette checks, and Docker builds.
- ❌ **Station Hopping Nightmares**: Moving to a new iMac/PC leaves broken symlinks because `/goinfre` is local to each station.
- ❌ **Hidden Bloat Traps**: `~/.local/share/Trash`, `.cache/huggingface`, `node_modules`, `vgcore.*`, and `.docker`.

**`ft_ncdu` was engineered from the ground up to eliminate quota anxiety forever.**

---

## 🌟 Key Features

| Feature | Description |
| :--- | :--- |
| **🚀 16-Thread Fast Engine** | Multi-threaded recursive scanner with live size streaming. Handles **65,536+ files** without freezing. |
| **💾 Goinfre Ecosystem** | Auto-detects `/goinfre/$USER`, `/sgoinfre/$USER`, macOS `/Volumes/Storage/goinfre`, and `/tmp/goinfre`. |
| **🩹 Station Healer (`H`)** | Automatically repairs dangling goinfre symlinks when you switch to a new physical workstation. |
| **🔄 Goinfre Unlinker (`u`)** | Seamlessly moves directories and files back from `/goinfre` into `$HOME` and deletes the symlink. |
| **🗑️ 1-Click Trash Purger (`T`)** | Wipes `~/.local/share/Trash` and `~/.Trash` (the #1 hidden cause of 100% quota lockouts). |
| **🧹 8 Modular Presets (`C`)** | One-key cleanup for 42 C/C++, Web/Transcendence, AI/PyTorch, Browser caches, Docker, and Logs. |
| **👁️ Scrollable Peek (`p`)** | In-terminal scrollable file content viewer with line numbers, `PageUp`/`PageDown`, and binary detection. |
| **🐚 Quota Bypass Injector (`Z`)** | Injects redirect environment variables (`HF_HOME`, `CARGO_HOME`, etc.) and aliases into `~/.zshrc`. |
| **📊 Real-time Telemetry HUD** | Live visual gauge bars for Home Quota, Inode saturation, Goinfre space, and **>85% Quota Danger Alerts**. |
| **⚡ Headless CLI Mode** | Run cleanups, healing, bootstrapping, and reports directly from scripts or terminal one-liners. |

---

## 🎮 Interactive Keyboard Cheatsheet

### 🧭 Navigation & Browsing
| Key | Action |
| :--- | :--- |
| `j` / `k` / `↓` / `↑` | Navigate up / down in the file list |
| `PageDown` / `PageUp` | Fast scroll viewport up / down |
| `g` / `Home` | Jump to top (first item) |
| `G` / `End` | Jump to bottom (last item) |
| `l` / `Enter` / `→` | Step into directory |
| `h` / `Backspace` / `←` / `-` | Step out to parent directory |
| `~` | Instant jump to `$HOME` |
| `P` or `:` | Teleport jump to arbitrary path prompt |

### 🛠️ 42 / 1337 Storage Powers
| Key | Action |
| :--- | :--- |
| `s` | **Move to Goinfre & Symlink**: Moves highlighted (or marked) item to `/goinfre/$USER` and links it |
| `u` | **Unlink from Goinfre**: Restores real data from `/goinfre` back into `$HOME` and removes symlink |
| `H` | **Station Healer**: Scans `$HOME` and repairs all broken `/goinfre` symlinks for the current station |
| `b` | **Magic Bootstrap**: Auto-relocates heavy toolchains (AI models, Rust, Docker, NPM) to `/goinfre` |
| `T` | **Empty Trash**: Permanently purges `~/.local/share/Trash` and `~/.Trash` |
| `C` | **Cleaning Presets Menu**: Opens interactive picker with 8 specialized 42 cleaning presets |
| `K` | **Quick Nuke Junk**: Instantly cleans build outputs, `.dSYM`, `.cache`, `vgcore.*`, `__pycache__` |
| `Z` | **Inject ~/.zshrc**: Appends quota bypass exports (`HF_HOME`, `DOCKER_CONFIG`, etc.) & aliases |
| `G` | **Git Doctor**: Runs `git clean -fdx` and `git gc --aggressive` across all repos in subtree |
| `D` | **Docker Prune**: Executes full `docker system prune -a --volumes -f` |

### 🔍 Selection, View & Inspection
| Key | Action |
| :--- | :--- |
| `Space` | Toggle mark on current item (Batch Mode) |
| `v` or `M` | Invert selection marks on all items |
| `U` | Clear / unmark all items |
| `d` / `dd` / `x` | Delete selected item or all marked items (with safety confirmation modal) |
| `p` | **Scrollable Peek**: Preview file contents with line numbering and scroll controls |
| `/` | **Instant Search**: Type substring to filter in real-time (`ESC` to clear) |
| `A` | Toggle **Apparent Size** vs. **Actual Disk Block Allocation** |
| `a` | Toggle hidden dotfiles visibility (Show / Hide `.*`) |
| `o` | Cycle sorting mode (`Size Desc` → `Size Asc` → `Name` → `Date`) |
| `r` | Refresh / Rescan current directory |
| `e` | Open highlighted file/folder in `$EDITOR` (Neovim, Vim, Nano, VS Code) |
| `t` | Drop into an interactive subshell at current directory |
| `!` | Execute custom shell command with selected target path |
| `E` | Export comprehensive Markdown storage audit to `quota_report.md` |
| `?` | Open interactive Help Reference Modal |
| `q` | Quit `ft_ncdu` |

---

## 🧹 Modular 42 Cleaning Presets (`C`)

Press `C` inside the TUI (or run `ft_ncdu --clean`) to access targeted cleaning modules:

1. **`[1] 42 C / C++ Dev Output`** — Wipes `*.o`, `*.a`, `*.dSYM`, `*.out`, `vgcore.*`, `core.*`, `.cache/clangd`, and `CMakeCache.txt`.
2. **`[2] Francinette & Testers`** — Wipes `~/francinette/temp`, `.francinette/logs`, `tests/valgrind*.log`, `*.gcda`, and `*.gcno`.
3. **`[3] AI & Python ML Caches`** — Wipes Hugging Face hub models, PyTorch checkpoints, Triton cache, Ollama temps, `__pycache__`, `.pytest_cache`, and pip cache.
4. **`[4] Web & 42 Transcendence`** — Purges `node_modules`, `.next`, `.turbo`, and global caches for NPM (`_cacache`), Yarn, and pnpm.
5. **`[5] Browser & Electron Bloat`** — Wipes GPU caches, ServiceWorker caches, and code caches for Google Chrome, Chromium, Brave, VS Code (`.config/Code/Cache*`), Slack, Discord, and Spotify.
6. **`[6] Docker Cluster Storage`** — Runs full `docker system prune -a --volumes -f` and removes desktop cache.
7. **`[7] Norminette & Cluster Logs`** — Clears Norminette cache, X11 session logs, `~/.zcompdump*`, and Valgrind logs.
8. **`[8] Empty Trash Bin`** — Purges desktop trash bin (`~/.local/share/Trash` and `~/.Trash`).
9. **`[9] Nuclear 1337 Wipe`** — One-shot safe wipe across all safe cache and build categories!


---

## 💻 Headless CLI Automation


### Headless CLI Automation

`ft_ncdu` can be run non-interactively in scripts, cronjobs, or `.zshrc` aliases:

```bash
# Launch interactive TUI in specific directory
ft_ncdu ~/Projects

# Run native C fast cluster cleaner (ntcl13 / clean42)
ntcl13
# or
clean42
# or
ft_ncdu --clean

# Heal broken goinfre symlinks after changing workstations
ft_ncdu --heal

# Bootstrap toolchains to goinfre headlessly
ft_ncdu --bootstrap

# Print storage quota audit report to stdout
ft_ncdu --report

# Display CLI help
ft_ncdu --help
```

---

## 📦 Compilation & Installation

### Requirements
- GCC or Clang
- NCurses (`libncurses-dev` or `ncurses-devel`)
- Pthreads

### Build & Run
```bash
# Clone repository
git clone https://github.com/laghzal49/ft_ncdu.git
cd ft_ncdu

# Compile optimized binary with 42 Norminette compliance
make

# Verify 100% 42 Norminette standard
make norm

# Install ft_ncdu, ntcl13, and clean42 to ~/.local/bin
make install

# Run anywhere
ft_ncdu
# or run fast native C cleaner
ntcl13
```


---

## 💡 42 Cadets Pro-Tips

> [!TIP]
> **Station Switching**: Every time you change iMac/PC station at school, run `ft_ncdu --heal` or press `H` inside `ft_ncdu` to immediately restore all your goinfre symlinks on the new workstation!

> [!TIP]
> **Quota Protection**: Press `Z` once to inject quota redirect exports into your `~/.zshrc`. This permanently redirects heavy toolchain downloads (HuggingFace, PyTorch, Cargo, Rust, Docker, NPM, Pip) into `/goinfre/$USER` instead of filling your network home!

> [!IMPORTANT]
> **Safety Guard**: Critical configuration files (`.ssh`, `.zshrc`, `.bashrc`, `.gitconfig`, `.vimrc`, `.config/nvim`, etc.) are protected from accidental deletion.

---

## 📜 License

Distributed under the **MIT License**. Built with ❤️ for **42 Network** and **1337 Coding School** cadets.
