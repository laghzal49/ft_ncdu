/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   security.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/27 10:00:00 by tlaghzal         ###   ########.fr       */
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

void	log_audit_action(const char *action, const char *target,
		const char *details)
{
	const char	*home_dir;
	char		log_path[PATH_MAX_LEN];
	FILE		*fp;
	time_t		now;
	struct tm	*t;

	home_dir = getenv("HOME");
	if (!home_dir)
		return ;
	snprintf(log_path, sizeof(log_path), "%.2048s/.ft_ncdu_cleanup.log",
		home_dir);
	fp = fopen(log_path, "a");
	if (!fp)
		return ;
	now = time(NULL);
	t = localtime(&now);
	if (t && details)
		fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s (%s)\n",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec, action, target, details);
	else if (t)
		fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec, action, target);
	fclose(fp);
}
