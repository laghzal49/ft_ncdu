/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	action_goto_path(void)
{
	WINDOW	*win;
	char	input_buffer[PATH_MAX_LEN];
	char	resolved_path[PATH_MAX_LEN];

	win = newwin(7, 70, (LINES - 7) / 2, (COLS - 70) / 2);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 1, 2, ":: [ TELEPORT / GOTO PATH ] ::");
	wattroff(win, COLOR_PAIR(1) | A_BOLD);
	mvwprintw(win, 3, 2, "Enter Path: ");
	echo();
	curs_set(1);
	wtimeout(win, -1);
	input_buffer[0] = '\0';
	wgetnstr(win, input_buffer, sizeof(input_buffer) - 1);
	noecho();
	curs_set(0);
	delwin(win);
	if (strlen(input_buffer) > 0 && realpath(input_buffer, resolved_path))
		start_async_scan(resolved_path);
}

static void	exec_custom_cmd(const char *cmd_text, const char *target_path)
{
	char	*escaped_path;
	char	*full_command;

	escaped_path = shell_escape(target_path);
	if (escaped_path && asprintf(&full_command, "%s %s", cmd_text,
			escaped_path) != -1)
	{
		def_prog_mode();
		endwin();
		printf("\n\033[1;36m[ft_ncdu]\033[0m Executing: %s\n", full_command);
		if (system(full_command))
			(void)0;
		free(full_command);
		printf("\nPress Enter to return...");
		getchar();
		reset_prog_mode();
		refresh();
	}
	free(escaped_path);
}

void	action_custom_command(void)
{
	WINDOW	*win;
	char	command_buffer[512];

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
	command_buffer[0] = '\0';
	wgetnstr(win, command_buffer, sizeof(command_buffer) - 1);
	noecho();
	curs_set(0);
	delwin(win);
	if (strlen(command_buffer) > 0)
		exec_custom_cmd(command_buffer,
			g_state.filtered[g_state.selected].path);
	start_async_scan(g_state.current_dir);
}

void	action_edit(void)
{
	const char	*editor_name;
	char		*escaped_path;
	char		*command_str;

	if (g_state.filtered_count == 0)
		return ;
	editor_name = getenv("EDITOR");
	if (!editor_name)
		editor_name = "nvim";
	escaped_path = shell_escape(g_state.filtered[g_state.selected].path);
	if (escaped_path && asprintf(&command_str, "%s %s", editor_name,
			escaped_path) != -1)
	{
		def_prog_mode();
		endwin();
		if (system(command_str))
			(void)0;
		free(command_str);
		reset_prog_mode();
		refresh();
	}
	free(escaped_path);
	start_async_scan(g_state.current_dir);
}

void	action_shell(void)
{
	const char	*shell_name;

	shell_name = getenv("SHELL");
	if (!shell_name)
		shell_name = "/bin/zsh";
	def_prog_mode();
	endwin();
	printf("\n\033[1;36m[ft_ncdu]\033[0m Subshell at %s\n",
		g_state.current_dir);
	if (chdir(g_state.current_dir) == 0 && system(shell_name))
		(void)0;
	reset_prog_mode();
	refresh();
	start_async_scan(g_state.current_dir);
}
