/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	render_header_bar(int max_x)
{
	char	breadcrumb_str[PATH_MAX_LEN];
	char	status[32];
	int		path_width;

	safe_str_copy(status, g_state.is_scanning ? "SCANNING" : "READY",
		sizeof(status));
	path_width = max_x - 28;
	if (max_x < 54)
		path_width = max_x - 13;
	format_breadcrumbs(g_state.current_dir, breadcrumb_str, path_width);
	attron(A_REVERSE | A_BOLD);
	mvhline(0, 0, ' ', max_x);
	mvprintw(0, 1, " %s ", APP_NAME);
	attroff(A_BOLD);
	printw(" %s", breadcrumb_str);
	if (max_x >= 54)
		mvprintw(0, max_x - (int)strlen(status) - 3, " %s ", status);
	attroff(A_REVERSE);
}

static void	render_search_footer(int footer_y)
{
	int	query_width;

	query_width = COLS - 31;
	if (query_width < 4)
		query_width = 4;
	if (COLS < 44)
	{
		attron(A_REVERSE | A_BOLD);
		mvprintw(footer_y, 0, " / %.*s_", COLS - 5, g_state.search_query);
		attroff(A_REVERSE | A_BOLD);
		return ;
	}
	query_width = COLS - 34;
	attron(COLOR_PAIR(10) | A_BOLD);
	mvprintw(footer_y, 0, " SEARCH ");
	attroff(COLOR_PAIR(10) | A_BOLD);
	attron(COLOR_PAIR(3) | A_BOLD);
	printw(" %.*s_  Enter apply  Esc close", query_width,
		g_state.search_query);
	wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
}

static const char	*get_sort_tag(void)
{
	if (g_state.sort_mode == SORT_SIZE_ASC)
		return ("Sort: Size ASC");
	if (g_state.sort_mode == SORT_NAME_ASC)
		return ("Sort: Name");
	if (g_state.sort_mode == SORT_MTIME_DESC)
		return ("Sort: Date");
	return ("Sort: Size DESC");
}

void	render_status_footer(int max_y, int max_x)
{
	char	size_str[16];
	int		footer_y;

	footer_y = max_y - 1;
	if (g_state.is_searching)
	{
		render_search_footer(footer_y);
		return ;
	}
	attron(A_REVERSE | A_BOLD);
	mvhline(footer_y, 0, ' ', max_x);
	mvprintw(footer_y, 1, " %d items ", g_state.filtered_count);
	attroff(A_REVERSE | A_BOLD);
	format_size(g_state.total_disk_usage, size_str, sizeof(size_str));
	attron(COLOR_PAIR(7));
	if (max_x >= 58)
		printw(" %s used | %d marked | %s", size_str,
			count_marked_items(), get_sort_tag());
	else
		printw(" %s | %d marked", size_str, count_marked_items());
	attroff(COLOR_PAIR(7));
	if (max_x > 76)
	{
		attron(A_BOLD);
		mvprintw(footer_y, max_x - 18, " ? help   q quit ");
		attroff(A_BOLD);
	}
}

void	draw_ui(void)
{
	int		max_y;
	int		max_x;
	int		split_x;
	int		body_height;
	t_rect	rect;

	erase();
	getmaxyx(stdscr, max_y, max_x);
	if (max_y < 10 || max_x < 30)
	{
		mvprintw(0, 0, "Need a 30x10 terminal (now %dx%d)", max_x, max_y);
		refresh();
		return ;
	}
	render_header_bar(max_x);
	render_top_hud(max_x);
	split_x = (max_x * 64) / 100;
	if (max_x < 104)
		split_x = max_x;
	body_height = max_y - 4;
	rect = (t_rect){3, 0, body_height, split_x};
	render_file_table(rect, split_x);
	if (split_x < max_x)
	{
		rect = (t_rect){3, split_x, body_height, max_x - split_x};
		render_inspector(rect, split_x);
	}
	render_status_footer(max_y, max_x);
	refresh();
}
