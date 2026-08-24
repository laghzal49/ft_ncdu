/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	handle_nav_keys(int ch, int list_h);
void	handle_action_keys(int ch);

static void	handle_search_input(int ch)
{
	size_t	len;

	if (ch == 27 || ch == 10)
		g_state.is_searching = 0;
	else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8)
	{
		len = strlen(g_state.search_query);
		if (len > 0)
			g_state.search_query[len - 1] = '\0';
		apply_filter();
	}
	else if (ch >= 32 && ch <= 126)
	{
		len = strlen(g_state.search_query);
		if (len < sizeof(g_state.search_query) - 2)
		{
			g_state.search_query[len] = (char)ch;
			g_state.search_query[len + 1] = '\0';
			apply_filter();
		}
	}
}

static void	process_input_key(int ch, int max_y)
{
	if (ch == 'q' || ch == KEY_RESIZE)
		return ;
	if (ch == '/')
		g_state.is_searching = 1;
	else if ((ch == 'l' || ch == 10 || ch == KEY_RIGHT)
		&& g_state.filtered_count > 0
		&& g_state.filtered[g_state.selected].type == TYPE_DIR)
		start_async_scan(g_state.filtered[g_state.selected].path);
	else
	{
		handle_nav_keys(ch, max_y - 6);
		handle_action_keys(ch);
	}
}

void	run_event_loop(void)
{
	int	ch;
	int	max_y;

	while (1)
	{
		draw_ui();
		ch = getch();
		if (ch == ERR)
			continue ;
		max_y = getmaxy(stdscr);
		if (g_state.is_searching)
			handle_search_input(ch);
		else if (ch == 'q')
			break ;
		else
			process_input_key(ch, max_y);
	}
}
