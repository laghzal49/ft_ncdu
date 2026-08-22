#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define UNUSED_ATTR __attribute__((unused))

/* App Metadata & Performance Tuning */
#define APP_NAME           "ft_ncdu"
#define APP_VERSION        "2.0.0"
#define APP_TITLE          "42 / 1337 CLUSTER STORAGE SUITE"
#define MAX_ENTRIES        65536
#define PATH_MAX_LEN       4096
#define THREAD_COUNT       16
#define MAX_DEPTH          64
#define TICK_DELAY_MS      40

/* Cluster Bootstrap Targets (Redirected to /goinfre via 'b') */
static const char *G_BOOTSTRAP_TARGETS[] UNUSED_ATTR = {
    /* AI & Machine Learning Hubs */
    ".cache/huggingface",
    ".cache/torch",
    ".cache/pip",
    ".ollama",
    ".triton",

    /* Compilers, Toolchains & Package Managers */
    ".docker",
    ".cargo",
    ".rustup",
    ".npm",
    ".yarn",
    ".pnpm-store",
    ".gradle",
    "go/pkg",
    ".cache/clangd",
    ".cache/ccache",

    /* Editors & Heavy IDE Caches */
    ".vscode/extensions",
    ".vscode-server",
    ".config/Code/CachedData",
    ".config/Code/User/workspaceStorage",

    /* Browsers & Electron App Caches */
    ".config/google-chrome",
    ".config/chromium",
    ".config/BraveSoftware",
    ".config/Slack/Service Worker/CacheStorage",
    ".config/discord/Cache",
    ".config/spotify/Storage",
    "Library/Caches",
    ".local/share/Steam",
    NULL
};

/* Protected Target Whitelist (Prevents Accidental Deletion) */
static const char *G_PROTECTED_TARGETS[] UNUSED_ATTR = {
    ".ssh",
    ".zshrc",
    ".bashrc",
    ".bash_profile",
    ".zprofile",
    ".zshenv",
    ".gitconfig",
    ".profile",
    ".vimrc",
    ".inputrc",
    ".gnupg",
    ".config/nvim",
    NULL
};

/* Environment Variable Exports Injected into ~/.zshrc and ~/.bashrc (via 'Z') */
static const char *G_SHELL_EXPORTS[] UNUSED_ATTR = {
    "# >>> 1337 / 42 Goinfre Quota Bypass >>>",
    "export HF_HOME=\"/goinfre/$USER/huggingface\"",
    "export TORCH_HOME=\"/goinfre/$USER/torch\"",
    "export CARGO_HOME=\"/goinfre/$USER/cargo\"",
    "export RUSTUP_HOME=\"/goinfre/$USER/rustup\"",
    "export DOCKER_CONFIG=\"/goinfre/$USER/docker\"",
    "export NPM_CONFIG_CACHE=\"/goinfre/$USER/npm\"",
    "export PIP_CACHE_DIR=\"/goinfre/$USER/pip\"",
    "export GOPATH=\"/goinfre/$USER/go\"",
    "export CCACHE_DIR=\"/goinfre/$USER/ccache\"",
    "alias clean42=\"ft_ncdu --clean\"",
    "alias goinfre=\"cd /goinfre/$USER 2>/dev/null || cd /tmp/goinfre_$USER\"",
    "alias sgoinfre=\"cd /sgoinfre/$USER 2>/dev/null\"",
    "# <<< 1337 / 42 Goinfre Quota Bypass <<<",
    NULL
};

/* Modular Cleaning Presets (Triggered via 'C' or '--clean') */
typedef struct {
    char        key;
    const char  *title;
    const char  *desc;
    const char  *command_fmt;
} CleanPreset;

static const CleanPreset G_CLEAN_PRESETS[] UNUSED_ATTR = {
    {
        '1',
        "42 C / C++ Dev Output",
        "Wipe *.o, *.a, *.dSYM, vgcore.*, core dumps, clangd & CMake cache",
        "find '%s' \\( -name \"*.o\" -o -name \"*.a\" -o -name \"*.dSYM\" "
        "-o -name \"*.out\" -o -name \"vgcore.*\" -o -name \"core.*\" "
        "-o -name \"CMakeCache.txt\" -o -name \"CMakeFiles\" "
        "-o -name \".browse.c_cpp.*\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
        "rm -rf ~/.cache/clangd 2>/dev/null"
    },
    {
        '2',
        "AI & Python ML Caches",
        "Clear Hugging Face, PyTorch, Triton, Ollama temp & __pycache__",
        "rm -rf ~/.cache/huggingface/hub ~/.cache/torch ~/.triton ~/.cache/pip 2>/dev/null; "
        "find '%s' \\( -name \"__pycache__\" -o -name \".pytest_cache\" "
        "-o -name \".mypy_cache\" -o -name \"*.pyc\" -o -name \"*.pyo\" "
        "-o -name \".ipynb_checkpoints\" \\) -prune -exec rm -rf {} + 2>/dev/null"
    },
    {
        '3',
        "Web & 42 Transcendence",
        "Purge node_modules, .next, .turbo, NPM/Yarn/pnpm cache",
        "find '%s' \\( -type d -name \"node_modules\" -o -name \".next\" "
        "-o -name \".turbo\" -o -name \".nuxt\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
        "rm -rf ~/.npm/_cacache ~/.yarn/cache ~/.pnpm-store 2>/dev/null"
    },
    {
        '4',
        "Browser & Electron Bloat",
        "Wipe Chrome, Slack, Discord, Spotify & VS Code service caches",
        "rm -rf ~/.config/*/Service\\ Worker/CacheStorage "
        "~/.config/*/GPUCache ~/.config/*/Code\\ Cache "
        "~/.config/google-chrome/*/Cache ~/.config/chromium/*/Cache "
        "~/.config/BraveSoftware/*/Cache ~/.config/Slack/Cache "
        "~/.config/discord/Cache ~/.config/spotify/Storage "
        "~/Library/Caches/Google/Chrome ~/Library/Application\\ Support/*/Service\\ Worker/CacheStorage 2>/dev/null"
    },
    {
        '5',
        "Docker Cluster Storage",
        "Execute full Docker system prune (unused images, containers, volumes)",
        "docker system prune -a --volumes -f >/dev/null 2>&1; "
        "rm -rf ~/.docker/desktop ~/.docker/run 2>/dev/null"
    },
    {
        '6',
        "Norminette & Cluster Logs",
        "Wipe Norminette cache, X11 logs, zcompdump & Valgrind logs",
        "rm -rf ~/.cache/norminette ~/.zcompdump* ~/.xsession-errors* "
        "~/.local/share/xorg/* ~/.valgrind* /tmp/vgcore* 2>/dev/null"
    },
    {
        '7',
        "Empty Trash Bin",
        "Permanently wipe desktop trash (~/.local/share/Trash and ~/.Trash)",
        "rm -rf ~/.local/share/Trash/* ~/.local/share/Trash/.* ~/.Trash/* 2>/dev/null"
    },
    {
        '8',
        "Nuclear Cluster Wipe",
        "One-shot safe wipe of build junk, web caches, AI caches, trash & logs",
        "find '%s' \\( -name \"*.o\" -o -name \"*.a\" -o -name \"node_modules\" "
        "-o -name \".dSYM\" -o -name \"__pycache__\" -o -name \"*.pyc\" "
        "-o -name \"vgcore.*\" -o -name \"core.*\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
        "rm -rf ~/.cache/norminette ~/.cache/huggingface/hub ~/.cache/torch ~/.triton "
        "~/.npm/_cacache ~/.yarn/cache ~/.local/share/Trash/* ~/.local/share/Trash/.* 2>/dev/null; "
        "docker system prune -a --volumes -f >/dev/null 2>&1"
    }
};

#define PRESET_COUNT (sizeof(G_CLEAN_PRESETS) / sizeof(G_CLEAN_PRESETS[0]))

#endif
