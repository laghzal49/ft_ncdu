/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nav.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 12:00:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/27 10:00:00 by tlaghzal         ###   ########.fr       */
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

static void	handle_nav_jump(int ch, int list_h)
{
	if (ch == 'g' || ch == KEY_HOME)
	{
		g_state.selected = 0;
		g_state.scroll_offset = 0;
	}
	else if (ch == 'G' || ch == KEY_END)
	{
		if (g_state.filtered_count > 0)
			g_state.selected = g_state.filtered_count - 1;
		if (g_state.selected >= g_state.scroll_offset + list_h)
			g_state.scroll_offset = g_state.selected - list_h + 1;
	}
	else if (ch == KEY_NPAGE)
	{
		g_state.selected += list_h;
		if (g_state.selected >= g_state.filtered_count)
			g_state.selected = g_state.filtered_count - 1;
	}
	else if (ch == KEY_PPAGE)
	{
		g_state.selected -= list_h;
		if (g_state.selected < 0)
			g_state.selected = 0;
	}
}

static int	handle_nav_step(int ch, int list_h)
{
	if ((ch == 'j' || ch == KEY_DOWN)
		&& g_state.selected < g_state.filtered_count - 1)
	{
		g_state.selected++;
		if (g_state.selected >= g_state.scroll_offset + list_h)
			g_state.scroll_offset++;
		return (1);
	}
	if ((ch == 'k' || ch == KEY_UP) && g_state.selected > 0)
	{
		g_state.selected--;
		if (g_state.selected < g_state.scroll_offset)
			g_state.scroll_offset--;
		return (1);
	}
	return (0);
}

void	action_cycle_sort_mode(void)
{
	if (g_state.sort_mode == SORT_SIZE_DESC)
		g_state.sort_mode = SORT_SIZE_ASC;
	else if (g_state.sort_mode == SORT_SIZE_ASC)
		g_state.sort_mode = SORT_NAME_ASC;
	else if (g_state.sort_mode == SORT_NAME_ASC)
		g_state.sort_mode = SORT_MTIME_DESC;
	else
		g_state.sort_mode = SORT_SIZE_DESC;
	pthread_mutex_lock(&g_state.lock);
	qsort(g_state.entries, g_state.count,
		sizeof(t_file_entry), compare_entries);
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
}

void	handle_nav_keys(int ch, int list_h)
{
	const char	*home;

	if (handle_nav_step(ch, list_h))
		return ;
	if (ch == 'h' || ch == KEY_BACKSPACE || ch == 127 || ch == '-')
		step_out_directory();
	else if (ch == '~')
	{
		home = getenv("HOME");
		if (!home)
			home = ".";
		start_async_scan(home);
	}
	else
		handle_nav_jump(ch, list_h);
}
