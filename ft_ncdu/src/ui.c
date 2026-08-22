#include "ft_ncdu.h"

static const char *spinner_frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};

void init_ui_colors(void) {
    if (has_colors()) {
        start_color();
        use_default_colors();

        /* Palette Definition */
        init_pair(1, COLOR_CYAN, -1);           /* Accents, Titles, Directory text */
        init_pair(2, COLOR_GREEN, -1);          /* Success, Healthy gauges, Links */
        init_pair(3, COLOR_YELLOW, -1);         /* Warnings, Marks, Graph bars */
        init_pair(4, COLOR_RED, -1);            /* Critical alerts, Errors, Dead links */
        init_pair(5, COLOR_BLACK, COLOR_CYAN);  /* Active cursor bar */
        init_pair(6, COLOR_MAGENTA, -1);        /* Symlink text & paths */
        init_pair(7, COLOR_WHITE, -1);          /* Default files & values */
        init_pair(8, COLOR_BLACK, COLOR_GREEN); /* Badge: [DIR ] */
        init_pair(9, COLOR_BLACK, COLOR_MAGENTA);/* Badge: [LINK] */
        init_pair(10, COLOR_WHITE, COLOR_BLUE); /* Badge: [FILE] */
        init_pair(11, COLOR_BLACK, COLOR_WHITE);/* Powerline neutral */
        init_pair(12, COLOR_WHITE, COLOR_RED);  /* Danger / [DEAD] Badge */
        init_pair(13, COLOR_BLACK, COLOR_YELLOW);/* Warning badge / Mark highlight */
        init_pair(14, COLOR_CYAN, COLOR_BLACK); /* Section Headers */
    }
}

static void draw_box(int y, int x, int h, int w, const char *title, int color_pair) {
    if (h < 2 || w < 2) return;
    attron(COLOR_PAIR(color_pair));

    /* Top border */
    mvaddstr(y, x, "╭");
    for (int i = 1; i < w - 1; i++) mvaddstr(y, x + i, "─");
    mvaddstr(y, x + w - 1, "╮");

    /* Vertical sides */
    for (int i = 1; i < h - 1; i++) {
        mvaddstr(y + i, x, "│");
        mvaddstr(y + i, x + w - 1, "│");
    }

    /* Bottom border */
    mvaddstr(y + h - 1, x, "╰");
    for (int i = 1; i < w - 1; i++) mvaddstr(y + h - 1, x + i, "─");
    mvaddstr(y + h - 1, x + w - 1, "╯");

    /* Title banner */
    if (title && title[0] != '\0') {
        int tlen = strlen(title);
        if (tlen + 4 < w) {
            attron(A_BOLD);
            mvprintw(y, x + 2, " %s ", title);
            wattroff(stdscr, A_BOLD);
        }
    }
    attroff(COLOR_PAIR(color_pair));
}

void draw_ui(void) {
    erase();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    /* 1. Storage & Filesystem Telemetry */
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

    /* 2. Top Powerline Header Bar */
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(0, 0, " 󰣇 1337 | 42 ");
    attroff(COLOR_PAIR(5) | A_BOLD);

    attron(COLOR_PAIR(11));
    printw(" %s v%s ", APP_NAME, APP_VERSION);
    attroff(COLOR_PAIR(11));

    if (g_state.is_scanning) {
        g_state.spinner_frame = (g_state.spinner_frame + 1) % 10;
        attron(COLOR_PAIR(3) | A_BOLD);
        printw(" %s SCANNING DIRECTORY TREE... ", spinner_frames[g_state.spinner_frame]);
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else {
        char breadcrumbs[PATH_MAX_LEN] = {0};
        format_breadcrumbs(g_state.current_dir, breadcrumbs, max_x - 44);
        attron(COLOR_PAIR(1) | A_BOLD);
        printw(" 📁 %s ", breadcrumbs);
        attroff(COLOR_PAIR(1) | A_BOLD);
    }

    /* 3. High-Density Telemetry HUD Cards (3-column layout) */
    int card_w = (max_x - 4) / 3;
    if (card_w < 20) card_w = 20;

    /* Card 1: HOME QUOTA */
    draw_box(1, 0, 3, card_w + 1, "🏠 HOME QUOTA", 1);
    char gauge_h[64];
    render_gauge(gauge_h, sizeof(gauge_h), h_pct, card_w > 30 ? 10 : 6);
    int h_col = (h_pct > 85.0) ? 4 : ((h_pct > 70.0) ? 3 : 2);
    attron(COLOR_PAIR(h_col) | A_BOLD);
    mvprintw(2, 2, "%.*s", card_w - 2, gauge_h);
    attroff(COLOR_PAIR(h_col) | A_BOLD);

    /* Card 2: INODES */
    draw_box(1, card_w + 1, 3, card_w + 1, "🧬 INODES & OBJECTS", 1);
    char gauge_ino[64];
    render_gauge(gauge_ino, sizeof(gauge_ino), ino_pct, card_w > 30 ? 10 : 6);
    attron(COLOR_PAIR(ino_pct > 80.0 ? 4 : 7));
    mvprintw(2, card_w + 3, "%.*s", card_w - 2, gauge_ino);
    attroff(COLOR_PAIR(ino_pct > 80.0 ? 4 : 7));

    /* Card 3: GOINFRE */
    int card3_x = (card_w + 1) * 2;
    draw_box(1, card3_x, 3, max_x - card3_x, "⚡ GOINFRE NVMe POOL", 1);
    if (has_goinfre) {
        char sz_g_free[16];
        format_size(g_free, sz_g_free, sizeof(sz_g_free));
        char gauge_g[64];
        render_gauge(gauge_g, sizeof(gauge_g), g_pct, 8);
        attron(COLOR_PAIR(6) | A_BOLD);
        mvprintw(2, card3_x + 2, "%s (%s Free)", gauge_g, sz_g_free);
        attroff(COLOR_PAIR(6) | A_BOLD);
    } else {
        attron(COLOR_PAIR(7));
        mvprintw(2, card3_x + 2, "/tmp Local Fallback");
        attroff(COLOR_PAIR(7));
    }

    /* 4. Split Pane Geometry */
    int split_x = (max_x * 58) / 100;
    int body_y = 4;
    int body_h = max_y - body_y - 2;
    if (body_h < 4) body_h = 4;

    /* Left Table Box */
    draw_box(body_y, 0, body_h, split_x, "CLUSTER FILE EXPLORER", 1);

    /* Right Inspector Box */
    draw_box(body_y, split_x, body_h, max_x - split_x, "TARGET INSPECTOR & ACTION DECK", 1);

    /* Table Column Header */
    attron(COLOR_PAIR(14) | A_BOLD);
    mvprintw(body_y + 1, 2, "ST  TYPE     SIZE     ALLOCATION %%       NAME");
    attroff(COLOR_PAIR(14) | A_BOLD);

    /* 5. Render File Table */
    int list_h = body_h - 3;
    pthread_mutex_lock(&g_state.lock);

    if (g_state.filtered_count == 0) {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(body_y + 3, 4, "─ No items match current query or filter ─");
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else {
        for (int i = 0; i < list_h; i++) {
            int idx = g_state.scroll_offset + i;
            if (idx >= g_state.filtered_count) break;

            FileEntry *fe = &g_state.filtered[idx];
            char sz_str[16];
            off_t effective_size = (g_state.size_mode == SIZE_ACTUAL_DISK) ? fe->disk_size : fe->size;

            if (fe->type == TYPE_LINK) {
                if (fe->is_broken_link) safe_str_copy(sz_str, "  DEAD ", sizeof(sz_str));
                else safe_str_copy(sz_str, "  LINK ", sizeof(sz_str));
            } else {
                format_size(effective_size, sz_str, sizeof(sz_str));
            }

            char graph_str[32];
            render_graph_bar(graph_str, sizeof(graph_str), effective_size, g_state.max_item_size, 8);

            const char *mark_sym = fe->marked ? "✔" : " ";
            int row_y = body_y + 2 + i;

            int badge_pair = 10;
            const char *badge_str = "FILE";
            const char *type_icon = "📄";
            if (fe->type == TYPE_DIR) {
                badge_pair = 8;
                badge_str = "DIR ";
                type_icon = "📁";
            } else if (fe->type == TYPE_LINK) {
                if (fe->is_broken_link) {
                    badge_pair = 12;
                    badge_str = "DEAD";
                    type_icon = "💀";
                } else {
                    badge_pair = 9;
                    badge_str = "LINK";
                    type_icon = "🔗";
                }
            }

            if (idx == g_state.selected) {
                /* Active Cursor Row */
                attron(COLOR_PAIR(5) | A_BOLD);
                mvprintw(row_y, 1, " ❯ %s ", mark_sym);

                attron(COLOR_PAIR(badge_pair));
                printw(" %s ", badge_str);
                attroff(COLOR_PAIR(badge_pair));

                attron(COLOR_PAIR(5) | A_BOLD);
                printw(" %s %s %s %-*.*s", sz_str, graph_str, type_icon, split_x - 40, split_x - 40, fe->name);
                attroff(COLOR_PAIR(5) | A_BOLD);
            } else {
                /* Inactive Rows */
                if (fe->marked) attron(COLOR_PAIR(3) | A_BOLD);
                mvprintw(row_y, 1, "   %s ", mark_sym);
                if (fe->marked) wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);

                attron(COLOR_PAIR(badge_pair));
                printw(" %s ", badge_str);
                attroff(COLOR_PAIR(badge_pair));

                int name_color = (fe->type == TYPE_DIR) ? 1 : (fe->is_broken_link ? 4 : (fe->type == TYPE_LINK ? 6 : 7));
                attron(COLOR_PAIR(7));
                printw(" %s ", sz_str);
                attroff(COLOR_PAIR(7));

                attron(COLOR_PAIR(3));
                printw("%s ", graph_str);
                attroff(COLOR_PAIR(3));

                printw("%s ", type_icon);

                attron(COLOR_PAIR(name_color));
                printw("%-*.*s", split_x - 40, split_x - 40, fe->name);
                attroff(COLOR_PAIR(name_color));
            }
        }
    }

    /* 6. Render Inspector & Action Deck */
    int rx = split_x + 2;
    int rw = max_x - split_x - 4;

    if (g_state.filtered_count > 0 && g_state.selected < g_state.filtered_count) {
        FileEntry *cur = &g_state.filtered[g_state.selected];
        char perms[16];
        format_permissions(cur->mode, perms);

        char time_str[32];
        struct tm *tm_info = localtime(&cur->mtime);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);

        char exact_sz[32];
        off_t eff_sz = (g_state.size_mode == SIZE_ACTUAL_DISK) ? cur->disk_size : cur->size;
        snprintf(exact_sz, sizeof(exact_sz), "%lld B", (long long)eff_sz);

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(body_y + 1, rx, "🎯 TARGET METADATA");
        attroff(COLOR_PAIR(1) | A_BOLD);

        mvprintw(body_y + 2, rx, "Name    : %.*s", rw - 10, cur->name);
        mvprintw(body_y + 3, rx, "Type    : %s (%zu items)", (cur->type == TYPE_DIR ? "Directory" : (cur->type == TYPE_LINK ? "Symlink" : "Regular File")), cur->items_count);
        mvprintw(body_y + 4, rx, "Size    : %s", exact_sz);
        mvprintw(body_y + 5, rx, "Perms   : %s (%04o)", perms, cur->mode & 0777);
        mvprintw(body_y + 6, rx, "Date    : %s", time_str);

        if (cur->type == TYPE_LINK) {
            if (cur->is_broken_link) {
                attron(COLOR_PAIR(4) | A_BOLD);
                mvprintw(body_y + 7, rx, "Link -> : %.*s [DEAD LINK!]", rw - 22, cur->symlink_target);
                attroff(COLOR_PAIR(4) | A_BOLD);
            } else {
                attron(COLOR_PAIR(2));
                mvprintw(body_y + 7, rx, "Link -> : %.*s [HEALTHY]", rw - 22, cur->symlink_target);
                attroff(COLOR_PAIR(2));
            }
        }
    }
    pthread_mutex_unlock(&g_state.lock);

    /* Divider inside Inspector */
    int mid_hud_y = body_y + 9;
    if (mid_hud_y < body_y + body_h - 11) {
        mvaddstr(mid_hud_y, split_x, "├");
        for (int i = 1; i < max_x - split_x - 1; i++) mvaddstr(mid_hud_y, split_x + i, "─");
        mvaddstr(mid_hud_y, max_x - 1, "┤");

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(mid_hud_y + 1, rx, "⚡ 42 CLUSTER COMMAND DECK");
        attroff(COLOR_PAIR(1) | A_BOLD);

        mvprintw(mid_hud_y + 2, rx, "[s] 󰌷 Link Goinfre    [u] 󰁌 Restore Home");
        mvprintw(mid_hud_y + 3, rx, "[H] 󰚌 Heal Station     [T] 󰩹 Empty Trash");
        mvprintw(mid_hud_y + 4, rx, "[C] 󰒃 Clean Presets    [b] 󰑓 Bootstrap Tools");
        mvprintw(mid_hud_y + 5, rx, "[p] 󰈈 Scroll Peek      [Z] 󱆃 Inject .zshrc");
        mvprintw(mid_hud_y + 6, rx, "[A] 󰋊 Size Mode        [a] 󰈉 Dotfiles Toggle");
        mvprintw(mid_hud_y + 7, rx, "[o] 🔃 Sort Mode       [E] 󰈚 Export Report");
    }

    /* 7. Bottom Status Powerline Footer */
    int foot_y = max_y - 1;
    const char *sort_str = (g_state.sort_mode == SORT_SIZE_DESC) ? "Size v" :
                           (g_state.sort_mode == SORT_SIZE_ASC ? "Size ^" :
                           (g_state.sort_mode == SORT_NAME_ASC ? "Name" : "Date"));

    const char *size_mode_str = (g_state.size_mode == SIZE_ACTUAL_DISK) ? "DISK" : "APPARENT";
    const char *hidden_str = g_state.show_hidden ? "ALL" : "CLEAN";

    if (g_state.is_searching) {
        attron(COLOR_PAIR(10) | A_BOLD);
        mvprintw(foot_y, 0, " SEARCH ");
        attroff(COLOR_PAIR(10) | A_BOLD);
        attron(COLOR_PAIR(3) | A_BOLD);
        printw(" %s_ (ESC to clear, Enter to apply)", g_state.search_query);
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else {
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(foot_y, 0, " NORMAL ");
        attroff(COLOR_PAIR(5) | A_BOLD);

        char total_sz_str[16];
        off_t eff_tot = (g_state.size_mode == SIZE_ACTUAL_DISK) ? g_state.total_disk_usage : g_state.total_dir_size;
        format_size(eff_tot, total_sz_str, sizeof(total_sz_str));
        int marked = count_marked_items();

        attron(COLOR_PAIR(7));
        printw(" Items: %d (%s) │ Sort: %s │ Mode: [%s|%s] ",
               g_state.filtered_count, total_sz_str, sort_str, size_mode_str, hidden_str);
        attroff(COLOR_PAIR(7));

        if (marked > 0) {
            attron(COLOR_PAIR(3) | A_BOLD);
            printw("│ Marked: %d items ", marked);
            attroff(COLOR_PAIR(3) | A_BOLD);
        }

        if (g_state.broken_links_count > 0) {
            attron(COLOR_PAIR(12) | A_BOLD);
            printw(" [DEAD LINKS: %zu - PRESS 'H'] ", g_state.broken_links_count);
            attroff(COLOR_PAIR(12) | A_BOLD);
        } else if (g_state.unreadable_count > 0) {
            attron(COLOR_PAIR(12) | A_BOLD);
            printw(" [EACCES: %zu] ", g_state.unreadable_count);
            attroff(COLOR_PAIR(12) | A_BOLD);
        }

        attron(COLOR_PAIR(11));
        mvprintw(foot_y, max_x - 14, " '?' For Help ");
        attroff(COLOR_PAIR(11));
    }

    refresh();
}


