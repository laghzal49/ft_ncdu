/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scanner.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	*scan_thread_worker(void *arg);

static void	populate_file_entry(t_file_entry *entry, struct dirent *item,
		const char *parent_dir)
{
	struct stat	stat_info;

	memset(entry, 0, sizeof(t_file_entry));
	safe_str_copy(entry->name, item->d_name, NAME_MAX_LEN);
	snprintf(entry->path, PATH_MAX_LEN, "%.2048s/%.256s", parent_dir,
		item->d_name);
	if (lstat(entry->path, &stat_info) == 0)
	{
		entry->size = stat_info.st_size;
		entry->disk_size = stat_info.st_blocks * 512;
		entry->mtime = stat_info.st_mtime;
		entry->mode = stat_info.st_mode;
		if (S_ISDIR(stat_info.st_mode))
			entry->type = TYPE_DIR;
		else if (S_ISLNK(stat_info.st_mode))
			entry->type = TYPE_LINK;
		check_symlink_health(entry);
		g_state.count++;
	}
}

static void	read_directory_children(DIR *dir_handle, const char *dir_path)
{
	struct dirent	*item;

	item = readdir(dir_handle);
	while (item && g_state.count < MAX_ENTRIES)
	{
		if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0)
			populate_file_entry(&g_state.entries[g_state.count], item,
				dir_path);
		item = readdir(dir_handle);
	}
}

static void	run_worker_threads(void)
{
	pthread_t	threads[SCAN_THREADS];
	int			thread_idx;

	thread_idx = -1;
	while (++thread_idx < SCAN_THREADS)
		pthread_create(&threads[thread_idx], NULL, scan_thread_worker,
			(void *)(intptr_t)thread_idx);
	while (--thread_idx >= 0)
		pthread_join(threads[thread_idx], NULL);
}

static void	*async_scan_orchestrator(void *arg)
{
	DIR		*dir_handle;
	char	*target_path;

	target_path = (char *)arg;
	dir_handle = opendir(target_path);
	if (dir_handle)
	{
		read_directory_children(dir_handle, target_path);
		closedir(dir_handle);
		apply_filter();
		run_worker_threads();
		apply_filter();
	}
	g_state.is_scanning = 0;
	free(target_path);
	return (NULL);
}

void	start_async_scan(const char *dir_path)
{
	pthread_t	thread_id;
	struct stat	stat_info;

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
	g_state.root_dev = 0;
	if (stat(dir_path, &stat_info) == 0)
		g_state.root_dev = stat_info.st_dev;
	safe_str_copy(g_state.current_dir, dir_path, PATH_MAX_LEN);
	g_state.abort_scan = 0;
	g_state.is_scanning = 1;
	pthread_mutex_unlock(&g_state.lock);
	pthread_create(&thread_id, NULL, async_scan_orchestrator, strdup(dir_path));
	pthread_detach(thread_id);
}
