/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inspector.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	render_link_status(t_rect rect, t_file_entry *entry, int start_x,
		int available_width)
{
	int	width_limit;

	if (entry->type != TYPE_LINK)
		return ;
	if (rect.h < 8)
		return ;
	width_limit = available_width - 22;
	if (width_limit < 4)
		width_limit = 4;
	if (entry->is_broken_link)
	{
		attron(COLOR_PAIR(4) | A_BOLD);
		mvprintw(rect.y + 7, start_x, "Link: %.*s [BROKEN]",
			width_limit, entry->symlink_target);
		wattroff(stdscr, COLOR_PAIR(4) | A_BOLD);
	}
	else
	{
		attron(COLOR_PAIR(2));
		mvprintw(rect.y + 7, start_x, "Link: %.*s [OK]",
			width_limit, entry->symlink_target);
		wattroff(stdscr, COLOR_PAIR(2));
	}
}

static void	render_target_metadata(t_rect rect, t_file_entry *entry,
		int start_x, int available_width)
{
	char	perms_str[16];
	char	size_str[32];
	off_t	effective_size;
	int		name_width;

	format_permissions(entry->mode, perms_str);
	effective_size = entry->size;
	if (g_state.size_mode == SIZE_ACTUAL_DISK)
		effective_size = entry->disk_size;
	snprintf(size_str, sizeof(size_str), "%lld Bytes",
		(long long)effective_size);
	name_width = available_width - 10;
	if (name_width < 4)
		name_width = 4;
	mvprintw(rect.y + 1, start_x, "Name  %.*s", name_width, entry->name);
	if (rect.h > 4)
		mvprintw(rect.y + 3, start_x, "Size  %s", size_str);
	if (rect.h > 5)
		mvprintw(rect.y + 4, start_x, "Items %zu", entry->items_count);
	if (rect.h > 6)
		mvprintw(rect.y + 5, start_x, "Mode  %s  %04o", perms_str,
			entry->mode & 0777);
	render_link_status(rect, entry, start_x, available_width);
}

static void	render_command_deck(t_rect rect, int divider_y, int start_x)
{
	mvaddch(divider_y, rect.x, ACS_LTEE);
	mvhline(divider_y, rect.x + 1, ACS_HLINE, rect.w - 2);
	mvaddch(divider_y, rect.x + rect.w - 1, ACS_RTEE);
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(divider_y + 1, start_x, "QUICK KEYS");
	wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
	mvprintw(divider_y + 2, start_x, "Enter open     h parent");
	mvprintw(divider_y + 3, start_x, "/ search       o sort");
	mvprintw(divider_y + 4, start_x, "Space mark     d delete");
	mvprintw(divider_y + 5, start_x, "s move         u restore");
	mvprintw(divider_y + 6, start_x, "? all keys     q quit");
}

void	render_inspector(t_rect rect, int split_x)
{
	int				start_x;
	int				available_width;
	int				divider_y;
	t_file_entry	*active_entry;

	draw_box(rect, "DETAILS", 1);
	start_x = split_x + 2;
	available_width = rect.w - 4;
	pthread_mutex_lock(&g_state.lock);
	if (g_state.filtered_count > 0 && g_state.selected < g_state.filtered_count)
	{
		active_entry = &g_state.filtered[g_state.selected];
		render_target_metadata(rect, active_entry, start_x, available_width);
	}
	pthread_mutex_unlock(&g_state.lock);
	divider_y = rect.y + 8;
	if (rect.h >= 17)
		render_command_deck(rect, divider_y, start_x);
}
