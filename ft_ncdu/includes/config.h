#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define UNUSED_ATTR __attribute__((unused))

/* App Metadata & Performance Tuning */
#define APP_TITLE          "42/1337 CLUSTER STORAGE SUITE"
#define MAX_ENTRIES        8192
#define PATH_MAX_LEN       4096
#define THREAD_COUNT       16
#define MAX_DEPTH          64
#define TICK_DELAY_MS      50

/* Cluster Bootstrap Targets (Redirected to /goinfre via 'b') */
static const char *G_BOOTSTRAP_TARGETS[] UNUSED_ATTR = {
    /* AI & Machine Learning Hubs */
    ".cache/huggingface",
    ".cache/torch",
    ".cache/pip",
    ".ollama",
    ".triton",

    /* Compilers, Engines & Package Managers */
    ".docker",
    ".cargo",
    ".rustup",
    ".npm",
    ".cache",
    ".gradle/caches",

    /* Editors & Caches */
    ".vscode/extensions",
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
    ".gitconfig",
    ".profile",
    ".vimrc",
    ".config/nvim",
    NULL
};

/* Environment Variable Exports Injected into ~/.zshrc (via 'Z') */
static const char *G_ZSHRC_EXPORTS[] UNUSED_ATTR = {
    "export HF_HOME=\"/goinfre/$USER/huggingface\"",
    "export TORCH_HOME=\"/goinfre/$USER/torch\"",
    "export CARGO_HOME=\"/goinfre/$USER/cargo\"",
    "export RUSTUP_HOME=\"/goinfre/$USER/rustup\"",
    "export DOCKER_CONFIG=\"/goinfre/$USER/docker\"",
    "export NPM_CONFIG_CACHE=\"/goinfre/$USER/npm\"",
    "export PIP_CACHE_DIR=\"/goinfre/$USER/pip\"",
    NULL
};

/* Modular Cleaning Presets (Triggered via 'C') */
typedef struct {
    char        key;
    const char  *title;
    const char  *desc;
    const char  *command_fmt;
} CleanPreset;

static const CleanPreset G_CLEAN_PRESETS[] UNUSED_ATTR = {
    {
        '1',
        "C / C++ Dev Output",
        "Wipe *.o, *.a, *.dSYM, vgcore.*, core dumps & CMake artifacts",
        "find '%s' \\( -name \"*.o\" -o -name \"*.a\" -o -name \"*.dSYM\" "
        "-o -name \"vgcore.*\" -o -name \"core.*\" -o -name \"CMakeCache.txt\" "
        "-o -name \"CMakeFiles\" \\) -prune -exec rm -rf {} + 2>/dev/null"
    },
    {
        '2',
        "AI & Python ML Caches",
        "Clear Hugging Face hub, PyTorch weights, Triton & __pycache__",
        "rm -rf ~/.cache/huggingface/hub ~/.cache/torch ~/.triton 2>/dev/null; "
        "find '%s' \\( -name \"__pycache__\" -o -name \".pytest_cache\" "
        "-o -name \".mypy_cache\" -o -name \"*.pyc\" \\) -prune -exec rm -rf {} + 2>/dev/null"
    },
    {
        '3',
        "Web / Node.js Bloat",
        "Purge node_modules trees, build output & NPM package cache",
        "find '%s' -type d -name \"node_modules\" -prune -exec rm -rf {} + 2>/dev/null; "
        "rm -rf ~/.npm/_cacache ~/.yarn/cache 2>/dev/null"
    },
    {
        '4',
        "Browser & Electron Apps",
        "Wipe Chrome, Slack, Discord & Spotify ServiceWorker caches",
        "rm -rf ~/.config/*/Service\\ Worker/CacheStorage "
        "~/.config/*/GPUCache ~/.config/*/Code\\ Cache "
        "~/Library/Application\\ Support/*/Service\\ Worker/CacheStorage 2>/dev/null"
    },
    {
        '5',
        "Nuclear Cluster Wipe",
        "Wipe build junk, AI caches, trash & trigger Docker system prune",
        "find '%s' \\( -name \"*.o\" -o -name \"*.a\" -o -name \"node_modules\" "
        "-o -name \".dSYM\" -o -name \"__pycache__\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
        "docker system prune -a --volumes -f >/dev/null 2>&1"
    }
};

#define PRESET_COUNT (sizeof(G_CLEAN_PRESETS) / sizeof(G_CLEAN_PRESETS[0]))

#endif
