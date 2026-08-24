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

static void	render_home_card(t_rect rect, double home_percent)
{
	char	gauge_text[64];
	int		color_pair;
	int		gauge_width;

	draw_box(rect, "💾 HOME QUOTA", 1);
	gauge_width = 6;
	if (rect.w > 32)
		gauge_width = 10;
	render_gauge(gauge_text, home_percent, gauge_width);
	color_pair = 2;
	if (home_percent > 85.0)
		color_pair = 4;
	else if (home_percent > 70.0)
		color_pair = 3;
	attron(COLOR_PAIR(color_pair) | A_BOLD);
	mvprintw(rect.y + 1, rect.x + 2, "%.*s", rect.w - 4, gauge_text);
	wattroff(stdscr, COLOR_PAIR(color_pair) | A_BOLD);
}

static void	render_goinfre_card(t_rect rect, int has_goinfre,
		double goinfre_percent, off_t free_bytes)
{
	char	free_size_str[16];
	char	gauge_text[64];

	draw_box(rect, "⚡ GOINFRE NVMe", 6);
	if (has_goinfre)
	{
		format_size(free_bytes, free_size_str, sizeof(free_size_str));
		render_gauge(gauge_text, goinfre_percent, 6);
		attron(COLOR_PAIR(6) | A_BOLD);
		mvprintw(rect.y + 1, rect.x + 2, "%s Free (%s)", free_size_str,
			gauge_text);
		wattroff(stdscr, COLOR_PAIR(6) | A_BOLD);
	}
	else
	{
		attron(COLOR_PAIR(7));
		mvprintw(rect.y + 1, rect.x + 2, "/tmp Fallback Storage");
		attroff(COLOR_PAIR(7));
	}
}

void	render_top_hud(int max_x)
{
	struct statvfs	fs_stats;
	struct statvfs	goinfre_stats;
	int				card_width;
	t_rect			rect;
	double			home_percent;

	card_width = (max_x - 4) / 3;
	if (card_width < 20)
		card_width = 20;
	statvfs(g_state.current_dir, &fs_stats);
	home_percent = 0.0;
	if (fs_stats.f_blocks > 0)
		home_percent = (double)(fs_stats.f_blocks - fs_stats.f_bfree)
			/ fs_stats.f_blocks * 100.0;
	rect = (t_rect){1, 0, 3, card_width + 1};
	render_home_card(rect, home_percent);
	rect = (t_rect){1, card_width + 1, 3, card_width + 1};
	draw_box(rect, "📊 INODES", 2);
	attron(COLOR_PAIR(2) | A_BOLD);
	mvprintw(2, card_width + 3, "%llu Inodes Used",
		(unsigned long long)(fs_stats.f_files - fs_stats.f_ffree));
	wattroff(stdscr, COLOR_PAIR(2) | A_BOLD);
	rect = (t_rect){1, (card_width + 1) * 2, 3, max_x - (card_width + 1) * 2};
	render_goinfre_card(rect, statvfs("/goinfre", &goinfre_stats) == 0, 0.0,
		(off_t)goinfre_stats.f_bfree * goinfre_stats.f_frsize);
}
