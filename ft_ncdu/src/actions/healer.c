/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   healer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

void	action_heal_symlinks(void)
{
	const char	*home;
	char		*esc_home;
	char		*cmd;

	home = getenv("HOME");
	if (!home || !confirm_modal("HEAL LINKS", "Repair dangling goinfre links?"))
		return ;
	esc_home = shell_escape(home);
	if (esc_home)
	{
		cmd = NULL;
		if (asprintf(&cmd, "find %s -maxdepth 4 -type l -exec sh -c '"
				"for link; do target=$(readlink \"$link\"); "
				"case \"$target\" in /goinfre/*|/sgoinfre/*|/tmp/goinfre_*) "
				"if [ ! -e \"$link\" ]; then mkdir -p \"$target\"; fi;; "
				"esac; done' sh {} + 2>/dev/null", esc_home) != -1)
		{
			if (system(cmd))
				(void)0;
			free(cmd);
		}
		free(esc_home);
	}
	start_async_scan(g_state.current_dir);
}

void	action_empty_trash(void)
{
	if (!confirm_modal("EMPTY TRASH", "Purge desktop Trash bins?"))
		return ;
	if (system("rm -rf ~/.local/share/Trash/* ~/.Trash/* 2>/dev/null"))
		(void)0;
	start_async_scan(g_state.current_dir);
}

static int	is_zshrc_injected(const char *path)
{
	FILE	*fp;
	char	line[512];
	int		found;

	fp = fopen(path, "r");
	if (!fp)
		return (0);
	found = 0;
	while (fgets(line, sizeof(line), fp))
	{
		if (strstr(line, "Goinfre Quota Bypass") != NULL)
		{
			found = 1;
			break ;
		}
	}
	fclose(fp);
	return (found);
}

void	action_inject_zshrc(void)
{
	const char	*home;
	char		path[PATH_MAX_LEN];
	FILE		*fp;
	size_t		i;

	home = getenv("HOME");
	if (!home)
		return ;
	snprintf(path, sizeof(path), "%.2048s/.zshrc", home);
	if (is_zshrc_injected(path))
	{
		confirm_modal("INFO", "Goinfre exports already in ~/.zshrc!");
		return ;
	}
	if (!confirm_modal("INJECT ~/.zshrc", "Append goinfre exports?"))
		return ;
	fp = fopen(path, "a");
	if (!fp)
		return ;
	fprintf(fp, "\n");
	i = 0;
	while (g_shell_exports[i] != NULL)
		fprintf(fp, "%s\n", g_shell_exports[i++]);
	fclose(fp);
	confirm_modal("SUCCESS", "Injected! Run: source ~/.zshrc");
}

void	action_bootstrap_goinfre(void)
{
	char		bg[PATH_MAX_LEN];
	const char	*home;
	char		*cmd;
	size_t		i;

	if (!confirm_modal("BOOTSTRAP", "Link AI models & tools to goinfre?"))
		return ;
	get_goinfre_path(bg, sizeof(bg));
	home = getenv("HOME");
	if (!home)
		return ;
	i = 0;
	while (g_bootstrap_targets[i] != NULL)
	{
		if (asprintf(&cmd,
				"mkdir -p $(dirname %s/%s) && mv %s/%s %s/%s && "
				"ln -s %s/%s %s/%s",
				bg, g_bootstrap_targets[i], home, g_bootstrap_targets[i],
				bg, g_bootstrap_targets[i], bg, g_bootstrap_targets[i],
				home, g_bootstrap_targets[i]) != -1 && system(cmd))
			free(cmd);
		i++;
	}
	start_async_scan(g_state.current_dir);
}
