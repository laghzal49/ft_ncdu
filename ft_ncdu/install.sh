#!/usr/bin/env bash

set -eu

REPO_URL="https://github.com/laghzal49/ft_ncdu.git"
INSTALL_DIR="${HOME}/.local/bin"
TEMP_DIR="${TMPDIR:-/tmp}/ft_ncdu_install_$$"
OS_TYPE="$(uname -s)"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"

if [ -t 1 ]; then
	CYAN='\033[0;36m'
	GREEN='\033[0;32m'
	YELLOW='\033[1;33m'
	RED='\033[0;31m'
	BOLD='\033[1m'
	RESET='\033[0m'
else
	CYAN=''
	GREEN=''
	YELLOW=''
	RED=''
	BOLD=''
	RESET=''
fi

say()
{
	printf '%b\n' "$*"
}

cleanup()
{
	if [ -d "$TEMP_DIR" ]; then
		rm -rf "$TEMP_DIR"
	fi
}

run_as_root()
{
	if [ "$(id -u)" -eq 0 ]; then
		"$@"
	elif command -v sudo >/dev/null 2>&1; then
		sudo "$@"
	else
		say "${RED}Missing build dependencies and sudo is unavailable.${RESET}"
		return 1
	fi
}

install_linux_dependencies()
{
	if [ "$MODE" = "gui" ]; then
		APT_PACKAGES="python3 python3-tk git"
		DNF_PACKAGES="python3 python3-tkinter git"
		PACMAN_PACKAGES="python tk git"
		ZYPPER_PACKAGES="python3 python3-tk git"
		APK_PACKAGES="python3 py3-tkinter git"
	else
		APT_PACKAGES="build-essential libncurses-dev git"
		DNF_PACKAGES="gcc make ncurses-devel git"
		PACMAN_PACKAGES="base-devel ncurses git"
		ZYPPER_PACKAGES="gcc make ncurses-devel git"
		APK_PACKAGES="build-base ncurses-dev git"
		if [ "$MODE" = "both" ]; then
			APT_PACKAGES="$APT_PACKAGES python3 python3-tk"
			DNF_PACKAGES="$DNF_PACKAGES python3 python3-tkinter"
			PACMAN_PACKAGES="$PACMAN_PACKAGES python tk"
			ZYPPER_PACKAGES="$ZYPPER_PACKAGES python3 python3-tk"
			APK_PACKAGES="$APK_PACKAGES python3 py3-tkinter"
		fi
	fi
	if command -v apt-get >/dev/null 2>&1; then
		run_as_root apt-get update
		run_as_root apt-get install -y $APT_PACKAGES
	elif command -v dnf >/dev/null 2>&1; then
		run_as_root dnf install -y $DNF_PACKAGES
	elif command -v pacman >/dev/null 2>&1; then
		run_as_root pacman -Sy --needed --noconfirm $PACMAN_PACKAGES
	elif command -v zypper >/dev/null 2>&1; then
		run_as_root zypper --non-interactive install $ZYPPER_PACKAGES
	elif command -v apk >/dev/null 2>&1; then
		run_as_root apk add $APK_PACKAGES
	else
		say "${RED}Unsupported package manager.${RESET} Install a C compiler, make, ncurses headers, and git."
		return 1
	fi
}

dependencies_ready()
{
	if [ "$NEEDS_CLONE" -eq 1 ]; then
		command -v git >/dev/null 2>&1 || return 1
	fi
	if [ "$MODE" != "gui" ]; then
		command -v cc >/dev/null 2>&1 || return 1
		command -v make >/dev/null 2>&1 || return 1
		printf '#include <ncurses.h>\n' | cc -E - >/dev/null 2>&1 || return 1
	fi
	if [ "$MODE" != "tui" ]; then
		command -v python3 >/dev/null 2>&1 || return 1
		python3 -c 'import tkinter' >/dev/null 2>&1 || return 1
	fi
	return 0
}

ensure_dependencies()
{
	if dependencies_ready; then
		return
	fi
	say "${YELLOW}==>${RESET} Installing required build tools..."
	if [ "$OS_TYPE" = "Darwin" ]; then
		if ! xcode-select -p >/dev/null 2>&1; then
			xcode-select --install
			say "Finish the Apple command line tools installation, then rerun this installer."
			exit 1
		fi
		if [ "$MODE" != "tui" ] \
			&& ! python3 -c 'import tkinter' >/dev/null 2>&1; then
			if command -v brew >/dev/null 2>&1; then
				brew install python-tk
			else
				say "Install Python 3 with Tk from python.org, then rerun."
				exit 1
			fi
		fi
	else
		install_linux_dependencies
	fi
	if ! dependencies_ready; then
		say "${RED}Dependencies are still unavailable.${RESET}"
		exit 1
	fi
}

choose_mode()
{
	MODE="${FT_NCDU_MODE:-}"
	if [ -z "$MODE" ] && ( : < /dev/tty ) 2>/dev/null; then
		printf '\nChoose an interface:\n  1) TUI (terminal)\n' > /dev/tty
		printf '  2) GUI (Python Tk)\n  3) Both\nSelection [1]: ' > /dev/tty
		read -r MODE_CHOICE < /dev/tty || MODE_CHOICE=1
		case "$MODE_CHOICE" in
			2|gui|GUI) MODE="gui" ;;
			3|both|Both) MODE="both" ;;
			*) MODE="tui" ;;
		esac
	fi
	if [ -z "$MODE" ]; then
		MODE="tui"
	fi
	case "$MODE" in
		tui|gui|both) ;;
		*) say "${RED}FT_NCDU_MODE must be tui, gui, or both.${RESET}"; exit 1 ;;
	esac
}

select_build_directory()
{
	NEEDS_CLONE=0
	if [ -f "$SCRIPT_DIR/Makefile" ] && [ -d "$SCRIPT_DIR/src" ]; then
		BUILD_DIR="$SCRIPT_DIR"
	elif [ -f "$SCRIPT_DIR/ft_ncdu/Makefile" ]; then
		BUILD_DIR="$SCRIPT_DIR/ft_ncdu"
	elif [ -f "Makefile" ] && [ -d "src" ]; then
		BUILD_DIR="$(pwd -P)"
	elif [ -f "ft_ncdu/Makefile" ]; then
		BUILD_DIR="$(pwd -P)/ft_ncdu"
	else
		BUILD_DIR="$TEMP_DIR/ft_ncdu"
		NEEDS_CLONE=1
	fi
}

fetch_source()
{
	if [ "$NEEDS_CLONE" -eq 1 ]; then
		say "${YELLOW}==>${RESET} Downloading ft_ncdu..."
		git clone --depth 1 "$REPO_URL" "$TEMP_DIR"
	fi
}

configure_shell()
{
	case "${SHELL:-}" in
		*/zsh) RC_FILE="$HOME/.zshrc" ;;
		*/bash) RC_FILE="$HOME/.bashrc" ;;
		*) RC_FILE="$HOME/.profile" ;;
	esac
	touch "$RC_FILE"
	if grep -Fq 'clean42 --heal >/dev/null' "$RC_FILE"; then
		RC_TEMP="$(mktemp "${TMPDIR:-/tmp}/ft_ncdu_rc.XXXXXX")"
		grep -Fv -e '# [Auto-heal /goinfre symlinks on login]' \
			-e 'clean42 --heal >/dev/null' "$RC_FILE" > "$RC_TEMP" || true
		cp "$RC_TEMP" "$RC_FILE"
		rm -f "$RC_TEMP"
	fi
	if ! grep -Fq '# ft_ncdu path' "$RC_FILE"; then
		printf '\n# ft_ncdu path\nexport PATH="$HOME/.local/bin:$PATH"\n' \
			>> "$RC_FILE"
	fi
}

trap cleanup EXIT HUP INT TERM

say "${CYAN}${BOLD}ft_ncdu installer${RESET}"
choose_mode
say "${YELLOW}==>${RESET} Selected interface: ${MODE}"
select_build_directory
ensure_dependencies
fetch_source
mkdir -p "$INSTALL_DIR"
if [ "$MODE" != "gui" ]; then
	JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '2')"
	say "${YELLOW}==>${RESET} Building TUI on ${OS_TYPE} with ${JOBS} jobs..."
	make -C "$BUILD_DIR" -j"$JOBS"
	for binary in ft_ncdu ntcl13 clean42; do
		cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/$binary"
		chmod 755 "$INSTALL_DIR/$binary"
	done
fi
if [ "$MODE" != "tui" ]; then
	cp -f "$BUILD_DIR/scripts/ft_ncdu_gui.py" "$INSTALL_DIR/ft_ncdu-gui"
	chmod 755 "$INSTALL_DIR/ft_ncdu-gui"
	if [ "$MODE" = "gui" ]; then
		cp -f "$BUILD_DIR/scripts/ft_ncdu_gui.py" "$INSTALL_DIR/ft_ncdu"
		chmod 755 "$INSTALL_DIR/ft_ncdu"
	fi
fi
configure_shell

say "${GREEN}${BOLD}Installed successfully.${RESET}"
say "Installed in: ${INSTALL_DIR}"
if [ "$MODE" = "both" ]; then
	say "Run TUI: ft_ncdu"
	say "Run GUI: ft_ncdu-gui"
elif [ "$MODE" = "gui" ]; then
	say "Run GUI: ft_ncdu"
else
	say "Run TUI: ft_ncdu"
fi
say "If the command is not visible yet, restart your shell or run:"
say "  export PATH=\"\$HOME/.local/bin:\$PATH\""
