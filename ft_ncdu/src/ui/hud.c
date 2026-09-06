/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	render_gauge(char *buffer, double percentage, int width)
{
	int	filled_blocks;
	int	i;

	if (width < 2)
		width = 2;
	if (width > 16)
		width = 16;
	filled_blocks = (int)((percentage / 100.0) * width);
	if (filled_blocks > width)
		filled_blocks = width;
	if (filled_blocks < 0)
		filled_blocks = 0;
	buffer[0] = '[';
	i = 0;
	while (i < filled_blocks)
		buffer[1 + i++] = '#';
	while (i < width)
		buffer[1 + i++] = '.';
	buffer[1 + width] = ']';
	buffer[2 + width] = ' ';
	snprintf(buffer + 3 + width, 16, "%5.1f%%", percentage);
}

void	render_graph_bar(char *buffer, off_t item_size, off_t max_size,
		int bar_width)
{
	int		filled_blocks;
	int		i;
	double	ratio;

	if (bar_width < 2)
		bar_width = 2;
	if (bar_width > 16)
		bar_width = 16;
	ratio = 0.0;
	if (max_size > 0)
		ratio = (double)item_size / (double)max_size;
	filled_blocks = (int)(ratio * bar_width);
	if (filled_blocks > bar_width)
		filled_blocks = bar_width;
	buffer[0] = '[';
	i = 0;
	while (i < filled_blocks)
		buffer[1 + i++] = '#';
	while (i < bar_width)
		buffer[1 + i++] = '.';
	buffer[1 + bar_width] = ']';
	buffer[2 + bar_width] = '\0';
}

void	render_top_hud(int max_x)
{
	struct statvfs	fs_stats;
	char			used[16];
	char			free_space[16];
	char			gauge[64];
	double			home_percent;
	int				has_home_stats;

	memset(&fs_stats, 0, sizeof(fs_stats));
	has_home_stats = (statvfs(g_state.current_dir, &fs_stats) == 0);
	home_percent = 0.0;
	if (has_home_stats && fs_stats.f_blocks > 0)
		home_percent = (double)(fs_stats.f_blocks - fs_stats.f_bavail)
			/ fs_stats.f_blocks * 100.0;
	if (!has_home_stats)
	{
		mvprintw(1, 1, "Storage information unavailable");
		mvhline(2, 0, ACS_HLINE, max_x);
		return ;
	}
	format_size((off_t)(fs_stats.f_blocks - fs_stats.f_bfree)
		* fs_stats.f_frsize, used, sizeof(used));
	format_size((off_t)fs_stats.f_bavail * fs_stats.f_frsize,
		free_space, sizeof(free_space));
	render_gauge(gauge, home_percent, max_x >= 72 ? 12 : 6);
	attron(COLOR_PAIR(home_percent > 85.0 ? 4 : 2) | A_BOLD);
	mvprintw(1, 1, "Disk %s", gauge);
	attroff(COLOR_PAIR(home_percent > 85.0 ? 4 : 2) | A_BOLD);
	if (max_x >= 52)
		printw("  %s used  %s free", used, free_space);
	if (max_x >= 86)
		printw("  %llu inodes",
			(unsigned long long)(fs_stats.f_files - fs_stats.f_ffree));
	mvhline(2, 0, ACS_HLINE, max_x);
}
