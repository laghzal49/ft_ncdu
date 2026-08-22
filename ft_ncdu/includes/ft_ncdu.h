#ifndef FT_NCDU_H
#define FT_NCDU_H

#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <pthread.h>
#include <ncurses.h>

#include "config.h"

typedef enum {
    TYPE_FILE,
    TYPE_DIR,
    TYPE_LINK
} EntryType;

typedef enum {
    SORT_SIZE_DESC,
    SORT_SIZE_ASC,
    SORT_NAME_ASC,
    SORT_MTIME_DESC
} SortMode;

typedef enum {
    SIZE_ACTUAL_DISK,
    SIZE_APPARENT
} SizeMode;

typedef struct {
    char        name[256];
    char        path[PATH_MAX_LEN];
    char        symlink_target[PATH_MAX_LEN];
    off_t       size;
    off_t       disk_size;
    size_t      items_count;
    mode_t      mode;
    time_t      mtime;
    dev_t       dev;
    ino_t       ino;
    EntryType   type;
    int         marked;
    int         is_broken_link;
    int         is_goinfre_link;
} FileEntry;

typedef struct {
    FileEntry       entries[MAX_ENTRIES];
    FileEntry       filtered[MAX_ENTRIES];
    int             count;
    int             filtered_count;
    int             selected;
    int             scroll_offset;
    off_t           total_dir_size;
    off_t           total_disk_usage;
    off_t           max_item_size;
    char            current_dir[PATH_MAX_LEN];
    char            username[64];
    char            search_query[128];
    int             is_searching;
    SortMode        sort_mode;
    SizeMode        size_mode;
    int             show_hidden;
    volatile int    is_scanning;
    volatile int    abort_scan;
    int             spinner_frame;
    size_t          unreadable_count;
    size_t          broken_links_count;
    dev_t           root_dev;
    pthread_mutex_t lock;
} AppState;

extern AppState g_state;

typedef struct {
    int         key;
    const char  *name;
    const char  *desc;
    void        (*handler)(void);
} CommandBinding;

/* utils.c */
void    safe_str_copy(char *dest, const char *src, size_t dest_size);
void    format_size(off_t bytes, char *out, size_t out_len);
void    format_permissions(mode_t mode, char *out);
void    render_gauge(char *out, size_t out_len, double percent, int width);
void    render_graph_bar(char *out, size_t out_len, off_t size, off_t max_size, int bar_width);
int     is_protected_target(const char *path);
int     count_marked_items(void);
void    format_breadcrumbs(const char *path, char *out, size_t max_len);
char    *shell_escape(const char *str);

/* scanner.c */
off_t   calculate_dir_recursive(const char *dir_path, size_t *items_count, off_t *out_disk_size, int depth);
int     compare_entries(const void *a, const void *b);
void    apply_filter(void);
void    start_async_scan(const char *path);

/* actions.c */
void    get_goinfre_path(char *dest, size_t dest_len);
int     confirm_modal(const char *title, const char *message);
void    show_help_modal(void);
void    action_delete(void);
void    action_symlink_goinfre(void);
void    action_unlink_goinfre(void);
void    action_bootstrap_goinfre(void);
void    action_heal_symlinks(void);
void    action_empty_trash(void);
void    action_nuke_junk(void);
void    action_git_doctor(void);
void    action_docker_prune(void);
void    action_custom_command(void);
void    action_export_report(void);
void    action_cleaning_presets(void);
void    action_file_peek(void);
void    action_goto_path(void);
void    action_inject_zshrc(void);
void    action_edit(void);
void    action_shell(void);
void    action_batch_invert(void);
void    action_batch_unmark(void);

/* Headless CLI functions */
int     run_cli_clean(void);
int     run_cli_heal(void);
int     run_cli_bootstrap(void);
int     run_cli_report(const char *target_path);
void    print_cli_help(const char *prog_name);
void    print_cli_version(void);

/* ui.c */
void    init_ui_colors(void);
void    draw_ui(void);

#endif
