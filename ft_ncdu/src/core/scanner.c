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

static void	fill_entry_stat(t_file_entry *e, struct stat *st)
{
	e->size = st->st_size;
	e->disk_size = st->st_blocks * 512;
	e->mtime = st->st_mtime;
	e->mode = st->st_mode;
	e->items_count = 0;
	if (S_ISDIR(st->st_mode))
		e->type = TYPE_DIR;
	else if (S_ISLNK(st->st_mode))
		e->type = TYPE_LINK;
	else
		e->type = TYPE_FILE;
}

static void	read_dir_entries(DIR *d, const char *dir_path)
{
	struct dirent	*de;
	struct stat		st;
	t_file_entry	*e;

	de = readdir(d);
	while (de && g_state.count < MAX_ENTRIES)
	{
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
		{
			e = &g_state.entries[g_state.count];
			memset(e, 0, sizeof(t_file_entry));
			safe_str_copy(e->name, de->d_name, NAME_MAX_LEN);
			snprintf(e->path, PATH_MAX_LEN, "%.2048s/%.256s",
				dir_path, de->d_name);
			if (lstat(e->path, &st) == 0)
			{
				fill_entry_stat(e, &st);
				check_symlink_health(e);
				g_state.count++;
			}
		}
		de = readdir(d);
	}
}

static void	dispatch_workers(void)
{
	pthread_t	threads[SCAN_THREADS];
	int			i;

	i = 0;
	while (i < SCAN_THREADS)
	{
		pthread_create(&threads[i], NULL, scan_thread_worker,
			(void *)(intptr_t)i);
		i++;
	}
	i = 0;
	while (i < SCAN_THREADS)
		pthread_join(threads[i++], NULL);
}

static void	*async_scan_orchestrator(void *arg)
{
	DIR		*d;
	char	*path;

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
	dispatch_workers();
	pthread_mutex_lock(&g_state.lock);
	qsort(g_state.entries, g_state.count,
		sizeof(t_file_entry), compare_entries);
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
	g_state.is_scanning = 0;
	free(path);
	return (NULL);
}

void	start_async_scan(const char *dir_path)
{
	pthread_t	tid;
	char		*path_copy;

	g_state.abort_scan = 1;
	while (g_state.is_scanning)
		usleep(1000);
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
	safe_str_copy(g_state.current_dir, dir_path, PATH_MAX_LEN);
	g_state.abort_scan = 0;
	g_state.is_scanning = 1;
	pthread_mutex_unlock(&g_state.lock);
	path_copy = strdup(dir_path);
	pthread_create(&tid, NULL, async_scan_orchestrator, (void *)path_copy);
	pthread_detach(tid);
}
