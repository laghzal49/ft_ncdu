/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   goinfre.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

void	get_goinfre_path(char *dest, size_t dest_len)
{
	const char	*home;
	char		local_g[PATH_MAX_LEN];

	if (access("/goinfre", F_OK) == 0)
		snprintf(dest, dest_len, "/goinfre/%.128s", g_state.username);
	else if (access("/sgoinfre", F_OK) == 0)
		snprintf(dest, dest_len, "/sgoinfre/%.128s", g_state.username);
	else if (access("/Volumes/Storage/goinfre", F_OK) == 0)
		snprintf(dest, dest_len,
			"/Volumes/Storage/goinfre/%.128s", g_state.username);
	else
	{
		home = getenv("HOME");
		if (home)
		{
			snprintf(local_g, sizeof(local_g), "%.2048s/goinfre", home);
			if (access(local_g, F_OK) == 0)
			{
				safe_str_copy(dest, local_g, dest_len);
				return ;
			}
		}
		snprintf(dest, dest_len, "/tmp/goinfre_%.128s", g_state.username);
	}
}

static void	exec_move_symlink(const char *src, const char *dest, const char *bg)
{
	char	*esc_bg;
	char	*esc_dst;
	char	*esc_src;
	char	*cmd;

	esc_bg = shell_escape(bg);
	esc_dst = shell_escape(dest);
	esc_src = shell_escape(src);
	if (esc_bg && esc_dst && esc_src)
	{
		cmd = NULL;
		if (asprintf(&cmd,
				"mkdir -p %s && rm -rf %s && mv %s %s && ln -s %s %s",
				esc_bg, esc_dst, esc_src, esc_dst, esc_dst, esc_src) != -1)
		{
			if (system(cmd))
				(void)0;
			free(cmd);
		}
	}
	free(esc_bg);
	free(esc_dst);
	free(esc_src);
}

void	action_symlink_goinfre(void)
{
	char			base_g[PATH_MAX_LEN];
	char			dest[PATH_MAX_LEN];
	t_file_entry	*target;

	if (g_state.filtered_count == 0)
		return ;
	get_goinfre_path(base_g, sizeof(base_g));
	target = &g_state.filtered[g_state.selected];
	if (target->type == TYPE_LINK && target->is_goinfre_link)
	{
		confirm_modal("INFO", "Target is already a symlink to goinfre!");
		return ;
	}
	snprintf(dest, sizeof(dest), "%.2048s/%.256s", base_g, target->name);
	if (confirm_modal("GOINFRE SYMLINK", "Move target to /goinfre & link?"))
	{
		exec_move_symlink(target->path, dest, base_g);
		start_async_scan(g_state.current_dir);
	}
}

static void	exec_restore_link(const char *link, const char *target)
{
	char	*esc_link;
	char	*esc_tgt;
	char	*cmd;

	esc_link = shell_escape(link);
	esc_tgt = shell_escape(target);
	if (esc_link && esc_tgt && asprintf(&cmd, "rm -f %s && mv %s %s",
			esc_link, esc_tgt, esc_link) != -1)
	{
		if (system(cmd))
			(void)0;
		free(cmd);
	}
	free(esc_link);
	free(esc_tgt);
}

void	action_unlink_goinfre(void)
{
	t_file_entry	*target;

	if (g_state.filtered_count == 0)
		return ;
	target = &g_state.filtered[g_state.selected];
	if (target->type != TYPE_LINK || strlen(target->symlink_target) == 0)
	{
		confirm_modal("INFO", "Target is not a valid symlink to restore.");
		return ;
	}
	if (!confirm_modal("RESTORE TARGET", "Restore real data back to HOME?"))
		return ;
	exec_restore_link(target->path, target->symlink_target);
	start_async_scan(g_state.current_dir);
}
