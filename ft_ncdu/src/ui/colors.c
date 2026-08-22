/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	init_ui_colors(void)
{
	if (!has_colors())
		return ;
	start_color();
	use_default_colors();
	init_pair(1, COLOR_CYAN, -1);
	init_pair(2, COLOR_GREEN, -1);
	init_pair(3, COLOR_YELLOW, -1);
	init_pair(4, COLOR_RED, -1);
	init_pair(5, COLOR_BLACK, COLOR_CYAN);
	init_pair(6, COLOR_MAGENTA, -1);
	init_pair(7, COLOR_WHITE, -1);
	init_pair(8, COLOR_BLACK, COLOR_GREEN);
	init_pair(9, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(10, COLOR_WHITE, COLOR_BLUE);
	init_pair(11, COLOR_BLACK, COLOR_WHITE);
	init_pair(12, COLOR_WHITE, COLOR_RED);
	init_pair(13, COLOR_BLACK, COLOR_YELLOW);
	init_pair(14, COLOR_CYAN, COLOR_BLACK);
}

static void	draw_box_borders(t_rect r)
{
	int	i;

	mvaddstr(r.y, r.x, "╭");
	i = 1;
	while (i < r.w - 1)
		mvaddstr(r.y, r.x + i++, "─");
	mvaddstr(r.y, r.x + r.w - 1, "╮");
	i = 1;
	while (i < r.h - 1)
	{
		mvaddstr(r.y + i, r.x, "│");
		mvaddstr(r.y + i, r.x + r.w - 1, "│");
		i++;
	}
	mvaddstr(r.y + r.h - 1, r.x, "╰");
	i = 1;
	while (i < r.w - 1)
		mvaddstr(r.y + r.h - 1, r.x + i++, "─");
	mvaddstr(r.y + r.h - 1, r.x + r.w - 1, "╯");
}

void	draw_box(t_rect r, const char *title, int color)
{
	int	tlen;

	if (r.h < 2 || r.w < 2)
		return ;
	attron(COLOR_PAIR(color));
	draw_box_borders(r);
	if (title && title[0] != '\0')
	{
		tlen = strlen(title);
		if (tlen + 4 < r.w)
		{
			attron(A_BOLD);
			mvprintw(r.y, r.x + 2, " %s ", title);
			wattroff(stdscr, A_BOLD);
		}
	}
	attroff(COLOR_PAIR(color));
}
