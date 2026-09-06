/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   presets.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

static void	render_preset_list(WINDOW *win)
{
	size_t	idx;
	int		height;
	int		width;

	getmaxyx(win, height, width);
	idx = 0;
	while (idx < PRESET_COUNT && 3 + (int)idx < height - 2)
	{
		mvwprintw(win, 3 + (int)idx, 2, "[%c] %-22.22s : %.*s",
			g_clean_presets[idx].key,
			g_clean_presets[idx].title,
			width - 32, g_clean_presets[idx].desc);
		idx++;
	}
	wattron(win, COLOR_PAIR(3) | A_BOLD);
	mvwprintw(win, height - 2, 2,
		"Select preset [1-%d] or [ESC]: ", (int)PRESET_COUNT);
	wattroff(win, COLOR_PAIR(3) | A_BOLD);
}

static void	exec_selected_preset(int key_pressed)
{
	size_t	idx;
	char	*command_str;

	idx = 0;
	while (idx < PRESET_COUNT)
	{
		if (key_pressed == g_clean_presets[idx].key
			&& asprintf(&command_str, g_clean_presets[idx].command_fmt,
				g_state.current_dir) != -1)
		{
			if (system(command_str))
				(void)0;
			free(command_str);
			break ;
		}
		idx++;
	}
}

void	action_cleaning_presets(void)
{
	WINDOW	*win;
	int		key_pressed;
	int		width;
	int		height;

	width = COLS - 2;
	if (width > 76)
		width = 76;
	height = LINES - 2;
	if (height > (int)PRESET_COUNT + 8)
		height = (int)PRESET_COUNT + 8;
	win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
	if (!win)
		return ;
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [ macOS 42 CLEANING PRESETS ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	render_preset_list(win);
	wrefresh(win);
	wtimeout(win, -1);
	key_pressed = wgetch(win);
	delwin(win);
	exec_selected_preset(key_pressed);
	start_async_scan(g_state.current_dir);
}

void	action_nuke_junk(void)
{
	char	*esc_dir;
	char	*command_str;

	if (!confirm_modal("NUKE JUNK", "Wipe Python caches & build artifacts?"))
		return ;
	esc_dir = shell_escape(g_state.current_dir);
	if (esc_dir)
	{
		command_str = NULL;
		if (asprintf(&command_str, "find %s -type d \\( -name \"node_modules\" "
				"-o -name \".cache\" -o -name \"*.dSYM\" -o -name "
				"\"__pycache__\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
				"find %s -type f \\( -name \"*.o\" -o -name \"*.a\" -o "
				"-name \"core.*\" \\) -delete 2>/dev/null",
				esc_dir, esc_dir) != -1)
		{
			if (system(command_str))
				(void)0;
			free(command_str);
		}
		free(esc_dir);
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
