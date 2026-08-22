#include "ft_ncdu.h"

off_t calculate_dir_recursive(const char *dir_path, size_t *items_count) {
    off_t total_size = 0;
    DIR *dir = opendir(dir_path);
    if (!dir) return 0;

    struct dirent *entry;
    char sub_path[PATH_MAX_LEN];
    struct stat st;
    size_t dir_len = strlen(dir_path);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t name_len = strlen(entry->d_name);
        if (dir_len + 1 + name_len >= PATH_MAX_LEN) continue;

        memcpy(sub_path, dir_path, dir_len);
        sub_path[dir_len] = '/';
        memcpy(sub_path + dir_len + 1, entry->d_name, name_len + 1);

        if (lstat(sub_path, &st) == -1) continue;
        if (items_count) (*items_count)++;

        if (S_ISLNK(st.st_mode)) {
            continue;
        } else if (S_ISDIR(st.st_mode)) {
            total_size += calculate_dir_recursive(sub_path, items_count);
        } else if (S_ISREG(st.st_mode)) {
            total_size += st.st_size;
        }
    }
    closedir(dir);
    return total_size;
}

typedef struct {
    int start_idx;
    int end_idx;
} ThreadTask;

static void *scan_thread_worker(void *arg) {
    ThreadTask *task = (ThreadTask *)arg;
    for (int i = task->start_idx; i < task->end_idx; i++) {
        if (g_state.entries[i].type == TYPE_DIR) {
            size_t count = 0;
            g_state.entries[i].size = calculate_dir_recursive(g_state.entries[i].path, &count);
            g_state.entries[i].items_count = count;
        }
    }
    return NULL;
}

int compare_entries(const void *a, const void *b) {
    const FileEntry *ea = (const FileEntry *)a;
    const FileEntry *eb = (const FileEntry *)b;

    if (g_state.sort_mode == SORT_SIZE_DESC) {
        if (eb->size > ea->size) return 1;
        if (eb->size < ea->size) return -1;
        return strcmp(ea->name, eb->name);
    } else {
        return strcasecmp(ea->name, eb->name);
    }
}

void apply_filter(void) {
    g_state.filtered_count = 0;
    g_state.total_dir_size = 0;
    g_state.max_item_size = 0;

    for (int i = 0; i < g_state.count; i++) {
        if (g_state.search_query[0] == '\0' || strcasestr(g_state.entries[i].name, g_state.search_query)) {
            g_state.filtered[g_state.filtered_count++] = g_state.entries[i];
            g_state.total_dir_size += g_state.entries[i].size;
            if (g_state.entries[i].size > g_state.max_item_size) {
                g_state.max_item_size = g_state.entries[i].size;
            }
        }
    }
    if (g_state.selected >= g_state.filtered_count) {
        g_state.selected = (g_state.filtered_count > 0) ? g_state.filtered_count - 1 : 0;
    }
}

void scan_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    g_state.count = 0;
    g_state.selected = 0;
    g_state.scroll_offset = 0;
    safe_str_copy(g_state.current_dir, path, sizeof(g_state.current_dir));

    struct dirent *entry;
    struct stat st;
    char full_path[PATH_MAX_LEN];
    size_t path_len = strlen(path);

    while ((entry = readdir(dir)) != NULL && g_state.count < MAX_ENTRIES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t name_len = strlen(entry->d_name);
        if (path_len + 1 + name_len >= PATH_MAX_LEN) continue;

        memcpy(full_path, path, path_len);
        full_path[path_len] = '/';
        memcpy(full_path + path_len + 1, entry->d_name, name_len + 1);

        if (lstat(full_path, &st) == -1) continue;

        FileEntry *fe = &g_state.entries[g_state.count];
        safe_str_copy(fe->name, entry->d_name, sizeof(fe->name));
        safe_str_copy(fe->path, full_path, sizeof(fe->path));
        fe->mode = st.st_mode;
        fe->mtime = st.st_mtime;
        fe->marked = 0;
        fe->items_count = 1;

        if (S_ISLNK(st.st_mode)) {
            fe->type = TYPE_LINK;
            fe->size = 0;
        } else if (S_ISDIR(st.st_mode)) {
            fe->type = TYPE_DIR;
            fe->size = 0;
        } else {
            fe->type = TYPE_FILE;
            fe->size = st.st_size;
        }
        g_state.count++;
    }
    closedir(dir);

    pthread_t threads[THREAD_COUNT];
    ThreadTask tasks[THREAD_COUNT];
    int items_per_thread = (g_state.count + THREAD_COUNT - 1) / THREAD_COUNT;

    for (int i = 0; i < THREAD_COUNT; i++) {
        tasks[i].start_idx = i * items_per_thread;
        tasks[i].end_idx = (i + 1) * items_per_thread;
        if (tasks[i].end_idx > g_state.count) tasks[i].end_idx = g_state.count;
        if (tasks[i].start_idx < g_state.count) {
            pthread_create(&threads[i], NULL, scan_thread_worker, &tasks[i]);
        } else {
            threads[i] = 0;
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        if (threads[i]) pthread_join(threads[i], NULL);
    }

    qsort(g_state.entries, g_state.count, sizeof(FileEntry), compare_entries);
    apply_filter();
}
