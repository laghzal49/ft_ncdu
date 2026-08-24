#!/usr/bin/env bash
# **************************************************************************** #
#                                                                              #
#   install.sh - 1-Line Universal Installer for ft_ncdu & clean42 / ntcl13     #
#   Built for 1337 Coding School & 42 Network Cadets                           #
#                                                                              #
# **************************************************************************** #

set -e

# Color definitions
CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo -e "${CYAN}${BOLD}"
echo "   1337 | 42 CLUSTER STORAGE SUITE INSTALLER"
echo "  ============================================"
echo -e "${NC}"

# Target directories
INSTALL_DIR="$HOME/.local/bin"
REPO_URL="https://github.com/laghzal49/ft_ncdu.git"
TEMP_DIR="/tmp/ft_ncdu_install_$$"

mkdir -p "$INSTALL_DIR"

# Determine build source
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

# Compile binaries with maximum optimization
echo -e "${YELLOW}==>${NC} Compiling with -O3 -march=native..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -C "$BUILD_DIR" -j"$NPROC" >/dev/null

# Install binaries
echo -e "${YELLOW}==>${NC} Installing binaries to ${INSTALL_DIR}..."
cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/ft_ncdu"
cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/ntcl13"
cp -f "$BUILD_DIR/ft_ncdu" "$INSTALL_DIR/clean42"
chmod +x "$INSTALL_DIR/ft_ncdu" "$INSTALL_DIR/ntcl13" "$INSTALL_DIR/clean42"

# Cleanup temporary clone if created
if [ -d "$TEMP_DIR" ]; then
    rm -rf "$TEMP_DIR"
fi

# Configure Shell Environment (~/.zshrc and ~/.bashrc)
configure_shell() {
    local rc_file="$1"
    if [ -f "$rc_file" ]; then
        # Ensure ~/.local/bin is in PATH
        if ! grep -q 'export PATH=.*\.local/bin' "$rc_file" 2>/dev/null; then
            echo '' >> "$rc_file"
            echo '# [1337 / 42 Cluster Suite Path]' >> "$rc_file"
            echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$rc_file"
        fi

        # Add helpful campus aliases if missing
        if ! grep -q 'alias clean42=' "$rc_file" 2>/dev/null; then
            echo '' >> "$rc_file"
            echo '# [1337 / 42 Cluster Suite Aliases]' >> "$rc_file"
            echo 'alias clean="clean42"' >> "$rc_file"
            echo 'alias space="ft_ncdu"' >> "$rc_file"
            echo 'alias heal="clean42 --heal"' >> "$rc_file"
        fi

        # Add login station-switch healer hook
        if ! grep -q 'clean42 --heal >/dev/null' "$rc_file" 2>/dev/null; then
            echo '' >> "$rc_file"
            echo '# [Auto-heal /goinfre symlinks on login]' >> "$rc_file"
            echo 'if [ -x "$HOME/.local/bin/clean42" ]; then clean42 --heal >/dev/null 2>&1; fi' >> "$rc_file"
        fi
    fi
}

configure_shell "$HOME/.zshrc"
configure_shell "$HOME/.bashrc"

echo -e "${GREEN}${BOLD}"
echo "  ✔ INSTALLATION SUCCESSFUL!"
echo "  =========================="
echo -e "${NC}"
echo -e "  Commands installed to ${CYAN}${INSTALL_DIR}${NC}:"
echo -e "    ${BOLD}ft_ncdu${NC}   : Interactive macOS-themed Disk & Quota Manager"
echo -e "    ${BOLD}ntcl13${NC}    : Native C 9-Tier Cluster Cleaner (<0.2s)"
echo -e "    ${BOLD}clean42${NC}   : Native C 9-Tier Cluster Cleaner (<0.2s)"
echo ""
echo -e "  ${YELLOW}Quick Usage:${NC}"
echo -e "    • Run ${BOLD}ft_ncdu${NC} to explore and manage your cluster quota."
echo -e "    • Run ${BOLD}clean42${NC} (or ${BOLD}clean${NC}) to instantly free up disk space."
echo -e "    • Inside ${BOLD}ft_ncdu${NC}, press ${BOLD}'f'${NC} to view all features and shortcuts."
echo ""
echo -e "  ${CYAN}Restart your terminal or run:${NC} ${BOLD}source ~/.zshrc${NC}"
echo ""
