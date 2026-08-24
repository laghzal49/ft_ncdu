/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

static void	exec_clean_loop(const char *home)
{
	size_t	i;
	char	*cmd;

	i = 0;
	while (i < PRESET_COUNT)
	{
		printf("  -> Cleaning: %s...\n", g_clean_presets[i].title);
		if (asprintf(&cmd, g_clean_presets[i].command_fmt, home) != -1)
		{
			if (system(cmd))
				(void)0;
			free(cmd);
		}
		i++;
	}
}

int	run_cli_clean(void)
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
	printf("\033[1;33m[*] Free Space Before: %s\033[0m\n\n", sz_b);
	exec_clean_loop(home);
	statvfs(home, &vfs);
	format_size((off_t)vfs.f_bfree * vfs.f_frsize, sz_a, sizeof(sz_a));
	printf("\n\033[1;32m[✔] Available Space After: %s\033[0m\n\n", sz_a);
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
			"find %s -maxdepth 4 -type l -exec sh -c '"
			"for l; do t=$(readlink \"$l\"); "
			"case \"$t\" in /goinfre/*|/sgoinfre/*|/tmp/goinfre_*) "
			"if [ ! -e \"$l\" ]; then mkdir -p \"$t\"; "
			"echo \"  Healed: $l\"; fi;; esac; done' sh {} + 2>/dev/null",
			esc) != -1)
	{
		if (system(cmd))
			(void)0;
		free(cmd);
	}
	free(esc);
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
	printf("\033[1;32m[ft_ncdu]\033[0m Bootstrap complete!\n");
	return (0);
}

int	run_cli_report(const char *target_path)
{
	struct statvfs	vfs;
	char			sz_tot[16];
	char			sz_used[16];
	char			sz_free[16];

	statvfs(target_path, &vfs);
	format_size((off_t)vfs.f_blocks * vfs.f_frsize, sz_tot, sizeof(sz_tot));
	format_size((off_t)(vfs.f_blocks - vfs.f_bfree) * vfs.f_frsize,
		sz_used, sizeof(sz_used));
	format_size((off_t)vfs.f_bfree * vfs.f_frsize, sz_free, sizeof(sz_free));
	printf("\n======================================================\n");
	printf("  \033[1;36m42 / 1337 CLUSTER STORAGE AUDIT REPORT\033[0m\n");
	printf("======================================================\n");
	printf("  Path  : %s\n", target_path);
	printf("  Quota : %s\n", sz_tot);
	printf("  Used  : %s\n", sz_used);
	printf("  Free  : %s\n", sz_free);
	printf("======================================================\n\n");
	return (0);
}
