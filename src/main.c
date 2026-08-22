#include "ft_ncdu.h"

AppState g_state;

int main(int argc, char **argv) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        safe_str_copy(g_state.username, pw->pw_name, sizeof(g_state.username));
    } else {
        safe_str_copy(g_state.username, "user", sizeof(g_state.username));
    }

    char start_path[PATH_MAX_LEN];
    if (argc > 1) {
        if (!realpath(argv[1], start_path)) {
            safe_str_copy(start_path, argv[1], sizeof(start_path));
        }
    } else {
        const char *home = getenv("HOME");
        safe_str_copy(start_path, home ? home : ".", sizeof(start_path));
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    init_ui_colors();

    g_state.sort_mode = SORT_SIZE_DESC;
    g_state.search_query[0] = '\0';
    g_state.is_searching = 0;

    scan_directory(start_path);

    int ch;
    int g_pressed = 0;
    int d_pressed = 0;

    while (1) {
        draw_ui();
        ch = getch();

        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        (void)max_x;
        int list_h = max_y - 6;
        if (list_h < 1) list_h = 1;

        if (ch == KEY_RESIZE) {
            endwin();
            refresh();
            clear();
            continue;
        }

        // Search Input Handling
        if (g_state.is_searching) {
            if (ch == 27 || ch == 10) {
                g_state.is_searching = 0;
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                size_t len = strlen(g_state.search_query);
                if (len > 0) g_state.search_query[len - 1] = '\0';
                apply_filter();
            } else if (ch >= 32 && ch <= 126) {
                size_t len = strlen(g_state.search_query);
                if (len < sizeof(g_state.search_query) - 2) {
                    g_state.search_query[len] = (char)ch;
                    g_state.search_query[len + 1] = '\0';
                    apply_filter();
                }
            }
            continue;
        }

        if (ch == 'q') break;

        // Escape Clears Filter
        if (ch == 27) {
            g_state.search_query[0] = '\0';
            apply_filter();
            continue;
        }

        // Navigation
        if (ch == 'j' || ch == KEY_DOWN) {
            if (g_state.selected < g_state.filtered_count - 1) {
                g_state.selected++;
                if (g_state.selected >= g_state.scroll_offset + list_h) {
                    g_state.scroll_offset++;
                }
            }
        } else if (ch == 'k' || ch == KEY_UP) {
            if (g_state.selected > 0) {
                g_state.selected--;
                if (g_state.selected < g_state.scroll_offset) {
                    g_state.scroll_offset--;
                }
            }
        } else if (ch == 'g') {
            if (g_pressed) {
                g_state.selected = 0;
                g_state.scroll_offset = 0;
                g_pressed = 0;
            } else {
                g_pressed = 1;
                continue;
            }
        } else if (ch == 'G') {
            if (g_state.filtered_count > 0) {
                g_state.selected = g_state.filtered_count - 1;
                g_state.scroll_offset = (g_state.filtered_count > list_h) ? (g_state.filtered_count - list_h) : 0;
            }
        } else if (ch == ' ') {
            if (g_state.filtered_count > 0) {
                g_state.filtered[g_state.selected].marked = !g_state.filtered[g_state.selected].marked;
                for (int i = 0; i < g_state.count; i++) {
                    if (strcmp(g_state.entries[i].path, g_state.filtered[g_state.selected].path) == 0) {
                        g_state.entries[i].marked = g_state.filtered[g_state.selected].marked;
                        break;
                    }
                }
                if (g_state.selected < g_state.filtered_count - 1) {
                    g_state.selected++;
                    if (g_state.selected >= g_state.scroll_offset + list_h) {
                        g_state.scroll_offset++;
                    }
                }
            }
        } else if (ch == 'l' || ch == 10 || ch == KEY_RIGHT) {
            if (g_state.filtered_count > 0 && g_state.filtered[g_state.selected].type == TYPE_DIR) {
                scan_directory(g_state.filtered[g_state.selected].path);
            }
        } else if (ch == 'h' || ch == KEY_BACKSPACE || ch == 127 || ch == '-' || ch == KEY_LEFT) {
            char parent[PATH_MAX_LEN];
            size_t c_len = strlen(g_state.current_dir);
            if (c_len + 4 < PATH_MAX_LEN) {
                memcpy(parent, g_state.current_dir, c_len);
                parent[c_len] = '/';
                parent[c_len + 1] = '.';
                parent[c_len + 2] = '.';
                parent[c_len + 3] = '\0';
                char resolved[PATH_MAX_LEN];
                if (realpath(parent, resolved)) {
                    scan_directory(resolved);
                }
            }
        }
        // Actions
        else if (ch == '?') {
            show_help_modal();
        } else if (ch == '/') {
            g_state.is_searching = 1;
        } else if (ch == 'o') {
            g_state.sort_mode = (g_state.sort_mode == SORT_SIZE_DESC) ? SORT_NAME_ASC : SORT_SIZE_DESC;
            qsort(g_state.entries, g_state.count, sizeof(FileEntry), compare_entries);
            apply_filter();
        } else if (ch == 'd' || ch == 'x') {
            if (ch == 'x') {
                action_delete();
            } else if (d_pressed) {
                d_pressed = 0;
                action_delete();
            } else {
                d_pressed = 1;
                continue;
            }
        } else if (ch == 's') {
            action_symlink_goinfre();
        } else if (ch == 'H') {
            action_heal_symlinks();
        } else if (ch == 'b') {
            action_bootstrap_goinfre();
        } else if (ch == 'K') {
            action_nuke_junk();
        } else if (ch == 'D') {
            action_docker_prune();
        } else if (ch == 'G') {
            action_git_doctor();
        } else if (ch == 'e') {
            action_edit();
        } else if (ch == 't') {
            action_shell();
        } else if (ch == 'r') {
            scan_directory(g_state.current_dir);
        }

        g_pressed = 0;
        d_pressed = 0;
    }

    endwin();
    return 0;
}
