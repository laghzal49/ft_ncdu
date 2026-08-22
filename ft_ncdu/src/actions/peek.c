/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   peek.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

#define MAX_PEEK 2000

static int	read_peek_lines(const char *path, char **lines, int *is_bin)
{
	FILE	*fp;
	char	buf[1024];
	int		count;

	fp = fopen(path, "r");
	if (!fp)
		return (0);
	count = 0;
	*is_bin = 0;
	while (count < MAX_PEEK && fgets(buf, sizeof(buf), fp))
	{
		if ((unsigned char)buf[0] < 9)
		{
			*is_bin = 1;
			break ;
		}
		buf[strcspn(buf, "\r\n")] = 0;
		lines[count++] = strdup(buf);
	}
	fclose(fp);
	return (count);
}

static void	render_peek_lines(WINDOW *win, char **lines, int count, int scroll)
{
	int	h;
	int	w;
	int	max_d;
	int	i;
	int	idx;

	getmaxyx(win, h, w);
	max_d = h - 4;
	i = 0;
	while (i < max_d)
	{
		idx = scroll + i;
		if (idx >= count)
			break ;
		mvwprintw(win, 3 + i, 2, "%4d │ %.*s",
			idx + 1, w - 10, lines[idx]);
		i++;
	}
}

static int	handle_peek_scroll(int ch, int scroll, int count, int h)
{
	if ((ch == 'j' || ch == KEY_DOWN) && scroll < count - (h - 4))
		scroll++;
	else if ((ch == 'k' || ch == KEY_UP) && scroll > 0)
		scroll--;
	else if (ch == KEY_NPAGE)
		scroll += (h - 4);
	else if (ch == KEY_PPAGE)
		scroll -= (h - 4);
	if (scroll < 0)
		scroll = 0;
	return (scroll);
}

static void	peek_scroll_loop(WINDOW *win, char **lines, int count, int is_bin)
{
	int	h;
	int	scroll;
	int	ch;

	scroll = 0;
	while (1)
	{
		h = getmaxy(win);
		werase(win);
		box(win, 0, 0);
		wattron(win, COLOR_PAIR(1) | A_BOLD);
		mvwprintw(win, 1, 2, ":: [ FILE PEEK ] :: (%d lines)", count);
		wattroff(win, COLOR_PAIR(1) | A_BOLD);
		if (is_bin)
			mvwprintw(win, 3, 2, "[Binary File]");
		else if (count > 0)
			render_peek_lines(win, lines, count, scroll);
		wrefresh(win);
		ch = wgetch(win);
		if (ch == 'q' || ch == 27 || ch == 'p')
			break ;
		scroll = handle_peek_scroll(ch, scroll, count, h);
	}
}

void	action_file_peek(void)
{
	t_file_entry	*target;
	char			**lines;
	int				is_bin;
	int				count;
	WINDOW			*win;

	if (g_state.filtered_count == 0)
		return ;
	target = &g_state.filtered[g_state.selected];
	if (target->type == TYPE_DIR)
		return ;
	lines = malloc(MAX_PEEK * sizeof(char *));
	if (!lines)
		return ;
	count = read_peek_lines(target->path, lines, &is_bin);
	win = newwin(LINES - 4, COLS - 8, 2, 4);
	keypad(win, TRUE);
	peek_scroll_loop(win, lines, count, is_bin);
	delwin(win);
	while (count > 0)
		free(lines[--count]);
	free(lines);
}
