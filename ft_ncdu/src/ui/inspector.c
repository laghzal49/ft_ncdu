/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inspector.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	render_link_status(t_rect r, t_file_entry *cur, int rx, int rw)
{
	if (cur->type != TYPE_LINK)
		return ;
	if (cur->is_broken_link)
	{
		attron(COLOR_PAIR(4) | A_BOLD);
		mvprintw(r.y + 7, rx, "Link -> : %.*s [DEAD LINK!]",
			rw - 22, cur->symlink_target);
		wattroff(stdscr, COLOR_PAIR(4) | A_BOLD);
	}
	else
	{
		attron(COLOR_PAIR(2));
		mvprintw(r.y + 7, rx, "Link -> : %.*s [HEALTHY]",
			rw - 22, cur->symlink_target);
		wattroff(stdscr, COLOR_PAIR(2));
	}
}

static void	render_target_metadata(t_rect r, t_file_entry *cur,
		int rx, int rw)
{
	char	perms[16];
	char	sz_buf[32];
	off_t	eff_sz;

	format_permissions(cur->mode, perms);
	eff_sz = cur->size;
	if (g_state.size_mode == SIZE_ACTUAL_DISK)
		eff_sz = cur->disk_size;
	snprintf(sz_buf, sizeof(sz_buf), "%lld Bytes", (long long)eff_sz);
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(r.y + 1, rx, "TARGET METADATA");
	wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
	mvprintw(r.y + 2, rx, "Name    : %.*s", rw - 10, cur->name);
	mvprintw(r.y + 3, rx, "Items   : %zu items", cur->items_count);
	mvprintw(r.y + 4, rx, "Size    : %s", sz_buf);
	mvprintw(r.y + 5, rx, "Perms   : %s (%04o)", perms, cur->mode & 0777);
	render_link_status(r, cur, rx, rw);
}

static void	render_command_deck(t_rect r, int mid_y, int rx)
{
	mvaddstr(mid_y, r.x, "├");
	mvaddstr(mid_y, r.x + r.w - 1, "┤");
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(mid_y + 1, rx, "42 CLUSTER COMMAND DECK");
	wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
	mvprintw(mid_y + 2, rx, "[s] Link Goinfre    [u] Restore Home");
	mvprintw(mid_y + 3, rx, "[H] Heal Station    [T] Empty Trash");
	mvprintw(mid_y + 4, rx, "[C] Clean Presets   [b] Bootstrap Tools");
	mvprintw(mid_y + 5, rx, "[p] Scroll Peek     [Z] Inject .zshrc");
	mvprintw(mid_y + 6, rx, "[A] Size Mode       [a] Toggle Dotfiles");
	mvprintw(mid_y + 7, rx, "[o] Sort Mode       [E] Export Report");
}

void	render_inspector(t_rect r, int split_x)
{
	int				rx;
	int				rw;
	int				mid_y;
	t_file_entry	*cur;

	draw_box(r, "TARGET INSPECTOR", 1);
	rx = split_x + 2;
	rw = r.w - 4;
	pthread_mutex_lock(&g_state.lock);
	if (g_state.filtered_count > 0 && g_state.selected < g_state.filtered_count)
	{
		cur = &g_state.filtered[g_state.selected];
		render_target_metadata(r, cur, rx, rw);
	}
	pthread_mutex_unlock(&g_state.lock);
	mid_y = r.y + 9;
	if (mid_y < r.y + r.h - 9)
		render_command_deck(r, mid_y, rx);
}
