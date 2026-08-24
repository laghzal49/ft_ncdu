/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   modals.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

int	confirm_modal(const char *title, const char *message)
{
	WINDOW	*win;
	int		ch;

	win = newwin(8, 68, (LINES - 8) / 2, (COLS - 68) / 2);
	if (!win)
		return (0);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [  %s ] ::", title);
	wattroff(win, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(win, 3, 2, " %.*s", 64, message);
	wattron(win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win, 5, 2, " [Y] Confirm  │  [N] / [ESC] Cancel");
	wattroff(win, COLOR_PAIR(2) | A_BOLD);
	wrefresh(win);
	wtimeout(win, -1);
	ch = wgetch(win);
	delwin(win);
	return (ch == 'y' || ch == 'Y');
}

static void	render_help_part1(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 2, 2, " 🧭 FINDER NAVIGATION");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 3, 4, "j / k / Arrows   : Navigate Rows");
	mvwprintw(win, 4, 4, "g / G / PgUp/PgDn: Top / Bottom / Page");
	mvwprintw(win, 5, 4, "l / Enter / h    : Open Folder / Parent");
	mvwprintw(win, 6, 4, "~ / P (or :)     : Jump HOME / Teleport");
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 8, 2, " ⚡ 42 CLUSTER ACTIONS");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 9, 4, "s / u            : Link / Unlink Goinfre");
	mvwprintw(win, 10, 4, "H / b            : Heal Station / Bootstrap");
	mvwprintw(win, 11, 4, "T / Z            : Empty Trash / Inject .zshrc");
	mvwprintw(win, 12, 4, "C / K            : 9 Presets / Quick Nuke");
}

static void	render_help_part2(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 14, 2, " 🛠️ SELECTION & TOOLS");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 15, 4, "Space / v / U    : Mark / Invert / Clear");
	mvwprintw(win, 16, 4, "d / x            : Safe Delete");
	mvwprintw(win, 17, 4, "p / e / t / !    : Peek / Edit / Shell / Exec");
	mvwprintw(win, 18, 4, "A / a / o / r    : Size / Hidden / Sort / Reload");
	mvwprintw(win, 19, 4, "E / /            : Export Report / Search");
	wattron(win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win, 20, 2, " [Press any key to close]");
	wattroff(win, COLOR_PAIR(2) | A_BOLD);
}

void	show_help_modal(void)
{
	WINDOW	*win;
	int		h;
	int		w;

	h = 22;
	w = 70;
	if (LINES < 23 || COLS < 72)
	{
		h = LINES - 2;
		w = COLS - 2;
	}
	win = newwin(h, w, (LINES - h) / 2, (COLS - w) / 2);
	if (!win)
		return ;
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [  FT_NCDU ALL FEATURES (Press F / ?) ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	render_help_part1(win);
	render_help_part2(win);
	wrefresh(win);
	wtimeout(win, -1);
	wgetch(win);
	delwin(win);
}
