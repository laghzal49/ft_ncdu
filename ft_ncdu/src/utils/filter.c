/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

int	compare_entries(const void *item_a, const void *item_b)
{
	const t_file_entry	*entry_a = (const t_file_entry *)item_a;
	const t_file_entry	*entry_b = (const t_file_entry *)item_b;
	off_t				size_a;
	off_t				size_b;

	if (g_state.size_mode == SIZE_ACTUAL_DISK)
	{
		size_a = entry_a->disk_size;
		size_b = entry_b->disk_size;
	}
	else
	{
		size_a = entry_a->size;
		size_b = entry_b->size;
	}
	if (g_state.sort_mode == SORT_SIZE_DESC)
		return ((size_b > size_a) - (size_b < size_a));
	if (g_state.sort_mode == SORT_SIZE_ASC)
		return ((size_a > size_b) - (size_a < size_b));
	if (g_state.sort_mode == SORT_NAME_ASC)
		return (strcasecmp(entry_a->name, entry_b->name));
	return ((entry_b->mtime > entry_a->mtime)
		- (entry_b->mtime < entry_a->mtime));
}

static int	entry_matches_filter(t_file_entry *entry)
{
	if (!g_state.show_hidden && entry->name[0] == '.')
		return (0);
	if (strlen(g_state.search_query) > 0)
	{
		if (!strcasestr(entry->name, g_state.search_query))
			return (0);
	}
	return (1);
}

void	apply_filter(void)
{
	int	idx;

	pthread_mutex_lock(&g_state.lock);
	g_state.filtered_count = 0;
	idx = 0;
	while (idx < g_state.count)
	{
		if (entry_matches_filter(&g_state.entries[idx]))
		{
			g_state.filtered[g_state.filtered_count] = g_state.entries[idx];
			g_state.filtered_count++;
		}
		idx++;
	}
	qsort(g_state.filtered, g_state.filtered_count, sizeof(t_file_entry),
		compare_entries);
	if (g_state.selected >= g_state.filtered_count)
		g_state.selected = g_state.filtered_count - 1;
	if (g_state.selected < 0)
		g_state.selected = 0;
	pthread_mutex_unlock(&g_state.lock);
}

int	count_marked_items(void)
{
	int	marked_count;
	int	idx;

	marked_count = 0;
	idx = 0;
	pthread_mutex_lock(&g_state.lock);
	while (idx < g_state.count)
	{
		if (g_state.entries[idx].marked)
			marked_count++;
		idx++;
	}
	pthread_mutex_unlock(&g_state.lock);
	return (marked_count);
}

void	action_batch_invert(void)
{
	int	idx;

	pthread_mutex_lock(&g_state.lock);
	idx = 0;
	while (idx < g_state.count)
	{
		g_state.entries[idx].marked = !g_state.entries[idx].marked;
		idx++;
	}
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
}
