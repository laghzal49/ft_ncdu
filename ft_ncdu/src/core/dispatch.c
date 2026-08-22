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

static void	step_out_directory(void)
{
	char	parent[PATH_MAX_LEN];
	char	resolved[PATH_MAX_LEN];

	snprintf(parent, sizeof(parent), "%.4000s/..", g_state.current_dir);
	if (realpath(parent, resolved))
		start_async_scan(resolved);
}

void	handle_nav_keys(int ch, int list_h)
{
	const char	*home;

	if ((ch == 'j' || ch == KEY_DOWN)
		&& g_state.selected < g_state.filtered_count - 1)
	{
		g_state.selected++;
		if (g_state.selected >= g_state.scroll_offset + list_h)
			g_state.scroll_offset++;
	}
	else if ((ch == 'k' || ch == KEY_UP) && g_state.selected > 0)
	{
		g_state.selected--;
		if (g_state.selected < g_state.scroll_offset)
			g_state.scroll_offset--;
	}
	else if (ch == 'h' || ch == KEY_BACKSPACE || ch == 127 || ch == '-')
		step_out_directory();
	else if (ch == '~')
	{
		home = getenv("HOME");
		if (!home)
			home = ".";
		start_async_scan(home);
	}
}

static void	handle_toggle_keys(int ch)
{
	if (ch == 'v'
		|| ch == 'M')
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
}

static void	handle_extra_keys(int ch)
{
	if (ch == 'p')
		action_file_peek();
	else if (ch == 'P'
		|| ch == ':')
		action_goto_path();
	else if (ch == 'd'
		|| ch == 'x')
		action_delete();
	else if (ch == '?')
		show_help_modal();
	else if (ch == ' ')
	{
		if (g_state.filtered_count > 0)
		{
			pthread_mutex_lock(&g_state.lock);
			g_state.filtered[g_state.selected].marked ^= 1;
			pthread_mutex_unlock(&g_state.lock);
		}
	}
	else
		handle_toggle_keys(ch);
}

void	handle_action_keys(int ch)
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
	else if (ch == 'C')
		action_cleaning_presets();
	else if (ch == 'K')
		action_nuke_junk();
	else
		handle_extra_keys(ch);
}
