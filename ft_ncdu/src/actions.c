#include "ft_ncdu.h"

void get_goinfre_path(char *dest, size_t dest_len) {
    if (access("/goinfre", F_OK) == 0) {
        snprintf(dest, dest_len, "/goinfre/%.128s", g_state.username);
    } else if (access("/sgoinfre", F_OK) == 0) {
        snprintf(dest, dest_len, "/sgoinfre/%.128s", g_state.username);
    } else if (access("/Volumes/Storage/goinfre", F_OK) == 0) {
        snprintf(dest, dest_len, "/Volumes/Storage/goinfre/%.128s", g_state.username);
    } else {
        const char *home = getenv("HOME");
        char local_g[PATH_MAX_LEN];
        if (home) {
            snprintf(local_g, sizeof(local_g), "%.2048s/goinfre", home);
            if (access(local_g, F_OK) == 0) {
                safe_str_copy(dest, local_g, dest_len);
                return;
            }
        }
        snprintf(dest, dest_len, "/tmp/goinfre_%.128s", g_state.username);
    }
}

int confirm_modal(const char *title, const char *message) {
    int h = 9, w = 70;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    if (y < 0) y = 0;
    if (x < 0) x = 0;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ %s ] ::", title);
    wattroff(win, COLOR_PAIR(4) | A_BOLD);

    mvwprintw(win, 3, 2, " %.*s", w - 4, message);
    wattron(win, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(win, 6, 2, " [Y] Confirm Action  │  [N] / [ESC] Abort");
    wattroff(win, COLOR_PAIR(2) | A_BOLD);

    wrefresh(win);
    wtimeout(win, -1);
    int ch = wgetch(win);
    delwin(win);
    return (ch == 'y' || ch == 'Y');
}

void show_help_modal(void) {
    int h = 26, w = 82;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    if (y < 0) y = 0;
    if (x < 0) x = 0;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ %s COMMAND REFERENCE ] ::", APP_TITLE);
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    /* Category 1: Navigation */
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 3, 2, " NAVIGATION & BROWSING");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 4, 4, "j / k / Arrows     : Up / Down       l / Enter / Right : Open Directory");
    mvwprintw(win, 5, 4, "h / Backspace / -  : Parent Dir      g / G             : Top / Bottom");
    mvwprintw(win, 6, 4, "~                  : Jump to HOME    P / :             : Teleport to Path");

    /* Category 2: 42 & 1337 Storage Actions */
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 8, 2, " 42 & 1337 CLUSTER STORAGE POWERS");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 9, 4, "s                  : Move target to /goinfre & create Symlink");
    mvwprintw(win, 10, 4, "u                  : Unlink from /goinfre (Restore back to HOME)");
    mvwprintw(win, 11, 4, "H                  : Station Healer (Repair dangling links across PCs)");
    mvwprintw(win, 12, 4, "b                  : Bootstrap AI / Rust / Docker / NPM to goinfre");
    mvwprintw(win, 13, 4, "T                  : Empty Desktop Trash (~/.local/share/Trash)");
    mvwprintw(win, 14, 4, "C                  : 42 Cleaning Presets (C/C++, Web, AI, Logs, Nuclear)");
    mvwprintw(win, 15, 4, "K                  : Quick Nuke build junk, caches & core dumps");
    mvwprintw(win, 16, 4, "Z                  : Inject Quota Bypass environment exports into ~/.zshrc");

    /* Category 3: Selection & Tools */
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 18, 2, " SELECTION, FILTERING & TOOLS");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 19, 4, "Space              : Toggle Mark     v / M             : Invert Marks");
    mvwprintw(win, 20, 4, "U                  : Unmark All      d / x             : Delete Items");
    mvwprintw(win, 21, 4, "/                  : Live Search     A / a             : Toggle Disk / Dotfiles");
    mvwprintw(win, 22, 4, "p                  : Scrollable Peek e / t             : $EDITOR / Subshell");
    mvwprintw(win, 23, 4, "o                  : Cycle Sort Mode E                 : Export Quota Report");

    wattron(win, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(win, 24, 2, " Press any key to return...");
    wattroff(win, COLOR_PAIR(2) | A_BOLD);

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
            pthread_mutex_lock(&g_state.lock);
            for (int i = 0; i < g_state.count; i++) {
                if (g_state.entries[i].marked) {
                    if (is_protected_target(g_state.entries[i].path)) continue;
                    char *esc = shell_escape(g_state.entries[i].path);
                    if (esc) {
                        char *cmd = NULL;
                        if (asprintf(&cmd, "rm -rf %s", esc) != -1) {
                            int ret = system(cmd);
                            (void)ret;
                            free(cmd);
                        }
                        free(esc);
                    }
                }
            }
            pthread_mutex_unlock(&g_state.lock);
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
            char *esc = shell_escape(target->path);
            if (esc) {
                char *cmd = NULL;
                if (asprintf(&cmd, "rm -rf %s", esc) != -1) {
                    int ret = system(cmd);
                    (void)ret;
                    free(cmd);
                }
                free(esc);
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
        snprintf(msg, sizeof(msg), "Move %d marked items to /goinfre & create symlinks?", marked);
        if (confirm_modal("BATCH GOINFRE LINK", msg)) {
            pthread_mutex_lock(&g_state.lock);
            for (int i = 0; i < g_state.count; i++) {
                if (g_state.entries[i].marked) {
                    if (g_state.entries[i].type == TYPE_LINK && g_state.entries[i].is_goinfre_link) continue;
                    char dest[PATH_MAX_LEN];
                    snprintf(dest, sizeof(dest), "%.2048s/%.256s", base_goinfre, g_state.entries[i].name);
                    char *esc_base = shell_escape(base_goinfre);
                    char *esc_dest = shell_escape(dest);
                    char *esc_src = shell_escape(g_state.entries[i].path);
                    if (esc_base && esc_dest && esc_src) {
                        char *cmd = NULL;
                        if (asprintf(&cmd, "mkdir -p %s && rm -rf %s && mv %s %s && ln -s %s %s",
                                     esc_base, esc_dest, esc_src, esc_dest, esc_dest, esc_src) != -1) {
                            int ret = system(cmd);
                            (void)ret;
                            free(cmd);
                        }
                    }
                    free(esc_base);
                    free(esc_dest);
                    free(esc_src);
                }
            }
            pthread_mutex_unlock(&g_state.lock);
            start_async_scan(g_state.current_dir);
        }
    } else {
        FileEntry *target = &g_state.filtered[g_state.selected];
        if (target->type == TYPE_LINK && target->is_goinfre_link) {
            confirm_modal("INFO", "Target is already a symlink pointing to goinfre!");
            return;
        }

        char dest_path[PATH_MAX_LEN];
        snprintf(dest_path, sizeof(dest_path), "%.2048s/%.256s", base_goinfre, target->name);

        char msg[256];
        snprintf(msg, sizeof(msg), "Move '%.40s' to goinfre & create symlink?", target->name);
        if (confirm_modal("GOINFRE SYMLINK", msg)) {
            char *esc_base = shell_escape(base_goinfre);
            char *esc_dest = shell_escape(dest_path);
            char *esc_src = shell_escape(target->path);
            if (esc_base && esc_dest && esc_src) {
                char *cmd = NULL;
                if (asprintf(&cmd, "mkdir -p %s && rm -rf %s && mv %s %s && ln -s %s %s",
                             esc_base, esc_dest, esc_src, esc_dest, esc_dest, esc_src) != -1) {
                    int ret = system(cmd);
                    (void)ret;
                    free(cmd);
                }
            }
            free(esc_base);
            free(esc_dest);
            free(esc_src);
            start_async_scan(g_state.current_dir);
        }
    }
}

void action_unlink_goinfre(void) {
    if (g_state.filtered_count == 0) return;
    FileEntry *target = &g_state.filtered[g_state.selected];

    if (target->type != TYPE_LINK) {
        confirm_modal("INFO", "Target is not a symlink. Press 's' to move it to /goinfre.");
        return;
    }

    if (strlen(target->symlink_target) == 0) {
        confirm_modal("ERROR", "Could not read symlink target destination.");
        return;
    }

    char msg[160];
    snprintf(msg, sizeof(msg), "Restore '%.28s' from goinfre back into HOME?", target->name);
    if (confirm_modal("UNLINK / RESTORE", msg)) {
        char *esc_link = shell_escape(target->path);
        char *esc_target = shell_escape(target->symlink_target);
        if (esc_link && esc_target) {
            char *cmd = NULL;
            if (asprintf(&cmd, "rm -f %s && mv %s %s", esc_link, esc_target, esc_link) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
        }
        free(esc_link);
        free(esc_target);
        start_async_scan(g_state.current_dir);
    }
}

void action_bootstrap_goinfre(void) {
    char base_goinfre[PATH_MAX_LEN];
    get_goinfre_path(base_goinfre, sizeof(base_goinfre));

    if (confirm_modal("BOOTSTRAP TOOLCHAINS", "Link heavy AI models, Docker, Rust & caches to /goinfre?")) {
        const char *home = getenv("HOME");
        if (!home) return;

        char *esc_base = shell_escape(base_goinfre);
        char *cmd = NULL;
        if (esc_base && asprintf(&cmd, "mkdir -p %s", esc_base) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        free(esc_base);

        for (size_t i = 0; G_BOOTSTRAP_TARGETS[i] != NULL; i++) {
            char src[PATH_MAX_LEN];
            char dst[PATH_MAX_LEN];
            snprintf(src, sizeof(src), "%.2048s/%.1024s", home, G_BOOTSTRAP_TARGETS[i]);
            snprintf(dst, sizeof(dst), "%.2048s/%.1024s", base_goinfre, G_BOOTSTRAP_TARGETS[i]);

            struct stat st;
            if (lstat(src, &st) == 0 && !S_ISLNK(st.st_mode)) {
                char *esc_src = shell_escape(src);
                char *esc_dst = shell_escape(dst);
                if (esc_src && esc_dst) {
                    if (asprintf(&cmd, "mkdir -p $(dirname %s) && rm -rf %s && mv %s %s && ln -s %s %s",
                                 esc_dst, esc_dst, esc_src, esc_dst, esc_dst, esc_src) != -1) {
                        int r = system(cmd);
                        (void)r;
                        free(cmd);
                    }
                }
                free(esc_src);
                free(esc_dst);
            } else if (lstat(src, &st) != 0) {
                char *esc_src = shell_escape(src);
                char *esc_dst = shell_escape(dst);
                if (esc_src && esc_dst) {
                    if (asprintf(&cmd, "mkdir -p %s && mkdir -p $(dirname %s) && ln -s %s %s",
                                 esc_dst, esc_src, esc_dst, esc_src) != -1) {
                        int r = system(cmd);
                        (void)r;
                        free(cmd);
                    }
                }
                free(esc_src);
                free(esc_dst);
            }
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_heal_symlinks(void) {
    const char *home = getenv("HOME");
    if (!home) return;

    if (confirm_modal("STATION SYMLINK HEALER", "Repair broken /goinfre links for this workstation?")) {
        char *esc_home = shell_escape(home);
        if (esc_home) {
            char *cmd = NULL;
            if (asprintf(&cmd,
                         "find %s -maxdepth 4 -type l -exec sh -c '"
                         "for link; do "
                         "  target=$(readlink \"$link\"); "
                         "  case \"$target\" in /goinfre/*|/sgoinfre/*|/Volumes/Storage/goinfre/*|/tmp/goinfre_*) "
                         "    if [ ! -e \"$link\" ]; then "
                         "      mkdir -p \"$target\"; "
                         "    fi;; "
                         "  esac; "
                         "done' sh {} + 2>/dev/null", esc_home) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            free(esc_home);
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_empty_trash(void) {
    if (confirm_modal("EMPTY TRASH BIN", "Permanently empty ~/.local/share/Trash and ~/.Trash?")) {
        int ret = system("rm -rf ~/.local/share/Trash/* ~/.local/share/Trash/.* ~/.Trash/* 2>/dev/null");
        (void)ret;
        start_async_scan(g_state.current_dir);
    }
}

void action_inject_zshrc(void) {
    const char *home = getenv("HOME");
    if (!home) return;

    char zshrc_path[PATH_MAX_LEN];
    snprintf(zshrc_path, sizeof(zshrc_path), "%.2048s/.zshrc", home);

    /* Check if already injected */
    FILE *check_fp = fopen(zshrc_path, "r");
    int already_injected = 0;
    if (check_fp) {
        char line[512];
        while (fgets(line, sizeof(line), check_fp)) {
            if (strstr(line, "Goinfre Quota Bypass") != NULL) {
                already_injected = 1;
                break;
            }
        }
        fclose(check_fp);
    }

    if (already_injected) {
        confirm_modal("ALREADY INJECTED", "Goinfre exports are already present in your ~/.zshrc!");
        return;
    }

    if (confirm_modal("INJECT ~/.zshrc EXPORTS", "Append goinfre cache redirect exports & aliases to ~/.zshrc?")) {
        FILE *fp = fopen(zshrc_path, "a");
        if (!fp) {
            confirm_modal("ERROR", "Could not open ~/.zshrc for writing.");
            return;
        }

        fprintf(fp, "\n");
        for (size_t i = 0; G_SHELL_EXPORTS[i] != NULL; i++) {
            fprintf(fp, "%s\n", G_SHELL_EXPORTS[i]);
        }
        fclose(fp);

        confirm_modal("SUCCESS", "Injected into ~/.zshrc! Run: source ~/.zshrc");
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
            snprintf(resolved, sizeof(resolved), "%.2048s%.2048s", home ? home : "", input_buf + 1);
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

    FILE *fp = fopen(target->path, "r");
    if (!fp) {
        confirm_modal("ERROR", "Could not open file for preview.");
        return;
    }

    #define MAX_PEEK_LINES 2000
    char **lines = malloc(MAX_PEEK_LINES * sizeof(char *));
    if (!lines) {
        fclose(fp);
        return;
    }

    int line_count = 0;
    char buffer[1024];
    int is_binary = 0;

    while (line_count < MAX_PEEK_LINES && fgets(buffer, sizeof(buffer), fp)) {
        for (int i = 0; buffer[i] != '\0'; i++) {
            if ((unsigned char)buffer[i] < 9 || ((unsigned char)buffer[i] > 13 && (unsigned char)buffer[i] < 32)) {
                is_binary = 1;
                break;
            }
        }
        if (is_binary) break;
        buffer[strcspn(buffer, "\r\n")] = 0;
        lines[line_count++] = strdup(buffer);
    }
    fclose(fp);

    int h = LINES - 4;
    int w = COLS - 8;
    if (h < 10) h = 10;
    if (w < 40) w = 40;

    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    keypad(win, TRUE);

    int scroll_pos = 0;
    int viewing = 1;

    while (viewing) {
        werase(win);
        box(win, 0, 0);

        wattron(win, COLOR_PAIR(1) | A_BOLD);
        mvwprintw(win, 1, 2, ":: [ PEEK: %.*s ] :: (%d lines)", w - 30, target->name, line_count);
        wattroff(win, COLOR_PAIR(1) | A_BOLD);

        if (is_binary) {
            wattron(win, COLOR_PAIR(4) | A_BOLD);
            mvwprintw(win, 3, 2, "[Binary File - Cannot display text preview]");
            wattroff(win, COLOR_PAIR(4) | A_BOLD);
        } else if (line_count == 0) {
            mvwprintw(win, 3, 2, "[Empty File]");
        } else {
            int max_display = h - 4;
            for (int i = 0; i < max_display; i++) {
                int line_idx = scroll_pos + i;
                if (line_idx >= line_count) break;
                mvwprintw(win, 3 + i, 2, "%4d │ %.*s", line_idx + 1, w - 10, lines[line_idx]);
            }
        }

        wattron(win, COLOR_PAIR(3) | A_BOLD);
        mvwprintw(win, h - 2, 2, " [j/k/Arrows/PageUp/Down] Scroll  │  [ESC/q] Exit Peek ");
        wattroff(win, COLOR_PAIR(3) | A_BOLD);

        wrefresh(win);
        wtimeout(win, -1);
        int ch = wgetch(win);

        if (ch == 'q' || ch == 27 || ch == 'p') {
            viewing = 0;
        } else if (ch == 'j' || ch == KEY_DOWN) {
            if (scroll_pos < line_count - (h - 4)) scroll_pos++;
        } else if (ch == 'k' || ch == KEY_UP) {
            if (scroll_pos > 0) scroll_pos--;
        } else if (ch == KEY_NPAGE || ch == ' ') {
            scroll_pos += (h - 4);
            if (scroll_pos > line_count - (h - 4)) scroll_pos = line_count - (h - 4);
            if (scroll_pos < 0) scroll_pos = 0;
        } else if (ch == KEY_PPAGE) {
            scroll_pos -= (h - 4);
            if (scroll_pos < 0) scroll_pos = 0;
        } else if (ch == 'g' || ch == KEY_HOME) {
            scroll_pos = 0;
        } else if (ch == 'G' || ch == KEY_END) {
            scroll_pos = line_count - (h - 4);
            if (scroll_pos < 0) scroll_pos = 0;
        }
    }

    delwin(win);
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);
}

void action_nuke_junk(void) {
    if (confirm_modal("NUKE CLUSTER JUNK", "Wipe Python/AI caches, node_modules, build bins (*.o, *.a, .dSYM)?")) {
        char *esc = shell_escape(g_state.current_dir);
        if (esc) {
            char *cmd = NULL;
            if (asprintf(&cmd,
                         "find %s -type d \\( -name \"node_modules\" -o -name \".cache\" -o -name \"target\" "
                         "-o -name \"*.dSYM\" -o -name \"__pycache__\" -o -name \".pytest_cache\" "
                         "-o -name \".mypy_cache\" -o -name \".ipynb_checkpoints\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
                         "find %s -type f \\( -name \"*.o\" -o -name \"*.a\" -o -name \"*.out\" -o -name \"*.pyc\" "
                         "-o -name \"*.pyo\" -o -name \".DS_Store\" -o -name \"core.*\" -o -name \"vgcore.*\" \\) -delete 2>/dev/null",
                         esc, esc) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            free(esc);
        }
        start_async_scan(g_state.current_dir);
    }
}

void action_git_doctor(void) {
    if (confirm_modal("GIT REPO DOCTOR", "Run git clean -fdx & git gc across all repos in subtree?")) {
        char *esc = shell_escape(g_state.current_dir);
        if (esc) {
            char *cmd = NULL;
            if (asprintf(&cmd,
                         "find %s -name \".git\" -type d -execdir git clean -fdx \\; -execdir git gc --prune=now --aggressive \\; 2>/dev/null",
                         esc) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            free(esc);
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
        char *esc = shell_escape(target->path);
        if (esc) {
            char *cmd = NULL;
            if (asprintf(&cmd, "%s %s", input_buf, esc) != -1) {
                printf("\n\033[1;36m[ft_ncdu Exec]\033[0m Running: %s\n", cmd);
                int ret = system(cmd);
                (void)ret;
                free(cmd);
                printf("\nPress Enter to return...");
                getchar();
            }
            free(esc);
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

    char hostname[128] = "cluster-station";
    gethostname(hostname, sizeof(hostname));

    fprintf(fp, "# 1337 / 42 Cluster Storage Audit Report\n\n");
    fprintf(fp, "**Workstation:** `%s` | **User:** `%s`\n", hostname, g_state.username);
    fprintf(fp, "**Path Audited:** `%s`\n\n", g_state.current_dir);
    fprintf(fp, "## Quota Overview\n\n");
    fprintf(fp, "| Storage Pool | Total | Used | Free | Saturation |\n");
    fprintf(fp, "| :--- | :--- | :--- | :--- |\n");
    fprintf(fp, "| Home Storage | %s | %s | %s | %.1f%% |\n\n",
            sz_tot, sz_used, sz_free, total ? ((double)used / (double)total) * 100.0 : 0.0);

    fprintf(fp, "## Top 30 Storage Consumers\n\n");
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
                (g_state.entries[i].type == TYPE_DIR ? "DIR " : (g_state.entries[i].type == TYPE_LINK ? "LINK" : "FILE")),
                sz,
                g_state.entries[i].name);
    }
    pthread_mutex_unlock(&g_state.lock);

    fclose(fp);
    confirm_modal("EXPORT SUCCESS", "Exported complete storage audit to quota_report.md!");
}

void action_cleaning_presets(void) {
    int h = (int)PRESET_COUNT + 8;
    int w = 76;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ 1337 / 42 CLEANING PRESETS ] ::");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    for (size_t i = 0; i < PRESET_COUNT; i++) {
        mvwprintw(win, 3 + (int)i, 2, "[%c] %-22s : %.44s",
                  G_CLEAN_PRESETS[i].key,
                  G_CLEAN_PRESETS[i].title,
                  G_CLEAN_PRESETS[i].desc);
    }

    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, h - 3, 2, "Select preset key [1-%d] or press [ESC] to cancel: ", (int)PRESET_COUNT);
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

    char *esc = shell_escape(g_state.filtered[g_state.selected].path);
    if (esc) {
        char *cmd = NULL;
        if (asprintf(&cmd, "%s %s", editor, esc) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        free(esc);
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

void action_batch_invert(void) {
    pthread_mutex_lock(&g_state.lock);
    for (int i = 0; i < g_state.count; i++) {
        g_state.entries[i].marked = !g_state.entries[i].marked;
    }
    pthread_mutex_unlock(&g_state.lock);
    apply_filter();
}

void action_batch_unmark(void) {
    pthread_mutex_lock(&g_state.lock);
    for (int i = 0; i < g_state.count; i++) {
        g_state.entries[i].marked = 0;
    }
    pthread_mutex_unlock(&g_state.lock);
    apply_filter();
}

int run_cli_clean(void) {
    printf("\033[1;32m[ft_ncdu]\033[0m Running 42 Cluster Fast Cleaner...\n");
    for (size_t i = 0; i < PRESET_COUNT; i++) {
        char *cmd = NULL;
        const char *home = getenv("HOME");
        if (!home) home = ".";
        if (asprintf(&cmd, G_CLEAN_PRESETS[i].command_fmt, home, home) != -1) {
            printf("  -> %s\n", G_CLEAN_PRESETS[i].title);
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
    }
    printf("\033[1;32m[ft_ncdu]\033[0m Cluster cleaning complete! Check storage with 'ft_ncdu'.\n");
    return 0;
}

int run_cli_heal(void) {
    const char *home = getenv("HOME");
    if (!home) return 1;
    printf("\033[1;36m[ft_ncdu]\033[0m Repairing broken /goinfre symlinks for current station...\n");
    char *esc_home = shell_escape(home);
    if (esc_home) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find %s -maxdepth 4 -type l -exec sh -c '"
                     "for link; do "
                     "  target=$(readlink \"$link\"); "
                     "  case \"$target\" in /goinfre/*|/sgoinfre/*|/Volumes/Storage/goinfre/*|/tmp/goinfre_*) "
                     "    if [ ! -e \"$link\" ]; then "
                     "      mkdir -p \"$target\"; "
                     "      echo \"  Healed: $link -> $target\"; "
                     "    fi;; "
                     "  esac; "
                     "done' sh {} + 2>/dev/null", esc_home) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        free(esc_home);
    }
    printf("\033[1;32m[ft_ncdu]\033[0m All goinfre targets restored.\n");
    return 0;
}

int run_cli_bootstrap(void) {
    char base_goinfre[PATH_MAX_LEN];
    struct passwd *pw = getpwuid(getuid());
    const char *username = pw ? pw->pw_name : "user";
    if (access("/goinfre", F_OK) == 0) snprintf(base_goinfre, sizeof(base_goinfre), "/goinfre/%.128s", username);
    else if (access("/sgoinfre", F_OK) == 0) snprintf(base_goinfre, sizeof(base_goinfre), "/sgoinfre/%.128s", username);
    else snprintf(base_goinfre, sizeof(base_goinfre), "/tmp/goinfre_%.128s", username);

    const char *home = getenv("HOME");
    if (!home) return 1;

    printf("\033[1;36m[ft_ncdu]\033[0m Bootstrapping caches and toolchains to %s...\n", base_goinfre);
    char *esc_base = shell_escape(base_goinfre);
    char *cmd = NULL;
    if (asprintf(&cmd, "mkdir -p %s", esc_base) != -1) {
        int r = system(cmd);
        (void)r;
        free(cmd);
    }
    free(esc_base);

    for (size_t i = 0; G_BOOTSTRAP_TARGETS[i] != NULL; i++) {
        char src[PATH_MAX_LEN], dst[PATH_MAX_LEN];
        snprintf(src, sizeof(src), "%.2048s/%.1024s", home, G_BOOTSTRAP_TARGETS[i]);
        snprintf(dst, sizeof(dst), "%.2048s/%.1024s", base_goinfre, G_BOOTSTRAP_TARGETS[i]);
        struct stat st;
        if (lstat(src, &st) == 0 && !S_ISLNK(st.st_mode)) {
            char *esc_src = shell_escape(src);
            char *esc_dst = shell_escape(dst);
            if (asprintf(&cmd, "mkdir -p $(dirname %s) && rm -rf %s && mv %s %s && ln -s %s %s",
                         esc_dst, esc_dst, esc_src, esc_dst, esc_dst, esc_src) != -1) {
                printf("  Linked: ~/%s -> %s\n", G_BOOTSTRAP_TARGETS[i], dst);
                int r = system(cmd);
                (void)r;
                free(cmd);
            }
            free(esc_src);
            free(esc_dst);
        }
    }
    printf("\033[1;32m[ft_ncdu]\033[0m Bootstrap completed successfully!\n");
    return 0;
}

int run_cli_report(const char *target_path) {
    struct statvfs vfs;
    statvfs(target_path, &vfs);
    unsigned long long total = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
    unsigned long long free_b = (unsigned long long)vfs.f_bfree * vfs.f_frsize;
    unsigned long long used = (total > free_b) ? (total - free_b) : 0;

    char sz_tot[16], sz_used[16], sz_free[16];
    format_size(total, sz_tot, sizeof(sz_tot));
    format_size(used, sz_used, sizeof(sz_used));
    format_size(free_b, sz_free, sizeof(sz_free));

    printf("\n======================================================\n");
    printf("  \033[1;36m42 / 1337 CLUSTER STORAGE AUDIT REPORT\033[0m\n");
    printf("======================================================\n");
    printf("  Target Path : %s\n", target_path);
    printf("  Total Quota : %s\n", sz_tot);
    printf("  Used Space  : %s (%.1f%%)\n", sz_used, total ? ((double)used / (double)total) * 100.0 : 0.0);
    printf("  Free Space  : %s\n", sz_free);
    printf("======================================================\n\n");
    return 0;
}

void print_cli_help(const char *prog_name) {
    printf("Usage: %s [OPTIONS] [PATH]\n\n", prog_name);
    printf("The high-performance NCurses Disk Usage & Cluster Quota Manager for 42 / 1337.\n\n");
    printf("Options:\n");
    printf("  -h, --help        Show this help message and exit\n");
    printf("  -v, --version     Show version information\n");
    printf("  -c, --clean       Run modular cluster cleaning presets headlessly\n");
    printf("  --heal            Repair broken goinfre symlinks headlessly\n");
    printf("  --bootstrap       Relocate heavy toolchains/models to goinfre headlessly\n");
    printf("  --report          Print quota audit summary to stdout\n\n");
    printf("Interactive Controls:\n");
    printf("  j/k, Up/Down      Navigate file list\n");
    printf("  l, Enter, Right   Step into directory\n");
    printf("  h, Backspace, -   Step out to parent directory\n");
    printf("  Space             Toggle selection mark (Batch Mode)\n");
    printf("  d, dd, x          Delete selected or marked items\n");
    printf("  s                 Move target to /goinfre and create symlink\n");
    printf("  u                 Unlink target from /goinfre back to HOME\n");
    printf("  H                 Station Healer (Fix dangling goinfre links)\n");
    printf("  b                 Bootstrap AI models & toolchains to goinfre\n");
    printf("  T                 Purge desktop Trash (~/.local/share/Trash)\n");
    printf("  C                 Modular Cleaning Presets menu\n");
    printf("  p                 Scrollable file content peek preview\n");
    printf("  Z                 Inject quota bypass exports into ~/.zshrc\n");
    printf("  /                 Instant fuzzy filter / search\n");
    printf("  ?                 Show interactive keyboard help\n");
    printf("  q                 Quit\n\n");
}

void print_cli_version(void) {
    printf("%s v%s - 42 / 1337 Cluster Storage Suite\n", APP_NAME, APP_VERSION);
}
