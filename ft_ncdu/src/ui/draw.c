/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	render_header_bar(int max_x)
{
	char	breadcrumbs[PATH_MAX_LEN];
	int		b_len;

	attron(COLOR_PAIR(5) | A_BOLD);
	mvprintw(0, 0, " 1337 | 42 ");
	attroff(COLOR_PAIR(5) | A_BOLD);
	attron(COLOR_PAIR(11));
	printw(" %s v%s ", APP_NAME, APP_VERSION);
	attroff(COLOR_PAIR(11));
	if (g_state.is_scanning)
	{
		attron(COLOR_PAIR(3) | A_BOLD);
		printw(" [SCANNING DIRECTORY TREE...] ");
		wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
	}
	else
	{
		b_len = max_x - 44;
		if (b_len < 6)
			b_len = 6;
		format_breadcrumbs(g_state.current_dir, breadcrumbs, b_len);
		attron(COLOR_PAIR(1) | A_BOLD);
		printw(" %s ", breadcrumbs);
		wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
	}
}

static void	render_search_footer(int foot_y)
{
	attron(COLOR_PAIR(10) | A_BOLD);
	mvprintw(foot_y, 0, " SEARCH ");
	attroff(COLOR_PAIR(10) | A_BOLD);
	attron(COLOR_PAIR(3) | A_BOLD);
	printw(" %s_ (ESC clear, Enter apply)", g_state.search_query);
	wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
}

void	render_status_footer(int max_y, int max_x)
{
	char	sz[16];
	int		foot_y;

	foot_y = max_y - 1;
	if (g_state.is_searching)
	{
		render_search_footer(foot_y);
		return ;
	}
	attron(COLOR_PAIR(5) | A_BOLD);
	mvprintw(foot_y, 0, " NORMAL ");
	attroff(COLOR_PAIR(5) | A_BOLD);
	format_size(g_state.total_disk_usage, sz, sizeof(sz));
	attron(COLOR_PAIR(7));
	printw(" Items: %d (%s) │ Marked: %d ",
		g_state.filtered_count, sz, count_marked_items());
	attroff(COLOR_PAIR(7));
	if (max_x > 20)
	{
		attron(COLOR_PAIR(11));
		mvprintw(foot_y, max_x - 14, " '?' For Help ");
		attroff(COLOR_PAIR(11));
	}
}

void	draw_ui(void)
{
	int		max_y;
	int		max_x;
	int		split_x;
	int		body_h;
	t_rect	r;

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
	body_h = max_y - 6;
	r = (t_rect){4, 0, body_h, split_x};
	render_file_table(r, split_x);
	r = (t_rect){4, split_x, body_h, max_x - split_x};
	render_inspector(r, split_x);
	render_status_footer(max_y, max_x);
	refresh();
}
