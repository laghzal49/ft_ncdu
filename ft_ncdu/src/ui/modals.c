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

static void	render_help_part1(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 3, 2, " 🧭 FINDER NAVIGATION");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 4, 4, "j / k / Arrows   : Navigate Rows (Up / Down)");
	mvwprintw(win, 5, 4, "g / G / Home/End : Jump to Top / Bottom");
	mvwprintw(win, 6, 4, "PgUp / PgDn      : Fast Viewport Scroll");
	mvwprintw(win, 7, 4, "l / Enter / h    : Open Folder / Parent Dir");
	mvwprintw(win, 8, 4, "~ / P (or :)     : Jump to HOME / Teleport Path");
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 10, 2, " ⚡ 42 / 1337 CLUSTER STORAGE & HEALER");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 11, 4, "s                : Move to Goinfre & Link");
	mvwprintw(win, 12, 4, "u                : Restore from Goinfre to Home");
	mvwprintw(win, 13, 4, "H                : Station Healer (Fix Symlinks)");
	mvwprintw(win, 14, 4, "b                : Auto-Bootstrap Toolchains");
	mvwprintw(win, 15, 4, "T                : Purge Desktop Trash (~/.Trash)");
	mvwprintw(win, 16, 4, "Z                : Inject Quota Bypass in ~/.zshrc");
}

static void	render_help_part2(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 18, 2, " 🧹 CLEANING PRESETS & TOOLS");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 19, 4, "C / K            : 9 Clean Presets / Quick Nuke");
	mvwprintw(win, 20, 4, "p                : Quick Look (File Preview)");
	mvwprintw(win, 21, 4, "e / t / !        : Open $EDITOR / Shell / Exec");
	mvwprintw(win, 22, 4, "/                : Spotlight Real-Time Search");
	mvwprintw(win, 23, 4, "Space / v / U    : Mark / Invert / Clear Marks");
	mvwprintw(win, 24, 4, "d / x            : Safe Delete (Single / Batch)");
	mvwprintw(win, 25, 4, "A / a / o / r    : Size / Hidden / Sort / Reload");
	mvwprintw(win, 26, 4, "E                : Export Markdown Audit Report");
	wattron(win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win, 28, 2, " [Press any key to close this showcase]");
	wattroff(win, COLOR_PAIR(2) | A_BOLD);
}

void	show_help_modal(void)
{
	WINDOW	*win;

	win = newwin(30, 74, (LINES - 30) / 2, (COLS - 74) / 2);
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
