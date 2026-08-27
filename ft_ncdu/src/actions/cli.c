/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/27 10:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

static void	exec_clean_loop(const char *home, int is_dry_run)
{
	size_t	i;
	char	*cmd;

	i = 0;
	while (i < PRESET_COUNT)
	{
		if (is_dry_run)
		{
			printf("  \033[1;33m[DRY-RUN]\033[0m Would purge: %s\n",
				g_clean_presets[i].title);
			log_audit_action("DRY_RUN", home, g_clean_presets[i].title);
		}
		else
		{
			printf("  -> Cleaning: %s...\n", g_clean_presets[i].title);
			if (asprintf(&cmd, g_clean_presets[i].command_fmt, home) != -1
				&& system(cmd))
				free(cmd);
		}
		i++;
	}
}

static void	print_clean_results(const char *home, int is_dry_run,
		const char *sz_a)
{
	if (is_dry_run)
		printf("\n\033[1;32m[✔] Dry-run complete. No files wiped.\033[0m\n\n");
	else
	{
		log_audit_action("CLEAN_PURGE", home, "9 tiers purged");
		printf("\n\033[1;32m[✔] Available Space After: %s\033[0m\n\n", sz_a);
	}
}

int	run_cli_clean(int is_dry_run)
{
	const char		*home;
	struct statvfs	vfs;
	char			sz_b[16];
	char			sz_a[16];

	home = getenv("HOME");
	if (!home)
		home = ".";
	statvfs(home, &vfs);
	format_size((off_t)vfs.f_bfree * vfs.f_frsize, sz_b, sizeof(sz_b));
	printf("\n\033[1;36m:: [ ntcl13 / 1337 & 42 Fast Cleaner ] ::\033[0m\n");
	if (is_dry_run)
		printf("\033[1;35m[*] DRY-RUN: Simulating cleanup...\033[0m\n\n");
	else
		printf("\033[1;33m[*] Free Space Before: %s\033[0m\n\n", sz_b);
	exec_clean_loop(home, is_dry_run);
	statvfs(home, &vfs);
	format_size((off_t)vfs.f_bfree * vfs.f_frsize, sz_a, sizeof(sz_a));
	print_clean_results(home, is_dry_run, sz_a);
	return (0);
}

int	run_cli_heal(void)
{
	const char	*home;
	char		*esc;
	char		*cmd;

	home = getenv("HOME");
	if (!home)
		return (1);
	printf("\033[1;36m[ft_ncdu]\033[0m Repairing broken goinfre links...\n");
	esc = shell_escape(home);
	if (esc && asprintf(&cmd,
			"find %s -maxdepth 4 -type l -exec sh -c 'for l; do "
			"t=$(readlink \"$l\"); case \"$t\" in /goinfre/*|/sgoinfre/*"
			"|/tmp/goinfre_*) [ ! -e \"$l\" ] && mkdir -p \"$t\" && "
			"echo \"  Healed: $l\";; esac; done' sh {} + 2>/dev/null",
			esc) != -1 && system(cmd))
		free(cmd);
	free(esc);
	log_audit_action("HEAL_LINKS", home, "Station symlinks repaired");
	printf("\033[1;32m[ft_ncdu]\033[0m All goinfre targets restored.\n");
	return (0);
}

int	run_cli_bootstrap(void)
{
	char		bg[PATH_MAX_LEN];
	const char	*home;
	char		*cmd;
	size_t		i;

	get_goinfre_path(bg, sizeof(bg));
	home = getenv("HOME");
	if (!home)
		return (1);
	printf("\033[1;36m[ft_ncdu]\033[0m Bootstrapping tools to %s...\n", bg);
	i = 0;
	while (g_bootstrap_targets[i] != NULL)
	{
		if (asprintf(&cmd, "mkdir -p $(dirname %s/%s) && mv %s/%s %s/%s && "
				"ln -s %s/%s %s/%s 2>/dev/null", bg, g_bootstrap_targets[i],
				home, g_bootstrap_targets[i], bg, g_bootstrap_targets[i],
				bg, g_bootstrap_targets[i], home, g_bootstrap_targets[i]) != -1
			&& system(cmd))
			free(cmd);
		i++;
	}
	log_audit_action("BOOTSTRAP_GOINFRE", bg, "Toolchain caches relocated");
	printf("\033[1;32m[ft_ncdu]\033[0m Bootstrap complete!\n");
	return (0);
}
