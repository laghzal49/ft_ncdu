#include "ft_ncdu.h"

void init_ui_colors(void) {
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, -1);
        init_pair(2, COLOR_GREEN, -1);
        init_pair(3, COLOR_YELLOW, -1);
        init_pair(4, COLOR_RED, -1);
        init_pair(5, COLOR_BLACK, COLOR_CYAN);
        init_pair(6, COLOR_MAGENTA, -1);
        init_pair(7, COLOR_WHITE, -1);
    }
}

void draw_ui(void) {
    erase();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // 1. Storage & Inode Calculations
    struct statvfs vfs_home;
    statvfs(g_state.current_dir, &vfs_home);
    unsigned long long h_total = (unsigned long long)vfs_home.f_blocks * vfs_home.f_frsize;
    unsigned long long h_free = (unsigned long long)vfs_home.f_bfree * vfs_home.f_frsize;
    unsigned long long h_used = (h_total > h_free) ? (h_total - h_free) : 0;
    double h_pct = h_total ? ((double)h_used / (double)h_total) * 100.0 : 0.0;

    unsigned long long ino_tot = vfs_home.f_files;
    unsigned long long ino_free = vfs_home.f_ffree;
    unsigned long long ino_used = (ino_tot > ino_free) ? (ino_tot - ino_free) : 0;
    double ino_pct = ino_tot ? ((double)ino_used / (double)ino_tot) * 100.0 : 0.0;

    char base_goinfre[PATH_MAX_LEN];
    get_goinfre_path(base_goinfre, sizeof(base_goinfre));
    struct statvfs vfs_g;
    int has_goinfre = (statvfs("/goinfre", &vfs_g) == 0 || statvfs("/sgoinfre", &vfs_g) == 0);
    double g_pct = 0.0;
    unsigned long long g_free = 0;
    if (has_goinfre) {
        unsigned long long g_tot = (unsigned long long)vfs_g.f_blocks * vfs_g.f_frsize;
        g_free = (unsigned long long)vfs_g.f_bfree * vfs_g.f_frsize;
        unsigned long long g_used = (g_tot > g_free) ? (g_tot - g_free) : 0;
        g_pct = g_tot ? ((double)g_used / (double)g_tot) * 100.0 : 0.0;
    }

    // Top Header Banner
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 1, " 42/1337 CLUSTER STORAGE SUITE ");
    attroff(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 34, "[ User: %s | Dir: %.45s ]", g_state.username, g_state.current_dir);

    // Gauges: Home Storage, Inodes, and Goinfre
    char gauge_h[64];
    render_gauge(gauge_h, sizeof(gauge_h), h_pct, 12);
    int h_col = (h_pct > 85.0) ? 4 : ((h_pct > 70.0) ? 3 : 2);

    attron(COLOR_PAIR(h_col) | A_BOLD);
    mvprintw(1, 1, " HOME: %s", gauge_h);
    attroff(COLOR_PAIR(h_col) | A_BOLD);

    attron(COLOR_PAIR(ino_pct > 80.0 ? 4 : 7) | A_BOLD);
    mvprintw(1, 27, " INODES: %llu/%llu (%.0f%%)", ino_used, ino_tot, ino_pct);
    attroff(COLOR_PAIR(ino_pct > 80.0 ? 4 : 7) | A_BOLD);

    if (has_goinfre) {
        char gauge_g[64];
        char sz_g_free[16];
        format_size(g_free, sz_g_free, sizeof(sz_g_free));
        render_gauge(gauge_g, sizeof(gauge_g), g_pct, 12);
        attron(COLOR_PAIR(6) | A_BOLD);
        mvprintw(1, 55, " GOINFRE: %s (%s free)", gauge_g, sz_g_free);
        attroff(COLOR_PAIR(6) | A_BOLD);
    }

    mvhline(2, 0, ACS_HLINE, max_x);

    // Layout Split
    int split_x = (max_x * 63) / 100;
    int list_h = max_y - 6;
    if (list_h < 1) list_h = 1;

    // Left Pane (File Table + Graph Bar)
    for (int i = 0; i < list_h; i++) {
        int idx = g_state.scroll_offset + i;
        if (idx >= g_state.filtered_count) break;

        FileEntry *fe = &g_state.filtered[idx];
        char sz_str[16];
        if (fe->type == TYPE_LINK) safe_str_copy(sz_str, "  LINK ", sizeof(sz_str));
        else format_size(fe->size, sz_str, sizeof(sz_str));

        char graph_str[32];
        render_graph_bar(graph_str, sizeof(graph_str), fe->size, g_state.max_item_size, 8);

        const char *mark_tag = fe->marked ? "[*]" : "   ";
        const char *type_tag = (fe->type == TYPE_DIR ? "DIR" : (fe->type == TYPE_LINK ? "LNK" : "FIL"));

        if (idx == g_state.selected) {
            attron(COLOR_PAIR(5) | A_BOLD);
            mvprintw(3 + i, 1, "%s %-3s %s %s %-*.*s ",
                     mark_tag, type_tag, sz_str, graph_str,
                     split_x - 34, split_x - 34, fe->name);
            attroff(COLOR_PAIR(5) | A_BOLD);
        } else {
            if (fe->marked) attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(3 + i, 1, "%s", mark_tag);
            if (fe->marked) wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);

            int type_color = (fe->type == TYPE_DIR) ? 1 : (fe->type == TYPE_LINK ? 6 : 7);
            attron(COLOR_PAIR(type_color) | A_BOLD);
            printw(" %-3s", type_tag);
            attroff(COLOR_PAIR(type_color) | A_BOLD);

            printw(" %s %s %-*.*s", sz_str, graph_str, split_x - 34, split_x - 34, fe->name);
        }
    }

    // Vertical Divider
    for (int y = 3; y < max_y - 3; y++) {
        mvaddch(y, split_x, ACS_VLINE);
    }

    // Right Pane (Inspector & Controls)
    int rx = split_x + 2;
    int r_width = max_x - split_x - 4;

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(3, rx, ":: [ ITEM INSPECTOR ] ::");
    attroff(COLOR_PAIR(1) | A_BOLD);

    if (g_state.filtered_count > 0 && g_state.selected < g_state.filtered_count) {
        FileEntry *cur = &g_state.filtered[g_state.selected];
        char perms[16];
        format_permissions(cur->mode, perms);

        char time_str[32];
        struct tm *tm_info = localtime(&cur->mtime);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);

        char exact_sz[32];
        snprintf(exact_sz, sizeof(exact_sz), "%lld bytes", (long long)cur->size);

        mvprintw(5, rx, "Name:  %.*s", r_width - 8, cur->name);
        mvprintw(6, rx, "Type:  %s | Items: %zu", (cur->type == TYPE_DIR ? "Directory" : (cur->type == TYPE_LINK ? "Symlink" : "File")), cur->items_count);
        mvprintw(7, rx, "Size:  %s", exact_sz);
        mvprintw(8, rx, "Perms: %s", perms);
        mvprintw(9, rx, "MTime: %s", time_str);
    }

    mvhline(10, split_x, ACS_HLINE, max_x - split_x);

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(11, rx, ":: [ CLUSTER ACTIONS ] ::");
    attroff(COLOR_PAIR(1) | A_BOLD);

    mvprintw(13, rx, "Space  : Mark Item (Batch)");
    mvprintw(14, rx, "s      : Move & Symlink to /goinfre");
    mvprintw(15, rx, "H      : Station-Switch Symlink Healer");
    mvprintw(16, rx, "b      : Bootstrap AI/ML/Docker/Rust");
    mvprintw(17, rx, "K      : Nuke Python, C & Build Junk");
    mvprintw(18, rx, "G      : Git Clean & Pack Doctor");
    mvprintw(19, rx, "D      : Docker System Prune");
    mvprintw(20, rx, "e / t  : Edit (nvim) / Subshell");
    mvprintw(21, rx, "?      : Full Keymap Help");

    // Footer Bar
    mvhline(max_y - 3, 0, ACS_HLINE, max_x);

    if (g_state.is_searching) {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(max_y - 2, 2, "SEARCH: %s_", g_state.search_query);
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else if (g_state.search_query[0] != '\0') {
        attron(COLOR_PAIR(3));
        mvprintw(max_y - 2, 2, "FILTER: [%s] (Press / to edit, ESC to clear)", g_state.search_query);
        attroff(COLOR_PAIR(3));
    } else {
        char total_sz_str[16];
        format_size(g_state.total_dir_size, total_sz_str, sizeof(total_sz_str));
        int marked = count_marked_items();
        if (marked > 0) {
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(max_y - 2, 2, "Marked: %d items | Total Dir Size: %s | Sort: %s",
                     marked, total_sz_str, (g_state.sort_mode == SORT_SIZE_DESC ? "Size v" : "Name a-z"));
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else {
            mvprintw(max_y - 2, 2, "Items: %d (%s) | Sort: %s | '?' Help | 'q' Quit",
                     g_state.filtered_count, total_sz_str, (g_state.sort_mode == SORT_SIZE_DESC ? "Size v" : "Name a-z"));
        }
    }

    refresh();
}
