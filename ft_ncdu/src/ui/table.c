/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	get_type_info(t_file_entry *entry, int *badge_color,
		const char **badge_label)
{
	*badge_color = 10;
	*badge_label = "FILE";
	if (entry->type == TYPE_DIR)
	{
		*badge_color = 8;
		*badge_label = "DIR ";
	}
	else if (entry->type == TYPE_LINK)
	{
		if (entry->is_broken_link)
		{
			*badge_color = 12;
			*badge_label = "DEAD";
		}
		else
		{
			*badge_color = 9;
			*badge_label = "LINK";
		}
	}
}

static void	render_row_active(int row_y, t_file_entry *entry, int split_x,
		const char *size_str)
{
	int			badge_color;
	const char	*badge_label;
	char		bar_graph[32];
	int			name_width;

	name_width = split_x - 36;
	if (name_width < 4)
		name_width = 4;
	render_graph_bar(bar_graph, entry->disk_size, g_state.max_item_size, 8);
	get_type_info(entry, &badge_color, &badge_label);
	attron(COLOR_PAIR(5) | A_BOLD);
	if (entry->marked)
		mvprintw(row_y, 1, " ❯ ✔ ");
	else
		mvprintw(row_y, 1, " ❯   ");
	attron(COLOR_PAIR(badge_color));
	printw(" %s ", badge_label);
	attroff(COLOR_PAIR(badge_color));
	attron(COLOR_PAIR(5) | A_BOLD);
	printw(" %s %s %-*.*s", size_str, bar_graph, name_width, name_width,
		entry->name);
	wattroff(stdscr, COLOR_PAIR(5) | A_BOLD);
}

static void	render_row_inactive(int row_y, t_file_entry *entry, int split_x,
		const char *size_str)
{
	int			badge_color;
	const char	*badge_label;
	char		bar_graph[32];
	int			name_width;

	name_width = split_x - 36;
	if (name_width < 4)
		name_width = 4;
	render_graph_bar(bar_graph, entry->disk_size, g_state.max_item_size, 8);
	get_type_info(entry, &badge_color, &badge_label);
	if (entry->marked)
		mvprintw(row_y, 1, "   ✔ ");
	else
		mvprintw(row_y, 1, "     ");
	attron(COLOR_PAIR(badge_color));
	printw(" %s ", badge_label);
	attroff(COLOR_PAIR(badge_color));
	if (entry->type == TYPE_DIR)
		attron(COLOR_PAIR(1));
	printw(" %s %s %-*.*s", size_str, bar_graph, name_width, name_width,
		entry->name);
	if (entry->type == TYPE_DIR)
		attroff(COLOR_PAIR(1));
}

void	render_file_table(t_rect rect, int split_x)
{
	int				row_idx;
	int				item_idx;
	char			size_str[16];
	t_file_entry	*entry;

	draw_box(rect, " FINDER EXPLORER", 1);
	attron(COLOR_PAIR(14) | A_BOLD);
	mvprintw(rect.y + 1, 2, "ST  TYPE     SIZE     ALLOCATION %%       NAME");
	wattroff(stdscr, COLOR_PAIR(14) | A_BOLD);
	pthread_mutex_lock(&g_state.lock);
	row_idx = 0;
	while (row_idx < rect.h - 3
		&& (g_state.scroll_offset + row_idx) < g_state.filtered_count)
	{
		item_idx = g_state.scroll_offset + row_idx;
		entry = &g_state.filtered[item_idx];
		format_size(entry->disk_size, size_str, sizeof(size_str));
		if (item_idx == g_state.selected)
			render_row_active(rect.y + 2 + row_idx, entry, split_x, size_str);
		else
			render_row_inactive(rect.y + 2 + row_idx, entry, split_x, size_str);
		row_idx++;
	}
	pthread_mutex_unlock(&g_state.lock);
}
