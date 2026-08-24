/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   goinfre.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

void	get_goinfre_path(char *dest_path, size_t dest_len)
{
	const char	*home_dir;
	char		local_goinfre[PATH_MAX_LEN];

	if (access("/goinfre", F_OK) == 0)
		snprintf(dest_path, dest_len, "/goinfre/%.128s", g_state.username);
	else if (access("/sgoinfre", F_OK) == 0)
		snprintf(dest_path, dest_len, "/sgoinfre/%.128s", g_state.username);
	else if (access("/Volumes/Storage/goinfre", F_OK) == 0)
		snprintf(dest_path, dest_len,
			"/Volumes/Storage/goinfre/%.128s", g_state.username);
	else
	{
		home_dir = getenv("HOME");
		if (home_dir)
		{
			snprintf(local_goinfre, sizeof(local_goinfre), "%.2048s/goinfre",
				home_dir);
			if (access(local_goinfre, F_OK) == 0)
			{
				safe_str_copy(dest_path, local_goinfre, dest_len);
				return ;
			}
		}
		snprintf(dest_path, dest_len, "/tmp/goinfre_%.128s", g_state.username);
	}
}

static void	exec_move_symlink(const char *source_path,
		const char *destination_path, const char *base_goinfre)
{
	char	*esc_base;
	char	*esc_dest;
	char	*esc_src;
	char	*command_str;

	esc_base = shell_escape(base_goinfre);
	esc_dest = shell_escape(destination_path);
	esc_src = shell_escape(source_path);
	if (esc_base && esc_dest && esc_src)
	{
		command_str = NULL;
		if (asprintf(&command_str,
				"mkdir -p %s && rm -rf %s && mv %s %s && ln -s %s %s",
				esc_base, esc_dest, esc_src, esc_dest,
				esc_dest, esc_src) != -1)
		{
			if (system(command_str))
				(void)0;
			free(command_str);
		}
	}
	free(esc_base);
	free(esc_dest);
	free(esc_src);
}

void	action_symlink_goinfre(void)
{
	char			base_goinfre[PATH_MAX_LEN];
	char			destination_path[PATH_MAX_LEN];
	t_file_entry	*target_entry;

	if (g_state.filtered_count == 0)
		return ;
	get_goinfre_path(base_goinfre, sizeof(base_goinfre));
	target_entry = &g_state.filtered[g_state.selected];
	if (target_entry->type == TYPE_LINK && target_entry->is_goinfre_link)
	{
		confirm_modal("INFO", "Target is already a symlink to goinfre!");
		return ;
	}
	snprintf(destination_path, sizeof(destination_path), "%.2048s/%.256s",
		base_goinfre, target_entry->name);
	if (confirm_modal("GOINFRE SYMLINK", "Move target to /goinfre & link?"))
	{
		exec_move_symlink(target_entry->path, destination_path, base_goinfre);
		start_async_scan(g_state.current_dir);
	}
}

static void	exec_restore_link(const char *symlink_path,
		const char *real_target_path)
{
	char	*esc_link;
	char	*esc_target;
	char	*command_str;

	esc_link = shell_escape(symlink_path);
	esc_target = shell_escape(real_target_path);
	if (esc_link && esc_target && asprintf(&command_str, "rm -f %s && mv %s %s",
			esc_link, esc_target, esc_link) != -1)
	{
		if (system(command_str))
			(void)0;
		free(command_str);
	}
	free(esc_link);
	free(esc_target);
}

void	action_unlink_goinfre(void)
{
	t_file_entry	*target_entry;

	if (g_state.filtered_count == 0)
		return ;
	target_entry = &g_state.filtered[g_state.selected];
	if (target_entry->type != TYPE_LINK
		|| strlen(target_entry->symlink_target) == 0)
	{
		confirm_modal("INFO", "Target is not a valid symlink to restore.");
		return ;
	}
	if (!confirm_modal("RESTORE TARGET", "Restore real data back to HOME?"))
		return ;
	exec_restore_link(target_entry->path, target_entry->symlink_target);
	start_async_scan(g_state.current_dir);
}
