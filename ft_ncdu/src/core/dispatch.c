/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	handle_toggle_keys(int ch)
{
	if (ch == 'v' || ch == 'M')
		action_batch_invert();
	else if (ch == 'U')
		action_batch_unmark();
	else if (ch == 'A')
	{
		if (g_state.size_mode == SIZE_ACTUAL_DISK)
			g_state.size_mode = SIZE_APPARENT;
		else
			g_state.size_mode = SIZE_ACTUAL_DISK;
		apply_filter();
	}
	else if (ch == 'a')
	{
		g_state.show_hidden = !g_state.show_hidden;
		apply_filter();
	}
	else if (ch == 'o')
		action_cycle_sort_mode();
}

static void	handle_tool_keys(int ch)
{
	if (ch == 'r')
		start_async_scan(g_state.current_dir);
	else if (ch == 'e')
		action_edit();
	else if (ch == 't')
		action_shell();
	else if (ch == '!')
		action_custom_command();
	else if (ch == 'E')
		action_export_report();
	else if (ch == 'p')
		action_file_peek();
	else if (ch == 'P' || ch == ':')
		action_goto_path();
	else if (ch == 'd' || ch == 'x')
		action_delete();
	else
		handle_toggle_keys(ch);
}

static int	handle_goinfre_keys(int ch)
{
	if (ch == 's')
		action_symlink_goinfre();
	else if (ch == 'u')
		action_unlink_goinfre();
	else if (ch == 'H')
		action_heal_symlinks();
	else if (ch == 'b')
		action_bootstrap_goinfre();
	else if (ch == 'T')
		action_empty_trash();
	else if (ch == 'Z')
		action_inject_zshrc();
	else
		return (0);
	return (1);
}

void	handle_action_keys(int ch)
{
	if (handle_goinfre_keys(ch))
		return ;
	if (ch == 'C')
		action_cleaning_presets();
	else if (ch == 'K')
		action_nuke_junk();
	else if (ch == '?' || ch == 'f' || ch == 'F' || ch == 'm')
		show_help_modal();
	else if (ch == ' ' && g_state.filtered_count > 0)
	{
		pthread_mutex_lock(&g_state.lock);
		g_state.filtered[g_state.selected].marked ^= 1;
		pthread_mutex_unlock(&g_state.lock);
	}
	else
		handle_tool_keys(ch);
}
