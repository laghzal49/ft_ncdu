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
	int		breadcrumb_len;

	attron(COLOR_PAIR(4) | A_BOLD);
	mvprintw(0, 1, "● ");
	attron(COLOR_PAIR(3));
	printw("● ");
	attron(COLOR_PAIR(2));
	printw("●");
	attroff(COLOR_PAIR(2) | A_BOLD);
	attron(COLOR_PAIR(11));
	mvprintw(0, 8, "  1337 | 42 ");
	attroff(COLOR_PAIR(11));
	attron(COLOR_PAIR(5) | A_BOLD);
	printw(" %s v%s ", APP_NAME, APP_VERSION);
	wattroff(stdscr, COLOR_PAIR(5) | A_BOLD);
	breadcrumb_len = max_x - 48;
	if (breadcrumb_len < 6)
		breadcrumb_len = 6;
	format_breadcrumbs(g_state.current_dir, breadcrumb_str, breadcrumb_len);
	attron(COLOR_PAIR(1) | A_BOLD);
	printw(" 📁 %s ", breadcrumb_str);
	wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
}

static void	render_search_footer(int footer_y)
{
	attron(COLOR_PAIR(10) | A_BOLD);
	mvprintw(footer_y, 0, " 🔍 SPOTLIGHT ");
	attroff(COLOR_PAIR(10) | A_BOLD);
	attron(COLOR_PAIR(3) | A_BOLD);
	printw(" %s_ (ESC clear, Enter apply)", g_state.search_query);
	wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
}

static const char	*get_sort_tag(void)
{
	if (g_state.sort_mode == SORT_SIZE_ASC)
		return ("Sort: Size▲");
	if (g_state.sort_mode == SORT_NAME_ASC)
		return ("Sort: Name");
	if (g_state.sort_mode == SORT_MTIME_DESC)
		return ("Sort: Date");
	return ("Sort: Size▼");
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
	attron(COLOR_PAIR(5) | A_BOLD);
	mvprintw(footer_y, 0, "  FINDER ");
	attroff(COLOR_PAIR(5) | A_BOLD);
	format_size(g_state.total_disk_usage, size_str, sizeof(size_str));
	attron(COLOR_PAIR(7));
	printw(" %d items, %s used │ Marked: %d │ %s ",
		g_state.filtered_count, size_str, count_marked_items(), get_sort_tag());
	attroff(COLOR_PAIR(7));
	if (max_x > 36)
	{
		attron(COLOR_PAIR(11));
		mvprintw(footer_y, max_x - 22, " [F / ?] Features │ ⌘Q ");
		attroff(COLOR_PAIR(11));
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
	if (max_y < 12 || max_x < 40)
	{
		mvprintw(0, 0, "Terminal window too small! (Min 40x12)");
		refresh();
		return ;
	}
	render_header_bar(max_x);
	render_top_hud(max_x);
	split_x = (max_x * 58) / 100;
	body_height = max_y - 6;
	rect = (t_rect){4, 0, body_height, split_x};
	render_file_table(rect, split_x);
	rect = (t_rect){4, split_x, body_height, max_x - split_x};
	render_inspector(rect, split_x);
	render_status_footer(max_y, max_x);
	refresh();
}
