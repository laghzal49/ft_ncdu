/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   security.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "presets.h"

static void	fill_escaped(char *dst, const char *src, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	dst[j++] = '\'';
	while (i < len)
	{
		if (src[i] == '\'')
		{
			dst[j++] = '\'';
			dst[j++] = '\\';
			dst[j++] = '\'';
			dst[j++] = '\'';
		}
		else
			dst[j++] = src[i];
		i++;
	}
	dst[j++] = '\'';
	dst[j] = '\0';
}

char	*shell_escape(const char *str)
{
	size_t	len;
	char	*escaped;

	if (!str)
		return (NULL);
	len = strlen(str);
	escaped = malloc(len * 4 + 3);
	if (!escaped)
		return (NULL);
	fill_escaped(escaped, str, len);
	return (escaped);
}

int	is_protected_target(const char *path)
{
	const char	*base;
	size_t		i;

	if (!path)
		return (1);
	base = strrchr(path, '/');
	if (base)
		base++;
	else
		base = path;
	i = 0;
	while (g_protected_paths[i] != NULL)
	{
		if (strcmp(base, g_protected_paths[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}
