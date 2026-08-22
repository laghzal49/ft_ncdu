#include "ft_ncdu.h"

void get_goinfre_path(char *dest, size_t dest_len) {
    if (access("/goinfre", F_OK) == 0) {
        snprintf(dest, dest_len, "/goinfre/%s", g_state.username);
    } else if (access("/sgoinfre", F_OK) == 0) {
        snprintf(dest, dest_len, "/sgoinfre/%s", g_state.username);
    } else {
        snprintf(dest, dest_len, "/tmp/goinfre_%s", g_state.username);
    }
}

int confirm_modal(const char *title, const char *message) {
    int h = 9, w = 68;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ %s ] ::", title);
    wattroff(win, COLOR_PAIR(4) | A_BOLD);

    mvwprintw(win, 3, 2, " %s", message);
    wattron(win, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(win, 6, 2, " [Y] Confirm Action  |  [N]/[ESC] Abort");
    wattroff(win, COLOR_PAIR(2) | A_BOLD);

    wrefresh(win);
    wtimeout(win, -1);
    int ch = wgetch(win);
    delwin(win);
    return (ch == 'y' || ch == 'Y');
}

void show_help_modal(void) {
    int h = 24, w = 76;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ %s COMMAND REFERENCE ] ::", APP_TITLE);
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    mvwprintw(win, 3, 2,  " j / k / Arrows : Navigate Up / Down");
    mvwprintw(win, 4, 2,  " l / Enter      : Step into directory");
    mvwprintw(win, 5, 2,  " h / Backspace  : Step out to parent directory");
    mvwprintw(win, 6, 2,  " Space          : Toggle selection mark (Batch Mode)");
    mvwprintw(win, 7, 2,  " dd / x         : Delete highlighted (or marked) items");
    mvwprintw(win, 8, 2,  " s              : Move & Symlink target to /goinfre");
    mvwprintw(win, 9, 2,  " H              : Station-Switch Symlink Healer (Repair dangling)");
    mvwprintw(win, 10, 2, " b              : Bootstrap Toolchains & Models to goinfre");
    mvwprintw(win, 11, 2, " Z              : Inject quota bypass environment variables into ~/.zshrc");
    mvwprintw(win, 12, 2, " C              : Clean Presets Menu (C/C++, AI, Web, Nuclear)");
    mvwprintw(win, 13, 2, " K              : Quick Nuke build junk, caches & core dumps");
    mvwprintw(win, 14, 2, " G              : Git Doctor (clean -fdx & aggressive repack)");
    mvwprintw(win, 15, 2, " D              : Docker System Prune (-a --volumes)");
    mvwprintw(win, 16, 2, " p              : Quick Preview / Peek file contents");
    mvwprintw(win, 17, 2, " P / :          : Teleport / Goto path jump prompt");
    mvwprintw(win, 18, 2, " A              : Toggle Apparent Size vs. Actual Disk Allocation");
    mvwprintw(win, 19, 2, " a              : Toggle Dotfiles visibility (Show/Hide hidden)");
    mvwprintw(win, 20, 2, " o              : Cycle Sort Mode (Size v / Size ^ / Name / Date)");
    mvwprintw(win, 21, 2, " /              : Fuzzy search / filter (ESC to clear)");

    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 22, 2, " Press any key to close help...");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);

    wrefresh(win);
    wtimeout(win, -1);
    wgetch(win);
    delwin(win);
}

void action_delete(void) {
    if (g_state.filtered_count == 0) return;

    int marked = count_marked_items();
    if (marked > 0) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Permanently delete ALL %d marked items?", marked);
        if (confirm_modal("BATCH DELETE", msg)) {
            for (int i = 0; i < g_state.filtered_count; i++) {
                if (g_state.filtered[i].marked) {
                    if (is_protected_target(g_state.filtered[i].path)) continue;
                    char *cmd = NULL;
                    if (asprintf(&cmd, "rm -rf '%s'", g_state.filtered[i].path) != -1) {
                        int ret = system(cmd);
                        (void)ret;
                        free(cmd);
                    }
                }
            }
            start_async_scan(g_state.current_dir);
        }
    } else {
        FileEntry *target = &g_state.filtered[g_state.selected];
        if (is_protected_target(target->path)) {
            confirm_modal("BLOCKED", "Cannot delete protected system configuration file!");
            return;
        }
        char msg[128];
        snprintf(msg, sizeof(msg), "Permanently delete: %.38s?", target->name);
        if (confirm_modal("DELETE TARGET", msg)) {
            char *cmd = NULL;
            if (asprintf(&cmd, "rm -rf '%s'", target->path) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            start_async_scan(g_state.current_dir);
        }
    }
}

void action_symlink_goinfre(void) {
    if (g_state.filtered_count == 0) return;

    char base_goinfre[PATH_MAX_LEN];
    get_goinfre_path(base_goinfre, sizeof(base_goinfre));

    int marked = count_marked_items();
    if (marked > 0) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Move %d marked directories to /goinfre & symlink?", marked);
        if (confirm_modal("BATCH GOINFRE LINK", msg)) {
            for (int i = 0; i < g_state.filtered_count; i++) {
                if (g_state.filtered[i].marked && g_state.filtered[i].type == TYPE_DIR) {
                    char *dest = NULL;
                    if (asprintf(&dest, "%s/%s", base_goinfre, g_state.filtered[i].name) != -1) {
                        char *cmd = NULL;
                        if (asprintf(&cmd, "mkdir -p '%s' && rm -rf '%s' && mv '%s' '%s' && ln -s '%s' '%s'",
                                     base_goinfre, dest, g_state.filtered[i].path, dest, dest, g_state.filtered[i].path) != -1) {
                            int ret = system(cmd);
                            (void)ret;
                            free(cmd);
                        }
                        free(dest);
                    }
                }
            }
            start_async_scan(g_state.current_dir);
        }
    } else {
        FileEntry *target = &g_state.filtered[g_state.selected];
        if (target->type != TYPE_DIR) return;

        char *dest_path = NULL;
        if (asprintf(&dest_path, "%s/%s", base_goinfre, target->name) == -1) return;

        if (confirm_modal("GOINFRE SYMLINK", "Move folder to /goinfre & create symlink?")) {
            char *cmd = NULL;
            if (asprintf(&cmd, "mkdir -p '%s' && rm -rf '%s' && mv '%s' '%s' && ln -s '%s' '%s'",
                         base_goinfre, dest_path, target->path, dest_path, dest_path, target->path) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            start_async_scan(g_state.current_dir);
        }
        free(dest_path);
    }
}

void action_bootstrap_goinfre(void) {
    char base_goinfre[PATH_MAX_LEN];
    get_goinfre_path(base_goinfre, sizeof(base_goinfre));

    if (confirm_modal("BOOTSTRAP TOOLCHAINS", "Link configured caches & models to /goinfre?")) {
        const char *home = getenv("HOME");
        if (!home) return;

        char *cmd = NULL;
        if (asprintf(&cmd, "mkdir -p '%s'", base_goinfre) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }

        for (size_t i = 0; G_BOOTSTRAP_TARGETS[i] != NULL; i++) {
            char *src = NULL;
            char *dst = NULL;
            if (asprintf(&src, "%s/%s", home, G_BOOTSTRAP_TARGETS[i]) != -1 &&
                asprintf(&dst, "%s/%s", base_goinfre, G_BOOTSTRAP_TARGETS[i]) != -1) {
                struct stat st;
                if (lstat(src, &st) == 0 && !S_ISLNK(st.st_mode)) {
                    if (asprintf(&cmd, "mkdir -p $(dirname '%s') && rm -rf '%s' && mv '%s' '%s' && ln -s '%s' '%s'",
                                 dst, dst, src, dst, dst, src) != -1) {
                        int r = system(cmd);
                        (void)r;
                        free(cmd);
                    }
                } else if (lstat(src, &st) != 0) {
                    if (asprintf(&cmd, "mkdir -p '%s' && mkdir -p $(dirname '%s') && ln -s '%s' '%s'",
                                 dst, src, dst, src) != -1) {
                        int r = system(cmd);
                        (void)r;
                        free(cmd);
                    }
                }
                free(src);
                free(dst);
            }
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_heal_symlinks(void) {
    const char *home = getenv("HOME");
    if (!home) return;

    if (confirm_modal("SYMLINK HEALER", "Scan and repair broken /goinfre links for this workstation?")) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find '%s' -maxdepth 3 -type l -exec sh -c '"
                     "for link; do "
                     "  target=$(readlink \"$link\"); "
                     "  case \"$target\" in /goinfre/*|/sgoinfre/*) "
                     "    if [ ! -e \"$link\" ]; then "
                     "      mkdir -p \"$target\"; "
                     "    fi;; "
                     "  esac; "
                     "done' sh {} + 2>/dev/null", home) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_inject_zshrc(void) {
    const char *home = getenv("HOME");
    if (!home) return;

    char zshrc_path[PATH_MAX_LEN];
    snprintf(zshrc_path, sizeof(zshrc_path), "%s/.zshrc", home);

    if (confirm_modal("INJECT ~/.zshrc EXPORTS", "Append goinfre cache redirect exports to your ~/.zshrc?")) {
        FILE *fp = fopen(zshrc_path, "a");
        if (!fp) {
            confirm_modal("ERROR", "Could not open ~/.zshrc for writing.");
            return;
        }

        fprintf(fp, "\n# >>> 1337 Goinfre Quota Bypass >>>\n");
        for (size_t i = 0; G_ZSHRC_EXPORTS[i] != NULL; i++) {
            fprintf(fp, "%s\n", G_ZSHRC_EXPORTS[i]);
        }
        fprintf(fp, "# <<< 1337 Goinfre Quota Bypass <<<\n");
        fclose(fp);

        confirm_modal("SUCCESS", "Injected environment exports into ~/.zshrc! Reload with: source ~/.zshrc");
    }
}

void action_goto_path(void) {
    echo();
    curs_set(1);
    char input_buf[PATH_MAX_LEN] = {0};

    int h = 7, w = 70;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ TELEPORT / JUMP TO PATH ] ::");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    mvwprintw(win, 3, 2, "Enter Path: ");
    wrefresh(win);
    wtimeout(win, -1);

    wgetnstr(win, input_buf, sizeof(input_buf) - 1);
    noecho();
    curs_set(0);
    delwin(win);

    if (strlen(input_buf) > 0) {
        char resolved[PATH_MAX_LEN];
        if (input_buf[0] == '~') {
            const char *home = getenv("HOME");
            snprintf(resolved, sizeof(resolved), "%s%s", home ? home : "", input_buf + 1);
        } else {
            safe_str_copy(resolved, input_buf, sizeof(resolved));
        }

        char real_p[PATH_MAX_LEN];
        if (realpath(resolved, real_p)) {
            start_async_scan(real_p);
        } else {
            confirm_modal("ERROR", "Directory does not exist or permission denied.");
        }
    }
}

void action_file_peek(void) {
    if (g_state.filtered_count == 0) return;
    FileEntry *target = &g_state.filtered[g_state.selected];

    if (target->type == TYPE_DIR) {
        confirm_modal("INFO", "Target is a directory. Press 'l' or 'Enter' to open.");
        return;
    }

    int h = LINES - 6;
    int w = COLS - 10;
    if (h < 10) h = 10;
    if (w < 40) w = 40;

    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ PEEK: %.*s ] ::", w - 16, target->name);
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    FILE *fp = fopen(target->path, "r");
    if (!fp) {
        mvwprintw(win, 3, 2, "Error: Could not open file for reading.");
    } else {
        char line[512];
        int row = 3;
        while (fgets(line, sizeof(line), fp) && row < h - 2) {
            line[strcspn(line, "\r\n")] = 0;
            mvwprintw(win, row++, 2, "%.*s", w - 4, line);
        }
        fclose(fp);
    }

    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, h - 2, 2, "Press any key to close peek...");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);

    wrefresh(win);
    wtimeout(win, -1);
    wgetch(win);
    delwin(win);
}

void action_nuke_junk(void) {
    if (confirm_modal("NUKE CLUSTER JUNK", "Wipe Python/AI caches, node_modules, build bins (*.o, *.a, .dSYM)?")) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find '%s' -type d \\( -name \"node_modules\" -o -name \".cache\" -o -name \"target\" "
                     "-o -name \"*.dSYM\" -o -name \"__pycache__\" -o -name \".pytest_cache\" "
                     "-o -name \".mypy_cache\" -o -name \".ipynb_checkpoints\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
                     "find '%s' -type f \\( -name \"*.o\" -o -name \"*.a\" -o -name \"*.out\" -o -name \"*.pyc\" "
                     "-o -name \"*.pyo\" -o -name \".DS_Store\" -o -name \"core.*\" -o -name \"vgcore.*\" \\) -delete 2>/dev/null",
                     g_state.current_dir, g_state.current_dir) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_git_doctor(void) {
    if (confirm_modal("GIT REPO DOCTOR", "Run git clean -fdx & git gc across all repos in subtree?")) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find '%s' -name \".git\" -type d -execdir git clean -fdx \\; -execdir git gc --prune=now --aggressive \\; 2>/dev/null",
                     g_state.current_dir) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_docker_prune(void) {
    if (confirm_modal("DOCKER NUKE", "Execute docker system prune -a --volumes -f?")) {
        int ret = system("docker system prune -a --volumes -f >/dev/null 2>&1");
        (void)ret;
        start_async_scan(g_state.current_dir);
    }
}

void action_custom_command(void) {
    if (g_state.filtered_count == 0) return;
    FileEntry *target = &g_state.filtered[g_state.selected];

    echo();
    curs_set(1);
    char input_buf[512] = {0};

    int h = 7, w = 70;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ EXEC COMMAND ON TARGET ] ::");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    mvwprintw(win, 3, 2, "Target: %.45s", target->name);
    mvwprintw(win, 4, 2, "$ ");
    wrefresh(win);
    wtimeout(win, -1);

    wgetnstr(win, input_buf, sizeof(input_buf) - 1);
    noecho();
    curs_set(0);
    delwin(win);

    if (strlen(input_buf) > 0) {
        def_prog_mode();
        endwin();
        char *cmd = NULL;
        if (asprintf(&cmd, "%s '%s'", input_buf, target->path) != -1) {
            printf("\n\033[1;36m[ft_ncdu Exec]\033[0m Running: %s\n", cmd);
            int ret = system(cmd);
            (void)ret;
            free(cmd);
            printf("\nPress Enter to return...");
            getchar();
        }
        reset_prog_mode();
        refresh();
        start_async_scan(g_state.current_dir);
    }
}

void action_export_report(void) {
    FILE *fp = fopen("quota_report.md", "w");
    if (!fp) {
        confirm_modal("EXPORT ERROR", "Could not create quota_report.md in current directory.");
        return;
    }

    struct statvfs vfs;
    statvfs(g_state.current_dir, &vfs);
    unsigned long long total = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
    unsigned long long free_b = (unsigned long long)vfs.f_bfree * vfs.f_frsize;
    unsigned long long used = (total > free_b) ? (total - free_b) : 0;

    char sz_tot[16], sz_used[16], sz_free[16];
    format_size(total, sz_tot, sizeof(sz_tot));
    format_size(used, sz_used, sizeof(sz_used));
    format_size(free_b, sz_free, sizeof(sz_free));

    fprintf(fp, "# 1337 Cluster Storage Audit Report\n\n");
    fprintf(fp, "**Generated:** %ld | **User:** %s\n", time(NULL), g_state.username);
    fprintf(fp, "**Path:** `%s`\n\n", g_state.current_dir);
    fprintf(fp, "## Quota Overview\n\n");
    fprintf(fp, "| Storage | Total | Used | Free | Saturation |\n");
    fprintf(fp, "| :--- | :--- | :--- | :--- | :--- |\n");
    fprintf(fp, "| HOME | %s | %s | %s | %.1f%% |\n\n",
            sz_tot, sz_used, sz_free, total ? ((double)used / (double)total) * 100.0 : 0.0);

    fprintf(fp, "## Top Storage Consumers\n\n");
    fprintf(fp, "| Rank | Type | Size | Name |\n");
    fprintf(fp, "| :--- | :--- | :--- | :--- |\n");

    pthread_mutex_lock(&g_state.lock);
    for (int i = 0; i < g_state.count && i < 30; i++) {
        char sz[16];
        off_t item_sz = (g_state.size_mode == SIZE_ACTUAL_DISK) ?
                        g_state.entries[i].disk_size : g_state.entries[i].size;
        format_size(item_sz, sz, sizeof(sz));
        fprintf(fp, "| %02d | %s | %s | `%s` |\n",
                i + 1,
                (g_state.entries[i].type == TYPE_DIR ? "DIR" : "FILE"),
                sz,
                g_state.entries[i].name);
    }
    pthread_mutex_unlock(&g_state.lock);

    fclose(fp);
    confirm_modal("EXPORT SUCCESS", "Exported complete storage audit to quota_report.md!");
}

void action_cleaning_presets(void) {
    int h = (int)PRESET_COUNT + 8;
    int w = 72;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ 1337 CLEANING PRESETS ] ::");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    for (size_t i = 0; i < PRESET_COUNT; i++) {
        mvwprintw(win, 3 + (int)i, 2, "[%c] %-20s : %.42s",
                  G_CLEAN_PRESETS[i].key,
                  G_CLEAN_PRESETS[i].title,
                  G_CLEAN_PRESETS[i].desc);
    }

    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, h - 3, 2, "Select preset key or press [ESC] to cancel: ");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);

    wrefresh(win);
    wtimeout(win, -1);
    int ch = wgetch(win);
    delwin(win);

    for (size_t i = 0; i < PRESET_COUNT; i++) {
        if (ch == G_CLEAN_PRESETS[i].key) {
            char *cmd = NULL;
            if (asprintf(&cmd, G_CLEAN_PRESETS[i].command_fmt, g_state.current_dir, g_state.current_dir) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            start_async_scan(g_state.current_dir);
            break;
        }
    }
}

void action_edit(void) {
    if (g_state.filtered_count == 0) return;
    def_prog_mode();
    endwin();

    const char *editor = getenv("EDITOR");
    if (!editor) editor = "nvim";

    char *cmd = NULL;
    if (asprintf(&cmd, "%s '%s'", editor, g_state.filtered[g_state.selected].path) != -1) {
        int ret = system(cmd);
        (void)ret;
        free(cmd);
    }

    reset_prog_mode();
    refresh();
    start_async_scan(g_state.current_dir);
}

void action_shell(void) {
    def_prog_mode();
    endwin();

    const char *shell = getenv("SHELL");
    if (!shell) shell = "/bin/zsh";

    printf("\n\033[1;36m[ft_ncdu]\033[0m Subshell at %s (type 'exit' to return)...\n", g_state.current_dir);
    if (chdir(g_state.current_dir) == 0) {
        int ret = system(shell);
        (void)ret;
    }

    reset_prog_mode();
    refresh();
    start_async_scan(g_state.current_dir);
}
