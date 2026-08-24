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

	win = newwin(9, 70, (LINES - 9) / 2, (COLS - 70) / 2);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [  %s ] ::", title);
	wattroff(win, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(win, 3, 2, " %.*s", 66, message);
	wattron(win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win, 6, 2, " [Y] Confirm  │  [N] / [ESC] Cancel");
	wattroff(win, COLOR_PAIR(2) | A_BOLD);
	wrefresh(win);
	wtimeout(win, -1);
	ch = wgetch(win);
	delwin(win);
	return (ch == 'y' || ch == 'Y');
}

static void	render_help_lines(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 3, 2, " 🧭 FINDER NAVIGATION");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 4, 4, "j / k / Arrows   : Up / Down");
	mvwprintw(win, 5, 4, "g / G / PgUp/PgDn: Top / Bottom / Page");
	mvwprintw(win, 6, 4, "l / Enter / h    : Open / Parent Dir");
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 8, 2, " ⚡ 42 CLUSTER ACTIONS");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 9, 4, "s / u            : Link / Unlink Goinfre");
	mvwprintw(win, 10, 4, "H / T            : Heal Station / Empty Trash");
	mvwprintw(win, 11, 4, "b / Z            : Bootstrap / Inject .zshrc");
	mvwprintw(win, 12, 4, "C / K            : Clean Presets / Nuke Junk");
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 14, 2, " 🛠️ SELECTION & TOOLS");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 15, 4, "Space / v / U    : Mark / Invert / Clear");
	mvwprintw(win, 16, 4, "d / x / e / t / !: Delete / Edit / Shell / Exec");
	mvwprintw(win, 17, 4, "p / P / E        : Quick Look / Goto / Report");
	mvwprintw(win, 18, 4, "A / a / o / r    : Mode / Hidden / Sort / Reload");
	wattron(win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win, 20, 2, " Press any key to return...");
	wattroff(win, COLOR_PAIR(2) | A_BOLD);
}

void	show_help_modal(void)
{
	WINDOW	*win;

	win = newwin(22, 70, (LINES - 22) / 2, (COLS - 70) / 2);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [  macOS FT_NCDU KEYBOARD REFERENCE ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	render_help_lines(win);
	wrefresh(win);
	wtimeout(win, -1);
	wgetch(win);
	delwin(win);
}
