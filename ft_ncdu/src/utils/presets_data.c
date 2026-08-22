/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   presets_data.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

const t_clean_preset	g_clean_presets[PRESET_COUNT] = {
{
	'1',
	"42 C / C++ Dev Output",
	"Wipe *.o, *.a, *.dSYM, vgcore.*, clangd",
	"find %s -type f \\( -name \"*.o\" -o -name \"*.a\" -o -name \"*.out\" "
	"-o -name \"core.*\" -o -name \"vgcore.*\" \\) -delete 2>/dev/null; "
	"rm -rf %s/.cache/clangd 2>/dev/null"
},
{
	'2',
	"AI & Python ML Caches",
	"Purge HuggingFace, PyTorch, Triton & pycache",
	"find %s -type d \\( -name \"__pycache__\" -o -name \".pytest_cache\" "
	"-o -name \".mypy_cache\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
	"rm -rf %s/.cache/huggingface %s/.cache/torch %s/.cache/triton 2>/dev/null"
},
{
	'3',
	"Web & Transcendence",
	"Purge node_modules, .next, npm & yarn caches",
	"find %s -name \"node_modules\" -type d -prune -exec rm -rf {} + "
	"2>/dev/null; rm -rf %s/.npm/_cacache %s/.yarn/cache %s/.pnpm-store "
	"2>/dev/null"
},
{
	'4',
	"Browser & Electron Caches",
	"Wipe Chrome, Brave, Slack, Discord & Spotify",
	"rm -rf %s/.cache/google-chrome %s/.cache/chromium %s/.cache/brave "
	"%s/.var/app/com.discordapp.Discord/cache "
	"%s/.var/app/com.slack.Slack/cache 2>/dev/null"
},
{
	'5',
	"Docker Cluster Prune",
	"Run docker system prune -a --volumes -f",
	"docker system prune -a --volumes -f 2>/dev/null; "
	"rm -rf %s/.docker/*/cache 2>/dev/null"
},
{
	'6',
	"Norminette & Shell Logs",
	"Wipe norminette cache, .zcompdump & X11 logs",
	"rm -rf %s/.cache/norminette %s/.zcompdump* %s/.xsession-errors* "
	"2>/dev/null"
},
{
	'7',
	"Desktop Trash Bin",
	"Purge ~/.local/share/Trash and ~/.Trash",
	"rm -rf %s/.local/share/Trash/* %s/.Trash/* 2>/dev/null"
},
{
	'8',
	"Nuclear Cluster Wipe",
	"One-shot deep clean across all cache tiers",
	"rm -rf %s/.cache/* %s/.local/share/Trash/* %s/.42* %s/.zcompdump* "
	"2>/dev/null"
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
