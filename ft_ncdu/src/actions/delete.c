/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	exec_rm_rf(const char *target_path)
{
	char	*escaped_path;
	char	*command_str;

	escaped_path = shell_escape(target_path);
	if (!escaped_path)
		return ;
	command_str = NULL;
	if (asprintf(&command_str, "rm -rf %s", escaped_path) != -1)
	{
		if (system(command_str))
			(void)0;
		free(command_str);
	}
	free(escaped_path);
}

void	action_batch_unmark(void)
{
	int	idx;

	pthread_mutex_lock(&g_state.lock);
	idx = 0;
	while (idx < g_state.count)
	{
		g_state.entries[idx].marked = 0;
		idx++;
	}
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
}

static void	delete_single_target(t_file_entry *target)
{
	char	modal_message[128];

	if (is_protected_target(target->path))
	{
		confirm_modal("BLOCKED", "Cannot delete protected system config file!");
		return ;
	}
	snprintf(modal_message, sizeof(modal_message),
		"Permanently delete: %.38s?", target->name);
	if (confirm_modal("DELETE TARGET", modal_message))
	{
		exec_rm_rf(target->path);
		start_async_scan(g_state.current_dir);
	}
}

static void	delete_batch_marked(int marked_count)
{
	char	modal_message[128];
	int		idx;

	snprintf(modal_message, sizeof(modal_message),
		"Permanently delete %d marked items?", marked_count);
	if (!confirm_modal("BATCH DELETE", modal_message))
		return ;
	pthread_mutex_lock(&g_state.lock);
	idx = 0;
	while (idx < g_state.count)
	{
		if (g_state.entries[idx].marked
			&& !is_protected_target(g_state.entries[idx].path))
			exec_rm_rf(g_state.entries[idx].path);
		idx++;
	}
	pthread_mutex_unlock(&g_state.lock);
	start_async_scan(g_state.current_dir);
}

void	action_delete(void)
{
	int	marked_count;

	if (g_state.filtered_count == 0)
		return ;
	marked_count = count_marked_items();
	if (marked_count > 0)
		delete_batch_marked(marked_count);
	else
		delete_single_target(&g_state.filtered[g_state.selected]);
}
