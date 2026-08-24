/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   presets_data.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

const t_clean_preset	g_clean_presets[PRESET_COUNT] = {
{
	'1',
	"42 C / C++ Dev Output",
	"Wipe *.o, *.a, *.dSYM, vgcore.*, clangd",
	"sh -c 'cd \"%s\" && find . -type f \\( -name \"*.o\" -o -name \"*.a\" "
	"-o -name \"*.out\" -o -name \"core.*\" -o -name \"vgcore.*\" \\) "
	"-delete 2>/dev/null; rm -rf .cache/clangd CMakeCache.txt 2>/dev/null'"
},
{
	'2',
	"Francinette & Testers",
	"Purge Francinette temp logs & coverage files",
	"sh -c 'cd \"%s\" && rm -rf francinette/temp .francinette/logs "
	"tests/valgrind*.log 2>/dev/null; find . -type f \\( -name \"*.gcda\" "
	"-o -name \"*.gcno\" \\) -delete 2>/dev/null'"
},
{
	'3',
	"AI & Python ML Caches",
	"Purge HuggingFace, PyTorch, Triton & pycache",
	"sh -c 'cd \"%s\" && find . -type d \\( -name \"__pycache__\" -o "
	"-name \".pytest_cache\" -o -name \".mypy_cache\" \\) -prune -exec "
	"rm -rf {} + 2>/dev/null; rm -rf .cache/huggingface .cache/torch "
	".cache/triton .cache/ollama .cache/pip 2>/dev/null'"
},
{
	'4',
	"Web & Transcendence",
	"Purge node_modules, .next, npm & yarn caches",
	"sh -c 'cd \"%s\" && find . -name \"node_modules\" -type d -prune "
	"-exec rm -rf {} + 2>/dev/null; rm -rf .npm/_cacache .yarn/cache "
	".pnpm-store .next .turbo 2>/dev/null'"
},
{
	'5',
	"Browser & Electron Apps",
	"Wipe Chrome, Brave, VS Code, Discord & Slack",
	"sh -c 'cd \"%s\" && rm -rf .cache/google-chrome .cache/chromium "
	".cache/brave .cache/mozilla/firefox .var/app/*/cache "
	".config/Code/Cache* .config/Code/CachedData .config/discord/Cache* "
	"2>/dev/null'"
},
{
	'6',
	"Docker Cluster Prune",
	"Run docker system prune -a --volumes -f",
	"sh -c 'docker system prune -a --volumes -f 2>/dev/null; "
	"cd \"%s\" && rm -rf .docker/*/cache 2>/dev/null'"
},
{
	'7',
	"Norminette & Shell Logs",
	"Wipe norminette cache, .zcompdump & X11 logs",
	"sh -c 'cd \"%s\" && rm -rf .cache/norminette .zcompdump* "
	".xsession-errors* .cache/valgrind 2>/dev/null'"
},
{
	'8',
	"Desktop Trash Bin",
	"Purge ~/.local/share/Trash and ~/.Trash",
	"sh -c 'cd \"%s\" && rm -rf .local/share/Trash/* .Trash/* 2>/dev/null'"
},
{
	'9',
	"Nuclear 1337 Wipe",
	"One-shot deep clean across all cache tiers",
	"sh -c 'cd \"%s\" && rm -rf .cache/* .local/share/Trash/* .42* "
	".zcompdump* .var/app/*/cache francinette/temp 2>/dev/null'"
}
};

const char				*g_bootstrap_targets[] = {
	".cache/huggingface",
	".cache/torch",
	".cache/ollama",
	".cargo",
	".rustup",
	".npm",
	".local/share/pnpm",
	".docker",
	".var/app",
	NULL
};

const char				*g_protected_paths[] = {
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

const char				*g_shell_exports[] = {
	"# === 1337 / 42 Goinfre Quota Bypass (Managed by ft_ncdu) ===",
	"export GOINFRE_USER=\"/goinfre/$USER\"",
	"[ ! -d \"$GOINFRE_USER\" ] && [ -d \"/sgoinfre/$USER\" ] "
	"&& export GOINFRE_USER=\"/sgoinfre/$USER\"",
	"[ ! -d \"$GOINFRE_USER\" ] && export GOINFRE_USER=\"/tmp/goinfre_$USER\"",
	"mkdir -p \"$GOINFRE_USER\"",
	"export HF_HOME=\"$GOINFRE_USER/huggingface\"",
	"export TORCH_HOME=\"$GOINFRE_USER/torch\"",
	"export CARGO_HOME=\"$GOINFRE_USER/cargo\"",
	"export RUSTUP_HOME=\"$GOINFRE_USER/rustup\"",
	"export DOCKER_CONFIG=\"$GOINFRE_USER/docker\"",
	"export NPM_CONFIG_CACHE=\"$GOINFRE_USER/npm-cache\"",
	"export PIP_CACHE_DIR=\"$GOINFRE_USER/pip-cache\"",
	"alias clean42=\"ntcl13\"",
	"alias ntcl13=\"$HOME/.local/bin/ntcl13\"",
	"alias goinfre=\"cd $GOINFRE_USER\"",
	"# === End ft_ncdu block ===",
	NULL
};
