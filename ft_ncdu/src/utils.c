#include "ft_ncdu.h"

void safe_str_copy(char *dest, const char *src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) return;
    size_t len = strlen(src);
    if (len >= dest_size) len = dest_size - 1;
    memcpy(dest, src, len);
    dest[len] = '\0';
}

void format_size(off_t bytes, char *out, size_t out_len) {
    const char *units[] = {"B ", "KB", "MB", "GB", "TB", "PB"};
    double sz = (double)bytes;
    int unit_idx = 0;

    while (sz >= 1024.0 && unit_idx < 5) {
        sz /= 1024.0;
        unit_idx++;
    }
    if (unit_idx == 0) {
        snprintf(out, out_len, "%5lld %s", (long long)bytes, units[unit_idx]);
    } else if (sz < 10.0) {
        snprintf(out, out_len, "%5.2f %s", sz, units[unit_idx]);
    } else {
        snprintf(out, out_len, "%5.1f %s", sz, units[unit_idx]);
    }
}

void format_permissions(mode_t mode, char *out) {
    out[0] = S_ISDIR(mode) ? 'd' : (S_ISLNK(mode) ? 'l' : '-');
    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    out[3] = (mode & S_IXUSR) ? 'x' : '-';
    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    out[6] = (mode & S_IXGRP) ? 'x' : '-';
    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    out[9] = (mode & S_IXOTH) ? 'x' : '-';
    out[10] = '\0';
}

void render_gauge(char *out, size_t out_len, double percent, int width) {
    if (width <= 0) return;
    int filled = (int)((percent / 100.0) * width);
    if (filled > width) filled = width;
    if (filled < 0) filled = 0;

    char bar[512] = {0};
    int bpos = 0;
    for (int i = 0; i < width && bpos < 480; i++) {
        if (i < filled) {
            strcat(bar, "■");
            bpos += 3;
        } else {
            strcat(bar, "·");
            bpos += 2;
        }
    }
    snprintf(out, out_len, "%s %5.1f%%", bar, percent);
}

void render_graph_bar(char *out, size_t out_len, off_t size, off_t max_size, int bar_width) {
    if (bar_width <= 0 || out_len < 64) return;
    int filled = (max_size > 0) ? (int)(((double)size / (double)max_size) * bar_width) : 0;
    if (filled > bar_width) filled = bar_width;

    char bar[256] = {0};
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            strcat(bar, "■");
        } else {
            strcat(bar, " ");
        }
    }
    snprintf(out, out_len, "[%s]", bar);
}

int is_protected_target(const char *path) {
    if (!path) return 1;
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    for (size_t i = 0; G_PROTECTED_TARGETS[i] != NULL; i++) {
        if (strcmp(base, G_PROTECTED_TARGETS[i]) == 0) return 1;
    }
    return 0;
}

int count_marked_items(void) {
    int total = 0;
    pthread_mutex_lock(&g_state.lock);
    for (int i = 0; i < g_state.count; i++) {
        if (g_state.entries[i].marked) total++;
    }
    pthread_mutex_unlock(&g_state.lock);
    return total;
}

void format_breadcrumbs(const char *path, char *out, size_t max_len) {
    if (!path || !out || max_len == 0) return;
    const char *home = getenv("HOME");
    char temp[PATH_MAX_LEN] = {0};

    if (home && strncmp(path, home, strlen(home)) == 0) {
        snprintf(temp, sizeof(temp), "~%s", path + strlen(home));
    } else {
        safe_str_copy(temp, path, sizeof(temp));
    }

    size_t len = strlen(temp);
    if (len > max_len && max_len > 8) {
        snprintf(out, max_len, "...%s", temp + (len - max_len + 3));
    } else {
        safe_str_copy(out, temp, max_len);
    }
}

char *shell_escape(const char *str) {
    if (!str) return strdup("''");
    size_t len = strlen(str);
    char *escaped = malloc(len * 4 + 3);
    if (!escaped) return NULL;
    char *p = escaped;
    *p++ = '\'';
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\'') {
            *p++ = '\'';
            *p++ = '\\';
            *p++ = '\'';
            *p++ = '\'';
        } else {
            *p++ = str[i];
        }
    }
    *p++ = '\'';
    *p = '\0';
    return escaped;
}

