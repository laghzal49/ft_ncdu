# ft_ncdu

A responsive disk-usage explorer and storage toolkit for Linux, macOS, and
42/1337 workstations. Choose a fast ncurses terminal interface, a Python Tk
desktop interface, or install both.

## Install

```bash
curl -fsSL https://raw.githubusercontent.com/laghzal49/ft_ncdu/main/install.sh | bash
```

The installer asks which interface to install before downloading dependencies
or compiling:

- **TUI**: native C and ncurses, installed as `ft_ncdu`
- **GUI**: Python Tk desktop app, installed as `ft_ncdu`
- **Both**: TUI as `ft_ncdu`, GUI as `ft_ncdu-gui`

For a non-interactive installation:

```bash
FT_NCDU_MODE=tui bash install.sh
FT_NCDU_MODE=gui bash install.sh
FT_NCDU_MODE=both bash install.sh
```

The installer supports apt, dnf, pacman, zypper, apk, and macOS command-line
tools. It installs into `~/.local/bin` and adds that directory to the active
shell profile. It does not run cleaners automatically at login.

## Terminal UI

The TUI adapts from compact `30x10` terminals to wide desktop terminals. It
uses ASCII for content, terminal-native line drawing, reverse-video selection,
and a details panel only when enough space is available.

```bash
ft_ncdu
ft_ncdu /path/to/inspect
```

| Key | Action |
| --- | --- |
| `j`, `k`, arrows | Move selection |
| `PgUp`, `PgDn` | Move by a full visible page |
| `Enter`, `l` | Open directory |
| `h`, Backspace | Open parent |
| `/` | Filter entries |
| `o` | Change sort mode |
| `Space` | Mark an entry |
| `d` | Delete with confirmation |
| `s`, `u` | Move to goinfre or restore |
| `?` | Show all shortcuts |
| `q` | Quit |

## Desktop GUI

The Tk GUI scans in a background thread so the window stays responsive. It
includes directory navigation, search, sortable columns, hidden-file control,
disk-space status, native scrolling, and read-only inspection.

```bash
ft_ncdu-gui
```

When only the GUI is installed, launch it with `ft_ncdu`.

## CLI

```bash
ft_ncdu --help
ft_ncdu --report /path
ft_ncdu --heal
ft_ncdu --bootstrap
clean42 --dry-run
```

`--bootstrap` safely relocates Hugging Face, PyTorch, Triton, Whisper, Ollama,
pip, uv, Node, Rust, Docker, and Flatpak data to goinfre storage. Conflicting
source and destination data is preserved for manual review.

## Source Layout

The application source is in [`ft_ncdu/`](ft_ncdu/). The repository-level
Makefile and installer delegate to that directory.

```bash
make
make norm
make install
```
