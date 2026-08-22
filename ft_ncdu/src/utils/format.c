/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	format_size(off_t bytes, char *out, size_t out_len)
{
	const char	*units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
	int			unit_idx;
	double		sz;

	unit_idx = 0;
	sz = (double)bytes;
	while (sz >= 1024.0 && unit_idx < 5)
	{
		sz /= 1024.0;
		unit_idx++;
	}
	if (unit_idx == 0)
		snprintf(out, out_len, "%4lld   B", (long long)bytes);
	else
		snprintf(out, out_len, "%6.1f %s", sz, units[unit_idx]);
}

static void	fill_rwx(mode_t mode, char *out)
{
	const mode_t	flags[9] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP,
		S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
	const char		chars[9] = {'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x'};
	int				i;

	i = 0;
	while (i < 9)
	{
		out[i + 1] = '-';
		if (mode & flags[i])
			out[i + 1] = chars[i];
		i++;
	}
	out[10] = '\0';
}

void	format_permissions(mode_t mode, char *out)
{
	out[0] = '-';
	if (S_ISDIR(mode))
		out[0] = 'd';
	else if (S_ISLNK(mode))
		out[0] = 'l';
	fill_rwx(mode, out);
}

void	format_breadcrumbs(const char *path, char *out, size_t max_len)
{
	size_t	len;

	if (!path || !out || max_len == 0)
		return ;
	len = strlen(path);
	if (len <= max_len)
		safe_str_copy(out, path, max_len + 1);
	else
	{
		safe_str_copy(out, "...", max_len + 1);
		if (max_len > 3)
			strncat(out, path + (len - (max_len - 3)), max_len - 3);
	}
}
