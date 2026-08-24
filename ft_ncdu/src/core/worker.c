/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	check_symlink_health(t_file_entry *entry)
{
	ssize_t		target_len;
	struct stat	stat_info;

	if (entry->type != TYPE_LINK)
		return ;
	target_len = readlink(entry->path, entry->symlink_target,
			PATH_MAX_LEN - 1);
	if (target_len != -1)
		entry->symlink_target[target_len] = '\0';
	else
		entry->symlink_target[0] = '\0';
	if (stat(entry->path, &stat_info) == -1)
	{
		entry->is_broken_link = 1;
		pthread_mutex_lock(&g_state.lock);
		g_state.broken_links_count++;
		pthread_mutex_unlock(&g_state.lock);
	}
	else
		entry->is_broken_link = 0;
	if (strstr(entry->symlink_target, "/goinfre/") != NULL
		|| strstr(entry->symlink_target, "/sgoinfre/") != NULL)
		entry->is_goinfre_link = 1;
}

static void	calc_subitem(const char *parent_path, const char *file_name,
		off_t *apparent_size, off_t *disk_blocks)
{
	char		full_subpath[PATH_MAX_LEN];
	struct stat	stat_info;

	snprintf(full_subpath, sizeof(full_subpath), "%.2048s/%.256s",
		parent_path, file_name);
	if (lstat(full_subpath, &stat_info) != 0)
		return ;
	if (g_state.root_dev != 0 && stat_info.st_dev != g_state.root_dev)
		return ;
	if (S_ISDIR(stat_info.st_mode))
		calculate_dir_recursive(full_subpath, apparent_size, disk_blocks);
	else
	{
		*apparent_size += stat_info.st_size;
		*disk_blocks += stat_info.st_blocks * 512;
	}
}

void	calculate_dir_recursive(const char *path, off_t *out_s, off_t *out_d)
{
	DIR				*dir_handle;
	struct dirent	*item;

	if (g_state.abort_scan)
		return ;
	dir_handle = opendir(path);
	if (!dir_handle)
	{
		pthread_mutex_lock(&g_state.lock);
		g_state.unreadable_count++;
		pthread_mutex_unlock(&g_state.lock);
		return ;
	}
	item = readdir(dir_handle);
	while (item && !g_state.abort_scan)
	{
		if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0)
			calc_subitem(path, item->d_name, out_s, out_d);
		item = readdir(dir_handle);
	}
	closedir(dir_handle);
}

static void	process_worker_entry(int entry_idx)
{
	off_t	apparent_bytes;
	off_t	allocated_blocks;

	apparent_bytes = 0;
	allocated_blocks = 0;
	calculate_dir_recursive(g_state.entries[entry_idx].path,
		&apparent_bytes, &allocated_blocks);
	pthread_mutex_lock(&g_state.lock);
	g_state.entries[entry_idx].size = apparent_bytes;
	g_state.entries[entry_idx].disk_size = allocated_blocks;
	g_state.total_dir_size += apparent_bytes;
	g_state.total_disk_usage += allocated_blocks;
	if (allocated_blocks > g_state.max_item_size)
		g_state.max_item_size = allocated_blocks;
	pthread_mutex_unlock(&g_state.lock);
}

void	*scan_thread_worker(void *arg)
{
	int	thread_id;
	int	entry_idx;

	thread_id = (int)(intptr_t)arg;
	entry_idx = thread_id;
	while (!g_state.abort_scan)
	{
		pthread_mutex_lock(&g_state.lock);
		if (entry_idx >= g_state.count)
		{
			pthread_mutex_unlock(&g_state.lock);
			break ;
		}
		pthread_mutex_unlock(&g_state.lock);
		if (g_state.entries[entry_idx].type == TYPE_DIR)
			process_worker_entry(entry_idx);
		entry_idx += SCAN_THREADS;
	}
	return (NULL);
}
