# ft_ncdu

A blazing-fast, multi-threaded disk usage analyzer and storage lifecycle manager built specifically for **1337 / 42 Network cluster machines** using **C** and **ncurses**.

Designed to conquer tight NFS `$HOME` directory storage limits, inode exhaustion, and multi-station `/goinfre` symlink management without external runtime dependencies.

---

## Features

* **Multi-Threaded POSIX Engine:** Recursive directory calculation distributed across `pthread` worker pools for instant scans on multi-gigabyte trees.
* **Dual Quota & Inode Telemetry:** Real-time visual progress bars tracking NFS `$HOME` byte limits, file count (`inodes`), and local NVMe `/goinfre` storage.
* **Station-Switch Symlink Healer (`H`):** Automatically detects broken/dangling symlinks when switching physical iMac/Linux workstations and recreates missing `/goinfre` directory trees on the local machine.
* **1-Key Toolchain Bootstrapper (`b`):** Automatically moves and symlinks heavy caches to `/goinfre`:
  * **AI / ML:** Hugging Face (`~/.cache/huggingface`), PyTorch (`~/.cache/torch`), Pip (`~/.cache/pip`), Ollama (`~/.ollama`), Triton (`~/.triton`)
  * **Dev Engines:** Rust/Cargo (`~/.cargo`, `~/.rustup`), Docker (`~/.docker`), NPM (`~/.npm`), VS Code Extensions (`~/.vscode/extensions`), macOS/Linux Caches (`~/Library/Caches`, `~/.cache`)
* **Neovim / Vim Keymap:** Pure muscle-memory navigation (`j`, `k`, `h`, `l`, `gg`, `G`, `/`, `dd`, `x`).
* **Multi-Item Batch Selection (`<Space>`):** Select multiple files and folders to batch-delete or batch-symlink to `/goinfre`.
* **Deep Cleaners:**
  * `K`: Purges build artifacts (`*.o`, `*.a`, `*.out`, `.dSYM`, `core.*`, `vgcore.*`), Python caches (`__pycache__`, `.pytest_cache`), and `node_modules`.
  * `G`: Git Doctor runs `git clean -fdx` and `git gc --prune=now --aggressive` across all repos in the tree.
  * `D`: Full Docker prune (`docker system prune -a --volumes -f`).
* **Safety Whitelist:** Built-in safeguards prevent accidental deletion of critical shell and system configurations (`.ssh`, `.zshrc`, `.bashrc`, `.gitconfig`, `.vimrc`).

---

## Project Structure

```text
ft_ncdu/
├── Makefile
├── includes/
│   └── ft_ncdu.h
└── src/
    ├── main.c        # Event loop, initialization, key dispatch
    ├── scanner.c     # Multi-threaded POSIX file tree scanner & sorter
    ├── actions.c     # Cluster cleanups, symlink healer, bootstrapper
    ├── ui.c          # ncurses dashboard, color palettes, inspector
    └── utils.c       # Safe string utilities, formatting, permission parsers
