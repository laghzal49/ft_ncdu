#include "ft_ncdu.h"

static const char *spinner_frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};

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

    // Top Header Banner with Breadcrumbs
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 1, " %s ", APP_TITLE);
    attroff(COLOR_PAIR(1) | A_BOLD);

    if (g_state.is_scanning) {
        g_state.spinner_frame = (g_state.spinner_frame + 1) % 10;
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 32, "[ %s SCANNING... ]", spinner_frames[g_state.spinner_frame]);
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else {
        char breadcrumbs[PATH_MAX_LEN] = {0};
        format_breadcrumbs(g_state.current_dir, breadcrumbs, max_x - 34);
        mvprintw(0, 32, "[ %s ]", breadcrumbs);
    }

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
    pthread_mutex_lock(&g_state.lock);
    if (g_state.filtered_count == 0) {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(4, 2, "[ Empty directory or no items match current filter ]");
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else {
        for (int i = 0; i < list_h; i++) {
            int idx = g_state.scroll_offset + i;
            if (idx >= g_state.filtered_count) break;

            FileEntry *fe = &g_state.filtered[idx];
            char sz_str[16];
            off_t effective_size = (g_state.size_mode == SIZE_ACTUAL_DISK) ? fe->disk_size : fe->size;

            if (fe->type == TYPE_LINK) safe_str_copy(sz_str, "  LINK ", sizeof(sz_str));
            else format_size(effective_size, sz_str, sizeof(sz_str));

            char graph_str[32];
            render_graph_bar(graph_str, sizeof(graph_str), effective_size, g_state.max_item_size, 8);

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
    }
    pthread_mutex_unlock(&g_state.lock);

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

    pthread_mutex_lock(&g_state.lock);
    if (g_state.filtered_count > 0 && g_state.selected < g_state.filtered_count) {
        FileEntry *cur = &g_state.filtered[g_state.selected];
        char perms[16];
        format_permissions(cur->mode, perms);

        char time_str[32];
        struct tm *tm_info = localtime(&cur->mtime);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);

        char exact_sz[32];
        off_t eff_sz = (g_state.size_mode == SIZE_ACTUAL_DISK) ? cur->disk_size : cur->size;
        snprintf(exact_sz, sizeof(exact_sz), "%lld bytes (%s)", (long long)eff_sz,
                 (g_state.size_mode == SIZE_ACTUAL_DISK) ? "Disk" : "Apparent");

        mvprintw(5, rx, "Name:  %.*s", r_width - 8, cur->name);
        mvprintw(6, rx, "Type:  %s | Items: %zu", (cur->type == TYPE_DIR ? "Directory" : (cur->type == TYPE_LINK ? "Symlink" : "File")), cur->items_count);
        mvprintw(7, rx, "Size:  %s", exact_sz);
        mvprintw(8, rx, "Perms: %s", perms);
        mvprintw(9, rx, "MTime: %s", time_str);
    }
    pthread_mutex_unlock(&g_state.lock);

    mvhline(10, split_x, ACS_HLINE, max_x - split_x);

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(11, rx, ":: [ ACTIONS ] ::");
    attroff(COLOR_PAIR(1) | A_BOLD);

    mvprintw(13, rx, "Space  : Multi-Select Mark");
    mvprintw(14, rx, "s      : Link to /goinfre");
    mvprintw(15, rx, "H      : Symlink Healer");
    mvprintw(16, rx, "b      : Bootstrap AI/ML/Tools");
    mvprintw(17, rx, "Z      : Inject ~/.zshrc Exports");
    mvprintw(18, rx, "C      : Clean Presets Menu");
    mvprintw(19, rx, "p / P  : Peek File / Goto Path");
    mvprintw(20, rx, "A / a  : Toggle Disk Size/Hidden");
    mvprintw(21, rx, "?      : Command Reference");

    // Footer Bar
    mvhline(max_y - 3, 0, ACS_HLINE, max_x);

    const char *sort_str = "Size (Desc)";
    if (g_state.sort_mode == SORT_SIZE_ASC) sort_str = "Size (Asc)";
    else if (g_state.sort_mode == SORT_NAME_ASC) sort_str = "Name (A-Z)";
    else if (g_state.sort_mode == SORT_MTIME_DESC) sort_str = "Date (Newest)";

    const char *size_mode_str = (g_state.size_mode == SIZE_ACTUAL_DISK) ? "Disk-Blocks" : "Apparent";
    const char *hidden_str = g_state.show_hidden ? "All" : "Clean";

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
        off_t eff_tot = (g_state.size_mode == SIZE_ACTUAL_DISK) ? g_state.total_disk_usage : g_state.total_dir_size;
        format_size(eff_tot, total_sz_str, sizeof(total_sz_str));
        int marked = count_marked_items();

        if (g_state.unreadable_count > 0) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(max_y - 2, 2, "[EACCES: %zu unreadable dirs] ", g_state.unreadable_count);
            attroff(COLOR_PAIR(4) | A_BOLD);
        }

        if (marked > 0) {
            attron(COLOR_PAIR(3) | A_BOLD);
            printw("Marked: %d items | Total: %s | Sort: %s | [%s|%s]",
                   marked, total_sz_str, sort_str, size_mode_str, hidden_str);
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else {
            printw("Items: %d (%s) | Sort: %s | Mode: [%s|%s] | '?' Help | 'q' Quit",
                   g_state.filtered_count, total_sz_str, sort_str, size_mode_str, hidden_str);
        }
    }

    refresh();
}
