#!/usr/bin/env bash
# **************************************************************************** #
#                                                                              #
#   install.sh - 1-Line Universal Cross-Platform Installer for ft_ncdu         #
#   Compatible with macOS (Darwin) & Linux (Ubuntu, Debian, Arch, Fedora)      #
#   Author: tlaghzal <tlaghzal@student.1337.ma>                                #
#                                                                              #
# **************************************************************************** #

set -e

# Color definitions
CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BOLD='\033[1m'
NC='\033[0m'

echo -e "${CYAN}${BOLD}"
echo "   1337 | 42 CLUSTER STORAGE SUITE INSTALLER"
echo "  ============================================"
echo -e "${NC}"

OS_TYPE="$(uname -s)"
INSTALL_DIR="$HOME/.local/bin"
REPO_URL="https://github.com/laghzal49/ft_ncdu.git"
TEMP_DIR="/tmp/ft_ncdu_install_$$"

mkdir -p "$INSTALL_DIR"

# Check dependency helper
if ! command -v make >/dev/null 2>&1; then
    echo -e "${RED}Error:${NC} 'make' is required to compile ft_ncdu."
    if [ "$OS_TYPE" = "Darwin" ]; then
        echo -e "Install Xcode Command Line Tools: ${BOLD}xcode-select --install${NC}"
    else
        echo -e "Install build-essential or make using your package manager."
    fi
    exit 1
fi

# Determine build directory
if [ -d "ft_ncdu" ] && [ -f "ft_ncdu/Makefile" ]; then
    echo -e "${YELLOW}==>${NC} Building from local workspace directory..."
    BUILD_DIR="ft_ncdu"
elif [ -f "Makefile" ] && [ -d "src" ]; then
    echo -e "${YELLOW}==>${NC} Building from current directory..."
    BUILD_DIR="."
else
    echo -e "${YELLOW}==>${NC} Fetching latest release from GitHub (${REPO_URL})..."
    git clone --depth 1 "$REPO_URL" "$TEMP_DIR" >/dev/null 2>&1
    BUILD_DIR="$TEMP_DIR/ft_ncdu"
fi

# Determine core count
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo -e "${YELLOW}==>${NC} Compiling on ${OS_TYPE} using ${NPROC} threads..."
make -C "$BUILD_DIR" -j"$NPROC" >/dev/null

# Install binaries
echo -e "${YELLOW}==>${NC} Installing binaries to ${INSTALL_DIR}..."
cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/ft_ncdu"
cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/ntcl13"
cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/clean42"
chmod +x "$INSTALL_DIR/ft_ncdu" "$INSTALL_DIR/ntcl13" "$INSTALL_DIR/clean42"

# Cleanup temporary clone
if [ -d "$TEMP_DIR" ]; then
    rm -rf "$TEMP_DIR"
fi

# Configure Shell Environment (~/.zshrc, ~/.bashrc, ~/.zprofile, ~/.bash_profile)
configure_shell() {
    local rc_file="$1"
    if [ -f "$rc_file" ] || [ "$2" = "create_if_zsh" -a "$SHELL" = *"zsh"* ]; then
        touch "$rc_file"
        # Ensure ~/.local/bin is in PATH
        if ! grep -q 'export PATH=.*\.local/bin' "$rc_file" 2>/dev/null; then
            echo '' >> "$rc_file"
            echo '# [1337 / 42 Cluster Suite Path]' >> "$rc_file"
            echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$rc_file"
        fi

        # Add aliases if missing
        if ! grep -q 'alias clean42=' "$rc_file" 2>/dev/null; then
            echo '' >> "$rc_file"
            echo '# [1337 / 42 Cluster Suite Aliases]' >> "$rc_file"
            echo 'alias clean="clean42"' >> "$rc_file"
            echo 'alias space="ft_ncdu"' >> "$rc_file"
            echo 'alias heal="clean42 --heal"' >> "$rc_file"
        fi

        # Add login station-healer hook
        if ! grep -q 'clean42 --heal >/dev/null' "$rc_file" 2>/dev/null; then
            echo '' >> "$rc_file"
            echo '# [Auto-heal /goinfre symlinks on login]' >> "$rc_file"
            echo 'if [ -x "$HOME/.local/bin/clean42" ]; then clean42 --heal >/dev/null 2>&1; fi' >> "$rc_file"
        fi
    fi
}

configure_shell "$HOME/.zshrc" "create_if_zsh"
configure_shell "$HOME/.bashrc"
configure_shell "$HOME/.zprofile"
configure_shell "$HOME/.bash_profile"

echo -e "${GREEN}${BOLD}"
echo "  ✔ INSTALLATION SUCCESSFUL ON ${OS_TYPE}!"
echo "  =========================================="
echo -e "${NC}"
echo -e "  Commands installed to ${CYAN}${INSTALL_DIR}${NC}:"
echo -e "    ${BOLD}ft_ncdu${NC}   : Interactive macOS-themed Disk & Quota Manager"
echo -e "    ${BOLD}ntcl13${NC}    : Native C 9-Tier Cluster Cleaner (<0.2s)"
echo -e "    ${BOLD}clean42${NC}   : Native C 9-Tier Cluster Cleaner (<0.2s)"
echo ""
echo -e "  ${YELLOW}Quick Usage:${NC}"
echo -e "    • Run ${BOLD}ft_ncdu${NC} to explore and manage your quota."
echo -e "    • Run ${BOLD}clean42${NC} (or ${BOLD}clean${NC}) to instantly free up disk space."
echo -e "    • Inside ${BOLD}ft_ncdu${NC}, press ${BOLD}'f'${NC} to view all features and shortcuts."
echo ""
echo -e "  ${CYAN}To apply changes immediately, run:${NC} ${BOLD}source ~/.zshrc${NC} (or restart terminal)"
echo ""
