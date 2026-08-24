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
	width_limit = available_width - 22;
	if (width_limit < 4)
		width_limit = 4;
	if (entry->is_broken_link)
	{
		attron(COLOR_PAIR(4) | A_BOLD);
		mvprintw(rect.y + 7, start_x, "Link -> : %.*s [DEAD LINK!]",
			width_limit, entry->symlink_target);
		wattroff(stdscr, COLOR_PAIR(4) | A_BOLD);
	}
	else
	{
		attron(COLOR_PAIR(2));
		mvprintw(rect.y + 7, start_x, "Link -> : %.*s [HEALTHY]",
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
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(rect.y + 1, start_x, "⌘ GET INFO");
	wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
	mvprintw(rect.y + 2, start_x, "Name    : %.*s", name_width, entry->name);
	mvprintw(rect.y + 3, start_x, "Items   : %zu items", entry->items_count);
	mvprintw(rect.y + 4, start_x, "Size    : %s", size_str);
	mvprintw(rect.y + 5, start_x, "Perms   : %s (%04o)", perms_str,
		entry->mode & 0777);
	render_link_status(rect, entry, start_x, available_width);
}

static void	render_command_deck(t_rect rect, int divider_y, int start_x)
{
	mvaddstr(divider_y, rect.x, "├");
	mvaddstr(divider_y, rect.x + rect.w - 1, "┤");
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(divider_y + 1, start_x, " macOS SHORTCUTS");
	wattroff(stdscr, COLOR_PAIR(1) | A_BOLD);
	mvprintw(divider_y + 2, start_x, "[s] Link Goinfre    [u] Restore Home");
	mvprintw(divider_y + 3, start_x, "[H] Heal Station    [T] Empty Trash");
	mvprintw(divider_y + 4, start_x, "[C] Clean Presets   [b] Bootstrap Tools");
	mvprintw(divider_y + 5, start_x, "[p] Quick Look      [Z] Inject .zshrc");
	mvprintw(divider_y + 6, start_x, "[o] Sort Mode       [r] Reload Finder");
	mvprintw(divider_y + 7, start_x, "[e] Open in $EDITOR [t] Open Terminal");
	mvprintw(divider_y + 8, start_x, "[A] Size Mode       [E] Export Report");
}

void	render_inspector(t_rect rect, int split_x)
{
	int				start_x;
	int				available_width;
	int				divider_y;
	t_file_entry	*active_entry;

	draw_box(rect, "⌘ GET INFO", 1);
	start_x = split_x + 2;
	available_width = rect.w - 4;
	pthread_mutex_lock(&g_state.lock);
	if (g_state.filtered_count > 0 && g_state.selected < g_state.filtered_count)
	{
		active_entry = &g_state.filtered[g_state.selected];
		render_target_metadata(rect, active_entry, start_x, available_width);
	}
	pthread_mutex_unlock(&g_state.lock);
	divider_y = rect.y + 9;
	if (divider_y < rect.y + rect.h - 10)
		render_command_deck(rect, divider_y, start_x);
}
