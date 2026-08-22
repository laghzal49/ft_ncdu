/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	action_goto_path(void)
{
	WINDOW	*win;
	char	buf[PATH_MAX_LEN];
	char	real_p[PATH_MAX_LEN];

	win = newwin(7, 70, (LINES - 7) / 2, (COLS - 70) / 2);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [ TELEPORT / GOTO PATH ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 3, 2, "Enter Path: ");
	echo();
	curs_set(1);
	wtimeout(win, -1);
	buf[0] = '\0';
	wgetnstr(win, buf, sizeof(buf) - 1);
	noecho();
	curs_set(0);
	delwin(win);
	if (strlen(buf) > 0 && realpath(buf, real_p))
		start_async_scan(real_p);
}

static void	exec_custom_cmd(const char *buf, const char *path)
{
	char	*esc;
	char	*cmd;

	esc = shell_escape(path);
	if (esc && asprintf(&cmd, "%s %s", buf, esc) != -1)
	{
		def_prog_mode();
		endwin();
		printf("\n\033[1;36m[ft_ncdu]\033[0m Executing: %s\n", cmd);
		if (system(cmd))
			(void)0;
		free(cmd);
		printf("\nPress Enter to return...");
		getchar();
		reset_prog_mode();
		refresh();
	}
	free(esc);
}

void	action_custom_command(void)
{
	WINDOW	*win;
	char	buf[512];

	if (g_state.filtered_count == 0)
		return ;
	win = newwin(7, 70, (LINES - 7) / 2, (COLS - 70) / 2);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [ EXEC COMMAND ON TARGET ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 3, 2, "Target: %.40s",
		g_state.filtered[g_state.selected].name);
	mvwprintw(win, 4, 2, "$ ");
	echo();
	curs_set(1);
	wtimeout(win, -1);
	buf[0] = '\0';
	wgetnstr(win, buf, sizeof(buf) - 1);
	noecho();
	curs_set(0);
	delwin(win);
	if (strlen(buf) > 0)
		exec_custom_cmd(buf, g_state.filtered[g_state.selected].path);
	start_async_scan(g_state.current_dir);
}

void	action_edit(void)
{
	const char	*editor;
	char		*esc;
	char		*cmd;

	if (g_state.filtered_count == 0)
		return ;
	editor = getenv("EDITOR");
	if (!editor)
		editor = "nvim";
	esc = shell_escape(g_state.filtered[g_state.selected].path);
	if (esc && asprintf(&cmd, "%s %s", editor, esc) != -1)
	{
		def_prog_mode();
		endwin();
		if (system(cmd))
			(void)0;
		free(cmd);
		reset_prog_mode();
		refresh();
	}
	free(esc);
	start_async_scan(g_state.current_dir);
}

void	action_shell(void)
{
	const char	*sh;

	sh = getenv("SHELL");
	if (!sh)
		sh = "/bin/zsh";
	def_prog_mode();
	endwin();
	printf("\n\033[1;36m[ft_ncdu]\033[0m Subshell at %s\n",
		g_state.current_dir);
	if (chdir(g_state.current_dir) == 0 && system(sh))
		(void)0;
	reset_prog_mode();
	refresh();
	start_async_scan(g_state.current_dir);
}
