/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	get_type_info(t_file_entry *fe, int *badge, const char **b_str)
{
	*badge = 10;
	*b_str = "FILE";
	if (fe->type == TYPE_DIR)
	{
		*badge = 8;
		*b_str = "DIR ";
	}
	else if (fe->type == TYPE_LINK)
	{
		if (fe->is_broken_link)
		{
			*badge = 12;
			*b_str = "DEAD";
		}
		else
		{
			*badge = 9;
			*b_str = "LINK";
		}
	}
}

static void	render_row_active(int y, t_file_entry *fe, int split_x,
		const char *sz)
{
	int			badge;
	const char	*b_str;
	const char	*msym;
	char		graph[32];
	off_t		eff_sz;

	eff_sz = fe->size;
	if (g_state.size_mode == SIZE_ACTUAL_DISK)
		eff_sz = fe->disk_size;
	render_graph_bar(graph, eff_sz, g_state.max_item_size, 8);
	get_type_info(fe, &badge, &b_str);
	msym = " ";
	if (fe->marked)
		msym = "✔";
	attron(COLOR_PAIR(5) | A_BOLD);
	mvprintw(y, 1, " ❯ %s ", msym);
	attron(COLOR_PAIR(badge));
	printw(" %s ", b_str);
	attroff(COLOR_PAIR(badge));
	attron(COLOR_PAIR(5) | A_BOLD);
	printw(" %s %s %-*.*s", sz, graph, split_x - 36, split_x - 36, fe->name);
	wattroff(stdscr, COLOR_PAIR(5) | A_BOLD);
}

static void	render_row_inactive(int y, t_file_entry *fe, int split_x,
		const char *sz)
{
	int			badge;
	const char	*b_str;
	const char	*msym;
	char		graph[32];
	int			ncol;

	render_graph_bar(graph, fe->size, g_state.max_item_size, 8);
	get_type_info(fe, &badge, &b_str);
	msym = " ";
	if (fe->marked)
		msym = "✔";
	mvprintw(y, 1, "   %s ", msym);
	attron(COLOR_PAIR(badge));
	printw(" %s ", b_str);
	attroff(COLOR_PAIR(badge));
	ncol = 7;
	if (fe->type == TYPE_DIR)
		ncol = 1;
	else if (fe->is_broken_link)
		ncol = 4;
	printw(" %s %s ", sz, graph);
	attron(COLOR_PAIR(ncol));
	printw("%-*.*s", split_x - 36, split_x - 36, fe->name);
	attroff(COLOR_PAIR(ncol));
}

void	render_file_table(t_rect r, int split_x)
{
	int				i;
	int				idx;
	char			sz[16];
	t_file_entry	*fe;

	draw_box(r, "CLUSTER EXPLORER", 1);
	attron(COLOR_PAIR(14) | A_BOLD);
	mvprintw(r.y + 1, 2, "ST  TYPE     SIZE     ALLOCATION %%       NAME");
	wattroff(stdscr, COLOR_PAIR(14) | A_BOLD);
	pthread_mutex_lock(&g_state.lock);
	i = 0;
	while (i < r.h - 3 && (g_state.scroll_offset + i) < g_state.filtered_count)
	{
		idx = g_state.scroll_offset + i;
		fe = &g_state.filtered[idx];
		format_size(fe->size, sz, sizeof(sz));
		if (idx == g_state.selected)
			render_row_active(r.y + 2 + i, fe, split_x, sz);
		else
			render_row_inactive(r.y + 2 + i, fe, split_x, sz);
		i++;
	}
	pthread_mutex_unlock(&g_state.lock);
}
