#include "ft_ncdu.h"

AppState g_state;

static const CommandBinding G_COMMANDS[] = {
    {'s', "Symlink Goinfre", "Move directory to /goinfre and symlink", action_symlink_goinfre},
    {'H', "Heal Links",      "Repair broken symlinks on new station",   action_heal_symlinks},
    {'b', "Bootstrap Tools", "Auto-link AI/Rust/Docker/NPM to goinfre", action_bootstrap_goinfre},
    {'Z', "Inject ~/.zshrc", "Inject cache redirection env vars",       action_inject_zshrc},
    {'C', "Clean Presets",   "Open modular cleaning preset picker",     action_cleaning_presets},
    {'K', "Nuke Junk",       "Wipe build outputs, caches & core dumps", action_nuke_junk},
    {'G', "Git Doctor",      "Run git clean -fdx & aggressive gc",      action_git_doctor},
    {'D', "Docker Prune",    "Execute docker system prune -a --volumes",action_docker_prune},
    {'p', "Peek File",       "Quick file content previewer",            action_file_peek},
    {'P', "Goto Path",       "Teleport jump to arbitrary directory",    action_goto_path},
    {':', "Goto Path",       "Teleport jump to arbitrary directory",    action_goto_path},
    {'!', "Custom Command",  "Execute shell command with target path",  action_custom_command},
    {'E', "Export Report",   "Dump Markdown storage audit",             action_export_report},
    {'e', "Edit in Neovim",  "Open selected file/folder in $EDITOR",    action_edit},
    {'t', "Subshell",        "Drop into interactive terminal shell",    action_shell},
    {'?', "Help Menu",       "Display interactive keyboard reference",  show_help_modal},
    {0,   NULL,              NULL,                                      NULL}
};

static void handle_signal(int sig) {
    (void)sig;
    g_state.abort_scan = 1;
    endwin();
    pthread_mutex_destroy(&g_state.lock);
    exit(0);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    pthread_mutex_init(&g_state.lock, NULL);

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
    timeout(TICK_DELAY_MS);

    init_ui_colors();

    g_state.sort_mode = SORT_SIZE_DESC;
    g_state.size_mode = SIZE_ACTUAL_DISK;
    g_state.show_hidden = 1;
    g_state.search_query[0] = '\0';
    g_state.is_searching = 0;
    g_state.is_scanning = 0;
    g_state.abort_scan = 0;
    g_state.spinner_frame = 0;
    g_state.unreadable_count = 0;

    start_async_scan(start_path);

    int ch;
    int g_pressed = 0;
    int d_pressed = 0;

    while (1) {
        draw_ui();
        ch = getch();

        if (ch == ERR) continue;

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

        /* 1. Live Filter Input Handling */
        if (g_state.is_searching) {
            if (ch == 27 || ch == 10) {
                g_state.is_searching = 0;
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8 || ch == KEY_DC) {
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

        if (ch == 27) {
            g_state.search_query[0] = '\0';
            apply_filter();
            continue;
        }

        /* 2. Navigation & Feature Controls */
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
                pthread_mutex_lock(&g_state.lock);
                g_state.filtered[g_state.selected].marked = !g_state.filtered[g_state.selected].marked;
                for (int i = 0; i < g_state.count; i++) {
                    if (strcmp(g_state.entries[i].path, g_state.filtered[g_state.selected].path) == 0) {
                        g_state.entries[i].marked = g_state.filtered[g_state.selected].marked;
                        break;
                    }
                }
                pthread_mutex_unlock(&g_state.lock);
                if (g_state.selected < g_state.filtered_count - 1) {
                    g_state.selected++;
                    if (g_state.selected >= g_state.scroll_offset + list_h) {
                        g_state.scroll_offset++;
                    }
                }
            }
        } else if (ch == 'l' || ch == 10 || ch == KEY_RIGHT) {
            if (g_state.filtered_count > 0 && g_state.filtered[g_state.selected].type == TYPE_DIR) {
                start_async_scan(g_state.filtered[g_state.selected].path);
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
                    start_async_scan(resolved);
                }
            }
        } else if (ch == '/') {
            g_state.is_searching = 1;
        } else if (ch == 'A') {
            g_state.size_mode = (g_state.size_mode == SIZE_ACTUAL_DISK) ? SIZE_APPARENT : SIZE_ACTUAL_DISK;
            pthread_mutex_lock(&g_state.lock);
            qsort(g_state.entries, g_state.count, sizeof(FileEntry), compare_entries);
            pthread_mutex_unlock(&g_state.lock);
            apply_filter();
        } else if (ch == 'a') {
            g_state.show_hidden = !g_state.show_hidden;
            apply_filter();
        } else if (ch == 'o') {
            g_state.sort_mode = (SortMode)((g_state.sort_mode + 1) % 4);
            pthread_mutex_lock(&g_state.lock);
            qsort(g_state.entries, g_state.count, sizeof(FileEntry), compare_entries);
            pthread_mutex_unlock(&g_state.lock);
            apply_filter();
        } else if (ch == 'r') {
            start_async_scan(g_state.current_dir);
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
        } else {
            for (size_t i = 0; G_COMMANDS[i].handler != NULL; i++) {
                if (ch == G_COMMANDS[i].key) {
                    G_COMMANDS[i].handler();
                    break;
                }
            }
        }

        g_pressed = 0;
        d_pressed = 0;
    }

    pthread_mutex_destroy(&g_state.lock);
    endwin();
    return 0;
}
