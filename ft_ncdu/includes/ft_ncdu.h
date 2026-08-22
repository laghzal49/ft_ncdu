/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ncdu.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NCDU_H
# define FT_NCDU_H

# define _GNU_SOURCE
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/statvfs.h>
# include <sys/types.h>
# include <pthread.h>
# include <ncurses.h>
# include <locale.h>
# include <pwd.h>
# include <signal.h>
# include <time.h>
# include <ctype.h>

# include "config.h"

typedef enum e_file_type
{
	TYPE_FILE,
	TYPE_DIR,
	TYPE_LINK,
	TYPE_OTHER
}	t_file_type;

typedef enum e_sort_mode
{
	SORT_SIZE_DESC,
	SORT_SIZE_ASC,
	SORT_NAME_ASC,
	SORT_MTIME_DESC
}	t_sort_mode;

typedef enum e_size_mode
{
	SIZE_ACTUAL_DISK,
	SIZE_APPARENT
}	t_size_mode;

typedef struct s_file_entry
{
	char		name[NAME_MAX_LEN];
	char		path[PATH_MAX_LEN];
	char		symlink_target[PATH_MAX_LEN];
	off_t		size;
	off_t		disk_size;
	size_t		items_count;
	time_t		mtime;
	mode_t		mode;
	t_file_type	type;
	int			marked;
	int			is_goinfre_link;
	int			is_broken_link;
}	t_file_entry;

typedef struct s_app_state
{
	t_file_entry	*entries;
	t_file_entry	*filtered;
	int				count;
	int				filtered_count;
	int				selected;
	int				scroll_offset;
	off_t			total_dir_size;
	off_t			total_disk_usage;
	off_t			max_item_size;
	char			current_dir[PATH_MAX_LEN];
	char			username[64];
	char			search_query[128];
	t_sort_mode		sort_mode;
	t_size_mode		size_mode;
	int				is_searching;
	int				show_hidden;
	volatile int	is_scanning;
	volatile int	abort_scan;
	pthread_mutex_t	lock;
	int				spinner_frame;
	size_t			unreadable_count;
	size_t			broken_links_count;
}	t_app_state;

typedef struct s_clean_preset
{
	char		key;
	const char	*title;
	const char	*desc;
	const char	*command_fmt;
}	t_clean_preset;

typedef struct s_cmd_binding
{
	int			key;
	const char	*name;
	const char	*desc;
	void		(*handler)(void);
}	t_cmd_binding;

typedef struct s_rect
{
	int	y;
	int	x;
	int	h;
	int	w;
}	t_rect;

extern t_app_state	g_state;

/* Core Scanner & Workers */
void	start_async_scan(const char *dir_path);
void	calculate_dir_recursive(const char *path, off_t *out_s, off_t *out_d);
void	check_symlink_health(t_file_entry *entry);
void	run_event_loop(void);

/* UI & Rendering */
void	init_ui_colors(void);
void	draw_ui(void);
void	draw_box(t_rect r, const char *title, int color);
void	render_top_hud(int max_x);
void	render_file_table(t_rect r, int split_x);
void	render_inspector(t_rect r, int split_x);
void	render_status_footer(int max_y, int max_x);
int		confirm_modal(const char *title, const char *message);
void	show_help_modal(void);
void	render_gauge(char *buf, double pct, int width);
void	render_graph_bar(char *buf, off_t val, off_t max, int w);

/* Action Handlers */

void	action_delete(void);
void	action_symlink_goinfre(void);
void	action_unlink_goinfre(void);
void	action_bootstrap_goinfre(void);
void	action_heal_symlinks(void);
void	action_empty_trash(void);
void	action_inject_zshrc(void);
void	action_goto_path(void);
void	action_file_peek(void);
void	action_nuke_junk(void);
void	action_git_doctor(void);
void	action_docker_prune(void);
void	action_custom_command(void);
void	action_export_report(void);
void	action_cleaning_presets(void);
void	action_edit(void);
void	action_shell(void);
void	action_batch_invert(void);
void	action_batch_unmark(void);

/* Headless CLI Operations */
int		run_cli_clean(void);
int		run_cli_heal(void);
int		run_cli_bootstrap(void);
int		run_cli_report(const char *target_path);
void	print_cli_help(const char *prog_name);
void	print_cli_version(void);

/* Utilities */
void	safe_str_copy(char *dest, const char *src, size_t dest_len);
void	format_size(off_t bytes, char *out, size_t out_len);
void	format_permissions(mode_t mode, char *out);
void	format_breadcrumbs(const char *path, char *out, size_t max_len);
char	*shell_escape(const char *str);
int		is_protected_target(const char *path);
int		count_marked_items(void);
void	apply_filter(void);
int		compare_entries(const void *a, const void *b);
void	get_goinfre_path(char *dest, size_t dest_len);
void	init_state_memory(void);
void	free_state_memory(void);

#endif
