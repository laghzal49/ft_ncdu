/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   modals.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

int	confirm_modal(const char *title, const char *message)
{
	WINDOW	*win;
	int		key_code;

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
	key_code = wgetch(win);
	delwin(win);
	return (key_code == 'y' || key_code == 'Y');
}

static void	render_page1_nav(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 2, 2, " 🧭 FINDER NAVIGATION & SEARCH (TAB 1/3)");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 4, 4, "j / k / Arrows   : Navigate Rows");
	mvwprintw(win, 5, 4, "g / G / Home/End : Jump to Top / Bottom");
	mvwprintw(win, 6, 4, "PgUp / PgDn      : Fast Viewport Scroll");
	mvwprintw(win, 7, 4, "l / Enter / h    : Open Folder / Parent");
	mvwprintw(win, 8, 4, "~ / P (or :)     : Jump HOME / Teleport");
	mvwprintw(win, 9, 4, "/                : Spotlight Search");
	mvwprintw(win, 10, 4, "o                : Cycle Sort Modes");
	mvwprintw(win, 11, 4, "A / a / r        : Actual Size / Dotfiles / Rescan");
}

static void	render_page2_cluster(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 2, 2, " ⚡ 42 CLUSTER & GOINFRE HEALER (TAB 2/3)");
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, 4, 4, "s                : Move to Goinfre & Link");
	mvwprintw(win, 5, 4, "u                : Restore to HOME");
	mvwprintw(win, 6, 4, "H                : Station Healer (Fix Symlinks)");
	mvwprintw(win, 7, 4, "b                : Bootstrap Tools to Goinfre");
	mvwprintw(win, 8, 4, "T / Z            : Empty Trash / Inject .zshrc");
	mvwprintw(win, 9, 4, "C / K            : 9 Clean Presets / Nuke Junk");
	mvwprintw(win, 10, 4, "E                : Export Markdown Quota Report");
}

static void	render_page_content(WINDOW *win, int page_idx)
{
	werase(win);
	box(win, 0, 0);
	if (page_idx == 0)
		render_page1_nav(win);
	else if (page_idx == 1)
		render_page2_cluster(win);
	else
	{
		wattron(win, COLOR_PAIR(3) | A_BOLD);
		mvwprintw(win, 2, 2, " 🛠️ TOOLS & CLI MODES (TAB 3/3)");
		wattroff(win, COLOR_PAIR(3) | A_BOLD);
		mvwprintw(win, 4, 4, "p                : Quick Look File Preview");
		mvwprintw(win, 5, 4, "e / t / !        : $EDITOR / Subshell / Exec");
		mvwprintw(win, 6, 4, "Space / v / U    : Mark / Invert / Clear All");
		mvwprintw(win, 7, 4, "d / x            : Safe Delete");
		mvwprintw(win, 9, 4, "CLI: ntcl13 / clean42 : Native 9-Tier Clean");
		mvwprintw(win, 10, 4, "CLI: ft_ncdu --heal   : Headless Healer");
		mvwprintw(win, 11, 4, "CLI: ft_ncdu --report : Print Quota Summary");
	}
	wattron(win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win, 13, 2, " [Tab / Arrows] Next Tab │ [ESC] Close");
	wattroff(win, COLOR_PAIR(2) | A_BOLD);
	wrefresh(win);
}

void	show_help_modal(void)
{
	WINDOW	*win;
	int		page_idx;
	int		key_code;

	win = newwin(16, 70, (LINES - 16) / 2, (COLS - 70) / 2);
	if (!win)
		return ;
	page_idx = 0;
	while (1)
	{
		render_page_content(win, page_idx);
		key_code = wgetch(win);
		if (key_code == 'q' || key_code == 27)
			break ;
		if (key_code == '\t' || key_code == KEY_RIGHT || key_code == ' '
			|| key_code == 'l')
			page_idx = (page_idx + 1) % 3;
		else if (key_code == KEY_LEFT || key_code == 'h')
			page_idx = (page_idx + 2) % 3;
	}
	delwin(win);
}
