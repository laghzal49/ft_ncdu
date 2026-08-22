/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

void	render_gauge(char *buf, double pct, int width)
{
	int	filled;
	int	i;

	if (width < 2)
		width = 2;
	if (width > 20)
		width = 20;
	filled = (int)((pct / 100.0) * width);
	if (filled > width)
		filled = width;
	if (filled < 0)
		filled = 0;
	buf[0] = '[';
	i = 0;
	while (i < filled)
		buf[1 + i++] = '#';
	while (i < width)
		buf[1 + i++] = '.';
	buf[1 + width] = ']';
	buf[2 + width] = ' ';
	snprintf(buf + 3 + width, 16, "%4.1f%%", pct);
}

void	render_graph_bar(char *buf, off_t val, off_t max, int w)
{
	int		filled;
	int		i;
	double	pct;

	if (w < 2)
		w = 2;
	if (w > 16)
		w = 16;
	pct = 0.0;
	if (max > 0)
		pct = (double)val / (double)max;
	filled = (int)(pct * w);
	if (filled > w)
		filled = w;
	buf[0] = '[';
	i = 0;
	while (i < filled)
		buf[1 + i++] = '#';
	while (i < w)
		buf[1 + i++] = '.';
	buf[1 + w] = ']';
	buf[2 + w] = '\0';
}

static void	render_home_card(t_rect r, double h_pct)
{
	char	gauge[64];
	int		h_col;
	int		w_param;

	draw_box(r, "HOME QUOTA", 1);
	w_param = 6;
	if (r.w > 30)
		w_param = 10;
	render_gauge(gauge, h_pct, w_param);
	h_col = 2;
	if (h_pct > 85.0)
		h_col = 4;
	else if (h_pct > 70.0)
		h_col = 3;
	attron(COLOR_PAIR(h_col) | A_BOLD);
	mvprintw(r.y + 1, r.x + 2, "%.*s", r.w - 4, gauge);
	wattroff(stdscr, COLOR_PAIR(h_col) | A_BOLD);
}

static void	render_goinfre_card(t_rect r, int has_g, double g_pct, off_t g_free)
{
	char	sz_free[16];
	char	gauge[64];

	draw_box(r, "GOINFRE NVMe", 1);
	if (has_g)
	{
		format_size(g_free, sz_free, sizeof(sz_free));
		render_gauge(gauge, g_pct, 8);
		attron(COLOR_PAIR(6) | A_BOLD);
		mvprintw(r.y + 1, r.x + 2, "%s (%s Free)", gauge, sz_free);
		wattroff(stdscr, COLOR_PAIR(6) | A_BOLD);
	}
	else
	{
		attron(COLOR_PAIR(7));
		mvprintw(r.y + 1, r.x + 2, "/tmp Fallback Pool");
		attroff(COLOR_PAIR(7));
	}
}

void	render_top_hud(int max_x)
{
	struct statvfs	vfs;
	struct statvfs	vfsg;
	int				card_w;
	t_rect			r;
	double			h_pct;

	card_w = (max_x - 4) / 3;
	if (card_w < 20)
		card_w = 20;
	statvfs(g_state.current_dir, &vfs);
	h_pct = 0.0;
	if (vfs.f_blocks > 0)
		h_pct = (double)(vfs.f_blocks - vfs.f_bfree) / vfs.f_blocks * 100.0;
	r = (t_rect){1, 0, 3, card_w + 1};
	render_home_card(r, h_pct);
	r = (t_rect){1, card_w + 1, 3, card_w + 1};
	draw_box(r, "INODES", 1);
	mvprintw(2, card_w + 3, "Inodes: %llu used",
		(unsigned long long)(vfs.f_files - vfs.f_ffree));
	r = (t_rect){1, (card_w + 1) * 2, 3, max_x - (card_w + 1) * 2};
	render_goinfre_card(r, statvfs("/goinfre", &vfsg) == 0, 0.0,
		(off_t)vfsg.f_bfree * vfsg.f_frsize);
}
