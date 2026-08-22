#include "ft_ncdu.h"

off_t calculate_dir_recursive(const char *dir_path, size_t *items_count, off_t *out_disk_size, int depth) {
    if (g_state.abort_scan || depth > MAX_DEPTH) return 0;

    off_t total_size = 0;
    off_t total_disk = 0;
    DIR *dir = opendir(dir_path);
    if (!dir) {
        pthread_mutex_lock(&g_state.lock);
        g_state.unreadable_count++;
        pthread_mutex_unlock(&g_state.lock);
        return 0;
    }

    struct dirent *entry;
    char sub_path[PATH_MAX_LEN];
    struct stat st;
    size_t dir_len = strlen(dir_path);

    while (!g_state.abort_scan && (entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t name_len = strlen(entry->d_name);
        if (dir_len + 1 + name_len >= PATH_MAX_LEN) continue;

        memcpy(sub_path, dir_path, dir_len);
        sub_path[dir_len] = '/';
        memcpy(sub_path + dir_len + 1, entry->d_name, name_len + 1);

        if (lstat(sub_path, &st) == -1) continue;

        /* Filesystem boundary guard (prevent hopping into remote NFS or root) */
        if (st.st_dev != g_state.root_dev) continue;

        if (items_count) (*items_count)++;

        if (S_ISLNK(st.st_mode)) {
            continue;
        } else if (S_ISDIR(st.st_mode)) {
            off_t sub_disk = 0;
            total_size += calculate_dir_recursive(sub_path, items_count, &sub_disk, depth + 1);
            total_disk += sub_disk;
        } else if (S_ISREG(st.st_mode)) {
            total_size += st.st_size;
            total_disk += (st.st_blocks * 512);
        }
    }
    closedir(dir);

    if (out_disk_size) *out_disk_size = total_disk;
    return total_size;
}

typedef struct {
    int start_idx;
    int end_idx;
} ThreadTask;

static void *scan_thread_worker(void *arg) {
    ThreadTask *task = (ThreadTask *)arg;
    for (int i = task->start_idx; i < task->end_idx; i++) {
        if (g_state.abort_scan) break;

        if (g_state.entries[i].type == TYPE_DIR) {
            size_t count = 0;
            off_t disk_sz = 0;
            off_t sz = calculate_dir_recursive(g_state.entries[i].path, &count, &disk_sz, 0);

            if (!g_state.abort_scan) {
                pthread_mutex_lock(&g_state.lock);
                g_state.entries[i].size = sz;
                g_state.entries[i].disk_size = disk_sz;
                g_state.entries[i].items_count = count;
                pthread_mutex_unlock(&g_state.lock);
            }
        }
    }
    return NULL;
}

int compare_entries(const void *a, const void *b) {
    const FileEntry *ea = (const FileEntry *)a;
    const FileEntry *eb = (const FileEntry *)b;

    off_t size_a = (g_state.size_mode == SIZE_ACTUAL_DISK) ? ea->disk_size : ea->size;
    off_t size_b = (g_state.size_mode == SIZE_ACTUAL_DISK) ? eb->disk_size : eb->size;

    if (g_state.sort_mode == SORT_SIZE_DESC) {
        if (size_b > size_a) return 1;
        if (size_b < size_a) return -1;
        return strcmp(ea->name, eb->name);
    } else if (g_state.sort_mode == SORT_SIZE_ASC) {
        if (size_a > size_b) return 1;
        if (size_a < size_b) return -1;
        return strcmp(ea->name, eb->name);
    } else if (g_state.sort_mode == SORT_MTIME_DESC) {
        if (eb->mtime > ea->mtime) return 1;
        if (eb->mtime < ea->mtime) return -1;
        return strcmp(ea->name, eb->name);
    } else {
        return strcasecmp(ea->name, eb->name);
    }
}

void apply_filter(void) {
    pthread_mutex_lock(&g_state.lock);
    g_state.filtered_count = 0;
    g_state.total_dir_size = 0;
    g_state.total_disk_usage = 0;
    g_state.max_item_size = 0;

    for (int i = 0; i < g_state.count; i++) {
        if (!g_state.show_hidden && g_state.entries[i].name[0] == '.') {
            continue;
        }

        if (g_state.search_query[0] == '\0' || strcasestr(g_state.entries[i].name, g_state.search_query)) {
            g_state.filtered[g_state.filtered_count++] = g_state.entries[i];
            g_state.total_dir_size += g_state.entries[i].size;
            g_state.total_disk_usage += g_state.entries[i].disk_size;

            off_t effective_size = (g_state.size_mode == SIZE_ACTUAL_DISK) ?
                                   g_state.entries[i].disk_size : g_state.entries[i].size;

            if (effective_size > g_state.max_item_size) {
                g_state.max_item_size = effective_size;
            }
        }
    }

    if (g_state.filtered_count == 0) {
        g_state.selected = 0;
        g_state.scroll_offset = 0;
    } else if (g_state.selected >= g_state.filtered_count) {
        g_state.selected = g_state.filtered_count - 1;
    }
    pthread_mutex_unlock(&g_state.lock);
}

static void *async_scan_orchestrator(void *arg) {
    char *path = (char *)arg;
    DIR *dir = opendir(path);
    if (!dir) {
        g_state.is_scanning = 0;
        free(path);
        return NULL;
    }

    struct stat root_st;
    if (stat(path, &root_st) == 0) {
        g_state.root_dev = root_st.st_dev;
    }

    pthread_mutex_lock(&g_state.lock);
    g_state.count = 0;
    g_state.selected = 0;
    g_state.scroll_offset = 0;
    g_state.unreadable_count = 0;
    g_state.broken_links_count = 0;
    safe_str_copy(g_state.current_dir, path, sizeof(g_state.current_dir));
    pthread_mutex_unlock(&g_state.lock);

    struct dirent *entry;
    struct stat st;
    char full_path[PATH_MAX_LEN];
    size_t path_len = strlen(path);

    while (!g_state.abort_scan && (entry = readdir(dir)) != NULL && g_state.count < MAX_ENTRIES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t name_len = strlen(entry->d_name);
        if (path_len + 1 + name_len >= PATH_MAX_LEN) continue;

        memcpy(full_path, path, path_len);
        full_path[path_len] = '/';
        memcpy(full_path + path_len + 1, entry->d_name, name_len + 1);

        if (lstat(full_path, &st) == -1) continue;

        pthread_mutex_lock(&g_state.lock);
        FileEntry *fe = &g_state.entries[g_state.count];
        safe_str_copy(fe->name, entry->d_name, sizeof(fe->name));
        safe_str_copy(fe->path, full_path, sizeof(fe->path));
        fe->symlink_target[0] = '\0';
        fe->mode = st.st_mode;
        fe->mtime = st.st_mtime;
        fe->dev = st.st_dev;
        fe->ino = st.st_ino;
        fe->marked = 0;
        fe->is_broken_link = 0;
        fe->is_goinfre_link = 0;
        fe->items_count = 1;

        if (S_ISLNK(st.st_mode)) {
            fe->type = TYPE_LINK;
            fe->size = 0;
            fe->disk_size = 0;

            ssize_t rlen = readlink(full_path, fe->symlink_target, sizeof(fe->symlink_target) - 1);
            if (rlen > 0) {
                fe->symlink_target[rlen] = '\0';
            }

            struct stat target_st;
            if (stat(full_path, &target_st) != 0) {
                fe->is_broken_link = 1;
                g_state.broken_links_count++;
            }

            if (strstr(fe->symlink_target, "goinfre") != NULL) {
                fe->is_goinfre_link = 1;
            }
        } else if (S_ISDIR(st.st_mode)) {
            fe->type = TYPE_DIR;
            fe->size = 0;
            fe->disk_size = 0;
        } else {
            fe->type = TYPE_FILE;
            fe->size = st.st_size;
            fe->disk_size = (st.st_blocks * 512);
        }
        g_state.count++;
        pthread_mutex_unlock(&g_state.lock);
    }
    closedir(dir);

    if (g_state.abort_scan) {
        g_state.is_scanning = 0;
        free(path);
        return NULL;
    }

    apply_filter();

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

    if (!g_state.abort_scan) {
        pthread_mutex_lock(&g_state.lock);
        qsort(g_state.entries, g_state.count, sizeof(FileEntry), compare_entries);
        pthread_mutex_unlock(&g_state.lock);
        apply_filter();
    }

    g_state.is_scanning = 0;
    free(path);
    return NULL;
}

void start_async_scan(const char *path) {
    g_state.abort_scan = 1;
    while (g_state.is_scanning) {
        usleep(1000);
    }
    g_state.abort_scan = 0;
    g_state.is_scanning = 1;

    char *path_copy = strdup(path);
    pthread_t orchestrator;
    pthread_create(&orchestrator, NULL, async_scan_orchestrator, path_copy);
    pthread_detach(orchestrator);
}

