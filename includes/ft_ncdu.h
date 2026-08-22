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
#include <pthread.h>
#include <ncurses.h>

#define MAX_ENTRIES   8192
#define PATH_MAX_LEN  4096
#define THREAD_COUNT  16

typedef enum {
    TYPE_FILE,
    TYPE_DIR,
    TYPE_LINK
} EntryType;

typedef enum {
    SORT_SIZE_DESC,
    SORT_NAME_ASC
} SortMode;

typedef struct {
    char        name[256];
    char        path[PATH_MAX_LEN];
    off_t       size;
    size_t      items_count;
    mode_t      mode;
    time_t      mtime;
    EntryType   type;
    int         marked;
} FileEntry;

typedef struct {
    FileEntry   entries[MAX_ENTRIES];
    FileEntry   filtered[MAX_ENTRIES];
    int         count;
    int         filtered_count;
    int         selected;
    int         scroll_offset;
    off_t       total_dir_size;
    off_t       max_item_size;
    char        current_dir[PATH_MAX_LEN];
    char        username[64];
    char        search_query[128];
    int         is_searching;
    SortMode    sort_mode;
} AppState;

extern AppState g_state;

/* utils.c */
void    safe_str_copy(char *dest, const char *src, size_t dest_size);
void    format_size(off_t bytes, char *out, size_t out_len);
void    format_permissions(mode_t mode, char *out);
void    render_gauge(char *out, size_t out_len, double percent, int width);
void    render_graph_bar(char *out, size_t out_len, off_t size, off_t max_size, int bar_width);
int     is_protected_target(const char *path);
int     count_marked_items(void);

/* scanner.c */
off_t   calculate_dir_recursive(const char *dir_path, size_t *items_count);
int     compare_entries(const void *a, const void *b);
void    apply_filter(void);
void    scan_directory(const char *path);

/* actions.c */
void    get_goinfre_path(char *dest, size_t dest_len);
int     confirm_modal(const char *title, const char *message);
void    show_help_modal(void);
void    action_delete(void);
void    action_symlink_goinfre(void);
void    action_bootstrap_goinfre(void);
void    action_heal_symlinks(void);
void    action_nuke_junk(void);
void    action_git_doctor(void);
void    action_docker_prune(void);
void    action_edit(void);
void    action_shell(void);

/* ui.c */
void    init_ui_colors(void);
void    draw_ui(void);

#endif
