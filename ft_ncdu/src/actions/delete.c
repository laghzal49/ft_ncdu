/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	exec_rm_rf(const char *path)
{
	char	*esc;
	char	*cmd;

	esc = shell_escape(path);
	if (!esc)
		return ;
	cmd = NULL;
	if (asprintf(&cmd, "rm -rf %s", esc) != -1)
	{
		if (system(cmd))
			(void)0;
		free(cmd);
	}
	free(esc);
}

void	action_batch_unmark(void)
{
	int	i;

	pthread_mutex_lock(&g_state.lock);
	i = 0;
	while (i < g_state.count)
	{
		g_state.entries[i].marked = 0;
		i++;
	}
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
}

static void	delete_single_target(t_file_entry *target)
{
	char	msg[128];

	if (is_protected_target(target->path))
	{
		confirm_modal("BLOCKED", "Cannot delete protected system config file!");
		return ;
	}
	snprintf(msg, sizeof(msg), "Permanently delete: %.38s?", target->name);
	if (confirm_modal("DELETE TARGET", msg))
	{
		exec_rm_rf(target->path);
		start_async_scan(g_state.current_dir);
	}
}

static void	delete_batch_marked(int marked)
{
	char	msg[128];
	int		i;

	snprintf(msg, sizeof(msg), "Permanently delete %d marked items?", marked);
	if (!confirm_modal("BATCH DELETE", msg))
		return ;
	pthread_mutex_lock(&g_state.lock);
	i = 0;
	while (i < g_state.count)
	{
		if (g_state.entries[i].marked
			&& !is_protected_target(g_state.entries[i].path))
			exec_rm_rf(g_state.entries[i].path);
		i++;
	}
	pthread_mutex_unlock(&g_state.lock);
	start_async_scan(g_state.current_dir);
}

void	action_delete(void)
{
	int	marked;

	if (g_state.filtered_count == 0)
		return ;
	marked = count_marked_items();
	if (marked > 0)
		delete_batch_marked(marked);
	else
		delete_single_target(&g_state.filtered[g_state.selected]);
}
