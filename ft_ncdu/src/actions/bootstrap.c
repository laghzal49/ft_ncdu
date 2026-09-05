/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bootstrap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 00:00:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/09/06 00:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

static int	make_parent_dirs(const char *target, size_t secure_length)
{
	char	path[PATH_MAX_LEN];
	char	*cursor;
	struct stat	info;

	safe_str_copy(path, target, sizeof(path));
	cursor = strrchr(path, '/');
	if (!cursor)
		return (0);
	*cursor = '\0';
	cursor = path + 1;
	while (*cursor)
	{
		if (*cursor == '/')
		{
			*cursor = '\0';
			if (mkdir(path, 0700) != 0 && errno != EEXIST)
				return (1);
			if (strlen(path) >= secure_length
				&& (lstat(path, &info) != 0 || !S_ISDIR(info.st_mode)
					|| S_ISLNK(info.st_mode) || info.st_uid != getuid()
					|| (info.st_mode & 0022) != 0))
				return (1);
			*cursor = '/';
		}
		cursor++;
	}
	if (mkdir(path, 0700) != 0 && errno != EEXIST)
		return (1);
	if (strlen(path) >= secure_length
		&& (lstat(path, &info) != 0 || !S_ISDIR(info.st_mode)
			|| S_ISLNK(info.st_mode) || info.st_uid != getuid()
			|| (info.st_mode & 0022) != 0))
		return (1);
	return (0);
}

static int	move_without_shell(const char *source, const char *destination)
{
	pid_t	child;
	int		status;

	child = fork();
	if (child == 0)
	{
		execlp("mv", "mv", source, destination, (char *) NULL);
		_exit(127);
	}
	if (child < 0 || waitpid(child, &status, 0) < 0)
		return (1);
	return (!WIFEXITED(status) || WEXITSTATUS(status) != 0);
}

static int	relocate_target(const char *source, const char *destination,
		const char *base)
{
	t_target_state	state;

	state.has_source = (lstat(source, &state.info) == 0);
	if (state.has_source && S_ISLNK(state.info.st_mode))
	{
		state.length = readlink(source, state.link, sizeof(state.link) - 1);
		if (state.length < 0)
			return (1);
		state.link[state.length] = '\0';
		return (strcmp(state.link, destination) != 0);
	}
	state.has_destination = (lstat(destination, &state.info) == 0);
	if (state.has_source && state.has_destination)
		return (1);
	if (make_parent_dirs(source, (size_t)-1)
		|| make_parent_dirs(destination, strlen(base)))
		return (1);
	if (state.has_source && move_without_shell(source, destination))
		return (1);
	if (!state.has_source && !state.has_destination
		&& mkdir(destination, 0700) != 0)
		return (1);
	if (symlink(destination, source) == 0)
		return (0);
	if (state.has_source)
		move_without_shell(destination, source);
	else if (!state.has_destination)
		rmdir(destination);
	return (1);
}

static int	process_target(const char *home, const char *base,
		const char *target, int verbose)
{
	t_bootstrap_paths	paths;

	snprintf(paths.source, sizeof(paths.source), "%.2048s/%.1024s", home,
		target);
	snprintf(paths.destination, sizeof(paths.destination), "%.2048s/%.1024s",
		base, target);
	if (!relocate_target(paths.source, paths.destination, base))
	{
		if (verbose)
			printf("  [ready] %s\n", target);
		return (0);
	}
	if (verbose)
		fprintf(stderr, "  [skip] %s (conflict or move failed)\n", target);
	return (1);
}

int	relocate_bootstrap_targets(int verbose)
{
	const char	*home;
	char		base[PATH_MAX_LEN];
	t_target_state	state;
	size_t		i;
	int			failed;

	home = getenv("HOME");
	if (!home)
		return (1);
	get_goinfre_path(base, sizeof(base));
	if (make_parent_dirs(base, (size_t)-1)
		|| (mkdir(base, 0700) != 0 && errno != EEXIST)
		|| lstat(base, &state.info) != 0
		|| !S_ISDIR(state.info.st_mode) || S_ISLNK(state.info.st_mode)
		|| state.info.st_uid != getuid() || (state.info.st_mode & 0022) != 0)
		return (1);
	i = 0;
	failed = 0;
	while (g_bootstrap_targets[i] != NULL)
	{
		if (process_target(home, base, g_bootstrap_targets[i], verbose))
			failed = 1;
		i++;
	}
	return (failed);
}
