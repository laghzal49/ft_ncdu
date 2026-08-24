/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scanner.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	*scan_thread_worker(void *arg);

static void	init_single_entry(t_file_entry *e, struct dirent *de,
		const char *dir_path)
{
	struct stat	st;

	memset(e, 0, sizeof(t_file_entry));
	safe_str_copy(e->name, de->d_name, NAME_MAX_LEN);
	snprintf(e->path, PATH_MAX_LEN, "%.2048s/%.256s", dir_path, de->d_name);
	if (lstat(e->path, &st) == 0)
	{
		e->size = st.st_size;
		e->disk_size = st.st_blocks * 512;
		e->mtime = st.st_mtime;
		e->mode = st.st_mode;
		if (S_ISDIR(st.st_mode))
			e->type = TYPE_DIR;
		else if (S_ISLNK(st.st_mode))
			e->type = TYPE_LINK;
		check_symlink_health(e);
		g_state.count++;
	}
}

static void	read_dir_entries(DIR *d, const char *dir_path)
{
	struct dirent	*de;

	de = readdir(d);
	while (de && g_state.count < MAX_ENTRIES)
	{
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
			init_single_entry(&g_state.entries[g_state.count], de, dir_path);
		de = readdir(d);
	}
}

static void	*async_scan_orchestrator(void *arg)
{
	DIR			*d;
	char		*path;
	pthread_t	th[SCAN_THREADS];
	int			i;

	path = (char *)arg;
	d = opendir(path);
	if (!d)
	{
		g_state.is_scanning = 0;
		free(path);
		return (NULL);
	}
	read_dir_entries(d, path);
	closedir(d);
	apply_filter();
	i = -1;
	while (++i < SCAN_THREADS)
		pthread_create(&th[i], NULL, scan_thread_worker, (void *)(intptr_t)i);
	while (--i >= 0)
		pthread_join(th[i], NULL);
	apply_filter();
	g_state.is_scanning = 0;
	free(path);
	return (NULL);
}

static void	reset_scan_counters(const char *dir_path)
{
	struct stat	st;

	pthread_mutex_lock(&g_state.lock);
	g_state.count = 0;
	g_state.filtered_count = 0;
	g_state.selected = 0;
	g_state.scroll_offset = 0;
	g_state.total_dir_size = 0;
	g_state.total_disk_usage = 0;
	g_state.max_item_size = 0;
	g_state.unreadable_count = 0;
	g_state.broken_links_count = 0;
	g_state.root_dev = 0;
	if (stat(dir_path, &st) == 0)
		g_state.root_dev = st.st_dev;
	safe_str_copy(g_state.current_dir, dir_path, PATH_MAX_LEN);
	g_state.abort_scan = 0;
	g_state.is_scanning = 1;
	pthread_mutex_unlock(&g_state.lock);
}

void	start_async_scan(const char *dir_path)
{
	pthread_t	tid;
	char		*path_copy;

	g_state.abort_scan = 1;
	while (g_state.is_scanning)
		usleep(1000);
	reset_scan_counters(dir_path);
	path_copy = strdup(dir_path);
	pthread_create(&tid, NULL, async_scan_orchestrator, (void *)path_copy);
	pthread_detach(tid);
}
