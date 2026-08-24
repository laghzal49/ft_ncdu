/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	safe_str_copy(char *dest, const char *src, size_t dest_len)
{
	size_t	i;

	if (!dest || dest_len == 0)
		return ;
	if (!src)
	{
		dest[0] = '\0';
		return ;
	}
	i = 0;
	while (i + 1 < dest_len && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

void	init_state_memory(void)
{
	g_state.entries = malloc(MAX_ENTRIES * sizeof(t_file_entry));
	g_state.filtered = malloc(MAX_ENTRIES * sizeof(t_file_entry));
	if (!g_state.entries || !g_state.filtered)
	{
		fprintf(stderr, "Error: Failed to allocate scanner memory pools.\n");
		if (g_state.entries)
			free(g_state.entries);
		if (g_state.filtered)
			free(g_state.filtered);
		exit(1);
	}
	memset(g_state.entries, 0, MAX_ENTRIES * sizeof(t_file_entry));
	memset(g_state.filtered, 0, MAX_ENTRIES * sizeof(t_file_entry));
}

void	free_state_memory(void)
{
	pthread_mutex_lock(&g_state.lock);
	if (g_state.entries)
	{
		free(g_state.entries);
		g_state.entries = NULL;
	}
	if (g_state.filtered)
	{
		free(g_state.filtered);
		g_state.filtered = NULL;
	}
	pthread_mutex_unlock(&g_state.lock);
}
