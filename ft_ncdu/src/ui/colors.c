/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
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
	init_pair(15, COLOR_WHITE, COLOR_GREEN);
	init_pair(16, COLOR_YELLOW, COLOR_RED);
}

static void	draw_box_borders(t_rect r)
{
	int	i;

	mvaddch(r.y, r.x, ACS_ULCORNER);
	mvhline(r.y, r.x + 1, ACS_HLINE, r.w - 2);
	mvaddch(r.y, r.x + r.w - 1, ACS_URCORNER);
	i = 1;
	while (i < r.h - 1)
	{
		mvaddch(r.y + i, r.x, ACS_VLINE);
		mvaddch(r.y + i, r.x + r.w - 1, ACS_VLINE);
		i++;
	}
	mvaddch(r.y + r.h - 1, r.x, ACS_LLCORNER);
	mvhline(r.y + r.h - 1, r.x + 1, ACS_HLINE, r.w - 2);
	mvaddch(r.y + r.h - 1, r.x + r.w - 1, ACS_LRCORNER);
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
			mvaddstr(r.y, r.x + 1, "[");
			attron(A_BOLD);
			mvprintw(r.y, r.x + 2, "%s", title);
			wattroff(stdscr, A_BOLD);
			mvaddstr(r.y, r.x + tlen + 2, "]");
		}
	}
	attroff(COLOR_PAIR(color));
}
