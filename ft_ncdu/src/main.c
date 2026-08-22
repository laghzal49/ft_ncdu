/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

t_app_state	g_state;

static void	handle_signal(int sig)
{
	(void)sig;
	g_state.abort_scan = 1;
	endwin();
	free_state_memory();
	pthread_mutex_destroy(&g_state.lock);
	exit(0);
}

static int	parse_action_flags(int argc, char **argv)
{
	if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--clean") == 0)
		return (run_cli_clean() + 1);
	if (strcmp(argv[1], "--heal") == 0)
		return (run_cli_heal() + 1);
	if (strcmp(argv[1], "--bootstrap") == 0)
		return (run_cli_bootstrap() + 1);
	if (strcmp(argv[1], "--report") == 0)
	{
		if (argc > 2)
			return (run_cli_report(argv[2]) + 1);
		return (run_cli_report(".") + 1);
	}
	return (0);
}

static int	parse_cli_flags(int argc, char **argv)
{
	if (strstr(argv[0], "ntcl13") != NULL
		|| strstr(argv[0], "clean42") != NULL)
		return (run_cli_clean() + 1);
	if (argc <= 1)
		return (0);
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		print_cli_help(argv[0]);
		return (1);
	}
	if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
	{
		print_cli_version();
		return (1);
	}
	return (parse_action_flags(argc, argv));
}

static void	init_runtime(void)
{
	struct sigaction	sa;

	setlocale(LC_ALL, "");
	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	pthread_mutex_init(&g_state.lock, NULL);
	init_state_memory();
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	timeout(TICK_DELAY_MS);
	init_ui_colors();
}

int	main(int argc, char **argv)
{
	char		start_path[PATH_MAX_LEN];
	const char	*home;

	if (parse_cli_flags(argc, argv) > 0)
		return (0);
	init_runtime();
	home = getenv("HOME");
	if (!home)
		home = ".";
	safe_str_copy(start_path, home, PATH_MAX_LEN);
	if (argc > 1 && argv[1][0] != '-')
		realpath(argv[1], start_path);
	start_async_scan(start_path);
	run_event_loop();
	endwin();
	free_state_memory();
	pthread_mutex_destroy(&g_state.lock);
	return (0);
}
