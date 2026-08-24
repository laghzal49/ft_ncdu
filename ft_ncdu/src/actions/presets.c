/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   presets.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

static void	render_preset_list(WINDOW *win)
{
	size_t	i;

	i = 0;
	while (i < PRESET_COUNT)
	{
		mvwprintw(win, 3 + (int)i, 2, "[%c] %-22s : %.44s",
			g_clean_presets[i].key,
			g_clean_presets[i].title,
			g_clean_presets[i].desc);
		i++;
	}
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, (int)PRESET_COUNT + 5, 2,
		"Select preset [1-%d] or [ESC]: ", (int)PRESET_COUNT);
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
}

static void	exec_selected_preset(int ch)
{
	size_t	i;
	char	*cmd;

	i = 0;
	while (i < PRESET_COUNT)
	{
		if (ch == g_clean_presets[i].key && asprintf(&cmd,
				g_clean_presets[i].command_fmt, g_state.current_dir) != -1)
		{
			if (system(cmd))
				(void)0;
			free(cmd);
			break ;
		}
		i++;
	}
}

void	action_cleaning_presets(void)
{
	WINDOW	*win;
	int		ch;

	win = newwin((int)PRESET_COUNT + 8, 76,
			(LINES - (int)PRESET_COUNT - 8) / 2, (COLS - 76) / 2);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [  macOS 42 CLEANING PRESETS ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	render_preset_list(win);
	wrefresh(win);
	wtimeout(win, -1);
	ch = wgetch(win);
	delwin(win);
	exec_selected_preset(ch);
	start_async_scan(g_state.current_dir);
}

void	action_nuke_junk(void)
{
	char	*esc;
	char	*cmd;

	if (!confirm_modal("NUKE JUNK", "Wipe Python caches & build artifacts?"))
		return ;
	esc = shell_escape(g_state.current_dir);
	if (esc)
	{
		cmd = NULL;
		if (asprintf(&cmd, "find %s -type d \\( -name \"node_modules\" -o "
				"-name \".cache\" -o -name \"*.dSYM\" -o -name \"__pycache__\" "
				"\\) -prune -exec rm -rf {} + 2>/dev/null; "
				"find %s -type f \\( -name \"*.o\" -o -name \"*.a\" -o "
				"-name \"core.*\" \\) -delete 2>/dev/null", esc, esc) != -1)
		{
			if (system(cmd))
				(void)0;
			free(cmd);
		}
		free(esc);
	}
	start_async_scan(g_state.current_dir);
}

void	action_docker_prune(void)
{
	if (!confirm_modal("DOCKER PRUNE", "Run docker system prune -a --volumes?"))
		return ;
	if (system("docker system prune -a --volumes -f >/dev/null 2>&1"))
		(void)0;
	start_async_scan(g_state.current_dir);
}
