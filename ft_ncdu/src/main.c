/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/27 10:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

t_app_state	g_state;
volatile sig_atomic_t	g_exit_requested;

static void	handle_signal(int sig)
{
	(void)sig;
	g_exit_requested = 1;
}

static int	parse_action_flags(int argc, char **argv)
{
	int	is_dry_run;

	is_dry_run = 0;
	if (argc > 2 && (strcmp(argv[2], "--dry-run") == 0
			|| strcmp(argv[2], "-n") == 0))
		is_dry_run = 1;
	if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--clean") == 0)
		return (run_cli_clean(is_dry_run) + 1);
	if (strcmp(argv[1], "--dry-run") == 0 || strcmp(argv[1], "-n") == 0)
		return (run_cli_clean(1) + 1);
	if (strcmp(argv[1], "--heal") == 0)
		return (run_cli_heal() + 1);
	if (strcmp(argv[1], "--bootstrap") == 0)
		return (run_cli_bootstrap() + 1);
	if (strcmp(argv[1], "--report") == 0)
	{
		if (argc > 2 && argv[2][0] != '-')
			return (run_cli_report(argv[2]) + 1);
		return (run_cli_report(".") + 1);
	}
	return (0);
}

static int	parse_cli_flags(int argc, char **argv)
{
	int	is_dry_run;
	int	action_status;

	is_dry_run = 0;
	if (argc > 1 && (strcmp(argv[1], "--dry-run") == 0
			|| strcmp(argv[1], "-n") == 0))
		is_dry_run = 1;
	if (argc > 1 && (strcmp(argv[1], "-h") == 0
			|| strcmp(argv[1], "--help") == 0))
	{
		print_cli_help(argv[0]);
		return (1);
	}
	if (argc > 1 && (strcmp(argv[1], "-v") == 0
			|| strcmp(argv[1], "--version") == 0))
	{
		print_cli_version();
		return (1);
	}
	action_status = 0;
	if (argc > 1)
		action_status = parse_action_flags(argc, argv);
	if (action_status)
		return (action_status);
	if (strstr(argv[0], "ntcl13") || strstr(argv[0], "clean42"))
		return (run_cli_clean(is_dry_run) + 1);
	return (0);
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
	int			cli_status;
	int			path_error;

	safe_str_copy(g_state.username, getenv("USER")
		? getenv("USER") : "unknown", sizeof(g_state.username));
	cli_status = parse_cli_flags(argc, argv);
	if (cli_status > 0)
		return (cli_status - 1);
	if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
	{
		fprintf(stderr, "%s: interactive mode requires a terminal\n", APP_NAME);
		return (1);
	}
	if (!getenv("TERM") || strcmp(getenv("TERM"), "dumb") == 0)
	{
		fprintf(stderr, "%s: set TERM to a supported terminal type\n", APP_NAME);
		return (1);
	}
	init_runtime();
	home = getenv("HOME");
	if (!home)
		home = ".";
	safe_str_copy(start_path, home, PATH_MAX_LEN);
	if (argc > 1 && argv[1][0] != '-'
		&& realpath(argv[1], start_path) == NULL)
	{
		path_error = errno;
		endwin();
		fprintf(stderr, "%s: cannot open '%s': %s\n", APP_NAME, argv[1],
			strerror(path_error));
		free_state_memory();
		pthread_mutex_destroy(&g_state.lock);
		return (1);
	}
	start_async_scan(start_path);
	run_event_loop();
	g_state.abort_scan = 1;
	while (g_state.is_scanning)
		usleep(1000);
	endwin();
	free_state_memory();
	pthread_mutex_destroy(&g_state.lock);
	return (0);
}
