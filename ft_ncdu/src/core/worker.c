/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	check_symlink_health(t_file_entry *entry)
{
	ssize_t		len;
	struct stat	st;

	if (entry->type != TYPE_LINK)
		return ;
	len = readlink(entry->path, entry->symlink_target, PATH_MAX_LEN - 1);
	if (len != -1)
		entry->symlink_target[len] = '\0';
	else
		entry->symlink_target[0] = '\0';
	if (stat(entry->path, &st) == -1)
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

static void	calc_subitem(const char *path, const char *name,
		off_t *out_s, off_t *out_d)
{
	char		subpath[PATH_MAX_LEN];
	struct stat	st;

	snprintf(subpath, sizeof(subpath), "%.2048s/%.256s", path, name);
	if (lstat(subpath, &st) != 0)
		return ;
	if (g_state.root_dev != 0 && st.st_dev != g_state.root_dev)
		return ;
	if (S_ISDIR(st.st_mode))
		calculate_dir_recursive(subpath, out_s, out_d);
	else
	{
		*out_s += st.st_size;
		*out_d += st.st_blocks * 512;
	}
}

void	calculate_dir_recursive(const char *path, off_t *out_s, off_t *out_d)
{
	DIR				*dir;
	struct dirent	*entry;

	if (g_state.abort_scan)
		return ;
	dir = opendir(path);
	if (!dir)
	{
		pthread_mutex_lock(&g_state.lock);
		g_state.unreadable_count++;
		pthread_mutex_unlock(&g_state.lock);
		return ;
	}
	entry = readdir(dir);
	while (entry && !g_state.abort_scan)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			calc_subitem(path, entry->d_name, out_s, out_d);
		entry = readdir(dir);
	}
	closedir(dir);
}

static void	process_worker_entry(int idx)
{
	off_t	s;
	off_t	d;

	s = 0;
	d = 0;
	calculate_dir_recursive(g_state.entries[idx].path, &s, &d);
	pthread_mutex_lock(&g_state.lock);
	g_state.entries[idx].size = s;
	g_state.entries[idx].disk_size = d;
	g_state.total_dir_size += s;
	g_state.total_disk_usage += d;
	if (d > g_state.max_item_size)
		g_state.max_item_size = d;
	pthread_mutex_unlock(&g_state.lock);
}

void	*scan_thread_worker(void *arg)
{
	int	thread_id;
	int	idx;

	thread_id = (int)(intptr_t)arg;
	idx = thread_id;
	while (!g_state.abort_scan)
	{
		pthread_mutex_lock(&g_state.lock);
		if (idx >= g_state.count)
		{
			pthread_mutex_unlock(&g_state.lock);
			break ;
		}
		pthread_mutex_unlock(&g_state.lock);
		if (g_state.entries[idx].type == TYPE_DIR)
			process_worker_entry(idx);
		idx += SCAN_THREADS;
	}
	return (NULL);
}
