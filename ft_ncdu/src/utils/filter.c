/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

int	compare_entries(const void *a, const void *b)
{
	const t_file_entry	*ea = (const t_file_entry *)a;
	const t_file_entry	*eb = (const t_file_entry *)b;
	off_t				sa;
	off_t				sb;

	if (g_state.size_mode == SIZE_ACTUAL_DISK)
	{
		sa = ea->disk_size;
		sb = eb->disk_size;
	}
	else
	{
		sa = ea->size;
		sb = eb->size;
	}
	if (g_state.sort_mode == SORT_SIZE_DESC)
		return ((sb > sa) - (sb < sa));
	if (g_state.sort_mode == SORT_SIZE_ASC)
		return ((sa > sb) - (sa < sb));
	if (g_state.sort_mode == SORT_NAME_ASC)
		return (strcasecmp(ea->name, eb->name));
	return ((eb->mtime > ea->mtime) - (eb->mtime < ea->mtime));
}

static int	entry_matches_filter(t_file_entry *e)
{
	if (!g_state.show_hidden && e->name[0] == '.')
		return (0);
	if (strlen(g_state.search_query) > 0)
	{
		if (!strcasestr(e->name, g_state.search_query))
			return (0);
	}
	return (1);
}

void	apply_filter(void)
{
	int	i;

	pthread_mutex_lock(&g_state.lock);
	g_state.filtered_count = 0;
	i = 0;
	while (i < g_state.count)
	{
		if (entry_matches_filter(&g_state.entries[i]))
		{
			g_state.filtered[g_state.filtered_count] = g_state.entries[i];
			g_state.filtered_count++;
		}
		i++;
	}
	if (g_state.selected >= g_state.filtered_count)
		g_state.selected = g_state.filtered_count - 1;
	if (g_state.selected < 0)
		g_state.selected = 0;
	pthread_mutex_unlock(&g_state.lock);
}

int	count_marked_items(void)
{
	int	marked;
	int	i;

	marked = 0;
	i = 0;
	pthread_mutex_lock(&g_state.lock);
	while (i < g_state.count)
	{
		if (g_state.entries[i].marked)
			marked++;
		i++;
	}
	pthread_mutex_unlock(&g_state.lock);
	return (marked);
}

void	action_batch_invert(void)
{
	int	i;

	pthread_mutex_lock(&g_state.lock);
	i = 0;
	while (i < g_state.count)
	{
		g_state.entries[i].marked = !g_state.entries[i].marked;
		i++;
	}
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
}
