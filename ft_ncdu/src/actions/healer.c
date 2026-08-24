/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   healer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

void	action_heal_symlinks(void)
{
	const char	*home_dir;
	char		*esc_home;
	char		*command_str;

	home_dir = getenv("HOME");
	if (!home_dir
		|| !confirm_modal("HEAL LINKS", "Repair dangling goinfre links?"))
		return ;
	esc_home = shell_escape(home_dir);
	if (esc_home)
	{
		command_str = NULL;
		if (asprintf(&command_str, "find %s -maxdepth 4 -type l -exec sh -c '"
				"for link; do target=$(readlink \"$link\"); "
				"case \"$target\" in /goinfre/*|/sgoinfre/*|/tmp/goinfre_*) "
				"if [ ! -e \"$link\" ]; then mkdir -p \"$target\"; fi;; "
				"esac; done' sh {} + 2>/dev/null", esc_home) != -1)
		{
			if (system(command_str))
				(void)0;
			free(command_str);
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

static int	is_zshrc_injected(const char *zshrc_path)
{
	FILE	*file_handle;
	char	line_buffer[512];
	int		is_found;

	file_handle = fopen(zshrc_path, "r");
	if (!file_handle)
		return (0);
	is_found = 0;
	while (fgets(line_buffer, sizeof(line_buffer), file_handle))
	{
		if (strstr(line_buffer, "Goinfre Quota Bypass") != NULL)
		{
			is_found = 1;
			break ;
		}
	}
	fclose(file_handle);
	return (is_found);
}

void	action_inject_zshrc(void)
{
	const char	*home_dir;
	char		zshrc_path[PATH_MAX_LEN];
	FILE		*file_handle;
	size_t		idx;

	home_dir = getenv("HOME");
	if (!home_dir)
		return ;
	snprintf(zshrc_path, sizeof(zshrc_path), "%.2048s/.zshrc", home_dir);
	if (is_zshrc_injected(zshrc_path))
	{
		confirm_modal("INFO", "Goinfre exports already in ~/.zshrc!");
		return ;
	}
	if (!confirm_modal("INJECT ~/.zshrc", "Append goinfre exports?"))
		return ;
	file_handle = fopen(zshrc_path, "a");
	if (!file_handle)
		return ;
	fprintf(file_handle, "\n");
	idx = 0;
	while (g_shell_exports[idx] != NULL)
		fprintf(file_handle, "%s\n", g_shell_exports[idx++]);
	fclose(file_handle);
	confirm_modal("SUCCESS", "Injected! Run: source ~/.zshrc");
}

void	action_bootstrap_goinfre(void)
{
	char	bg[PATH_MAX_LEN];
	char	*cmd;
	size_t	i;

	if (!confirm_modal("BOOTSTRAP", "Link AI models & tools to goinfre?"))
		return ;
	get_goinfre_path(bg, sizeof(bg));
	i = 0;
	while (g_bootstrap_targets[i] != NULL)
	{
		if (asprintf(&cmd,
				"mkdir -p $(dirname %s/%s) && mv ~/%s %s/%s "
				"&& ln -s %s/%s ~/%s",
				bg, g_bootstrap_targets[i], g_bootstrap_targets[i], bg,
				g_bootstrap_targets[i], bg, g_bootstrap_targets[i],
				g_bootstrap_targets[i]) != -1 && system(cmd))
			free(cmd);
		i++;
	}
	start_async_scan(g_state.current_dir);
}
