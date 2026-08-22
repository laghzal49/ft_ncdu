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
    int ch = wgetch(win);
    delwin(win);
    return (ch == 'y' || ch == 'Y');
}

void show_help_modal(void) {
    int h = 20, w = 72;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 1, 2, ":: [ 1337 NCDU COMMAND REFERENCE ] ::");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);

    mvwprintw(win, 3, 2,  " j / k / Arrows : Move cursor Up / Down");
    mvwprintw(win, 4, 2,  " l / Enter      : Step inside directory");
    mvwprintw(win, 5, 2,  " h / Backspace  : Step out to parent directory");
    mvwprintw(win, 6, 2,  " Space          : Toggle selection mark (Batch mode)");
    mvwprintw(win, 7, 2,  " dd / x         : Delete item (or all marked items)");
    mvwprintw(win, 8, 2,  " s              : Move & Symlink to /goinfre");
    mvwprintw(win, 9, 2,  " H              : Station-Switch Symlink Healer (Repair links)");
    mvwprintw(win, 10, 2, " b              : Bootstrap AI/Rust/Docker/NPM tools to goinfre");
    mvwprintw(win, 11, 2, " K              : Nuke Python, C, Caches & AI temp bloat");
    mvwprintw(win, 12, 2, " G              : Git Doctor (clean -fdx & aggressive repack)");
    mvwprintw(win, 13, 2, " D              : Docker System Prune (-a --volumes)");
    mvwprintw(win, 14, 2, " o              : Toggle Sort (Size v / Name a-z)");
    mvwprintw(win, 15, 2, " /              : Live search/filter (ESC to clear)");
    mvwprintw(win, 16, 2, " e / t          : Open $EDITOR (nvim) / Subshell");

    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 18, 2, " Press any key to close help...");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);

    wrefresh(win);
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
                    if (asprintf(&cmd, "rm -rf \"%s\"", g_state.filtered[i].path) != -1) {
                        int ret = system(cmd);
                        (void)ret;
                        free(cmd);
                    }
                }
            }
            scan_directory(g_state.current_dir);
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
            if (asprintf(&cmd, "rm -rf \"%s\"", target->path) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            scan_directory(g_state.current_dir);
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
                        if (asprintf(&cmd, "mkdir -p \"%s\" && rm -rf \"%s\" && mv \"%s\" \"%s\" && ln -s \"%s\" \"%s\"",
                                     base_goinfre, dest, g_state.filtered[i].path, dest, dest, g_state.filtered[i].path) != -1) {
                            int ret = system(cmd);
                            (void)ret;
                            free(cmd);
                        }
                        free(dest);
                    }
                }
            }
            scan_directory(g_state.current_dir);
        }
    } else {
        FileEntry *target = &g_state.filtered[g_state.selected];
        if (target->type != TYPE_DIR) return;

        char *dest_path = NULL;
        if (asprintf(&dest_path, "%s/%s", base_goinfre, target->name) == -1) return;

        if (confirm_modal("GOINFRE SYMLINK", "Move folder to /goinfre & create symlink?")) {
            char *cmd = NULL;
            if (asprintf(&cmd, "mkdir -p \"%s\" && rm -rf \"%s\" && mv \"%s\" \"%s\" && ln -s \"%s\" \"%s\"",
                         base_goinfre, dest_path, target->path, dest_path, dest_path, target->path) != -1) {
                int ret = system(cmd);
                (void)ret;
                free(cmd);
            }
            scan_directory(g_state.current_dir);
        }
        free(dest_path);
    }
}

void action_bootstrap_goinfre(void) {
    char base_goinfre[PATH_MAX_LEN];
    get_goinfre_path(base_goinfre, sizeof(base_goinfre));

    if (confirm_modal("BOOTSTRAP 1337 TOOLCHAINS", "Link HuggingFace, PyTorch, Ollama, Cargo & Docker to goinfre?")) {
        const char *targets[] = {
            ".cache/huggingface",
            ".cache/torch",
            ".cache/pip",
            ".ollama",
            ".triton",
            ".cache",
            ".docker",
            ".cargo",
            ".rustup",
            ".npm",
            "Library/Caches",
            ".vscode/extensions"
        };
        const char *home = getenv("HOME");
        if (!home) return;

        char *cmd = NULL;
        if (asprintf(&cmd, "mkdir -p \"%s\"", base_goinfre) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }

        for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
            char *src = NULL;
            char *dst = NULL;
            if (asprintf(&src, "%s/%s", home, targets[i]) != -1 &&
                asprintf(&dst, "%s/%s", base_goinfre, targets[i]) != -1) {
                
                struct stat st;
                if (lstat(src, &st) == 0 && !S_ISLNK(st.st_mode)) {
                    if (asprintf(&cmd, "mkdir -p $(dirname \"%s\") && rm -rf \"%s\" && mv \"%s\" \"%s\" && ln -s \"%s\" \"%s\"",
                                 dst, dst, src, dst, dst, src) != -1) {
                        int r = system(cmd);
                        (void)r;
                        free(cmd);
                    }
                } else if (lstat(src, &st) != 0) {
                    if (asprintf(&cmd, "mkdir -p \"%s\" && mkdir -p $(dirname \"%s\") && ln -s \"%s\" \"%s\"",
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
        scan_directory(g_state.current_dir);
    }
}

void action_heal_symlinks(void) {
    const char *home = getenv("HOME");
    if (!home) return;

    if (confirm_modal("SYMLINK HEALER", "Scan and repair broken /goinfre links on new machine station?")) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find \"%s\" -maxdepth 3 -type l -exec sh -c '"
                     "for link; do "
                     "  target=$(readlink \"$link\"); "
                     "  case \"$target\" in /goinfre/*|/sgoinfre/*) "
                     "    if [ ! -e \"$link\" ]; then "
                     "      mkdir -p \"$target\"; "
                     "      echo \"Healed: $link -> $target\"; "
                     "    fi;; "
                     "  esac; "
                     "done' sh {} + 2>/dev/null", home) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        scan_directory(g_state.current_dir);
    }
}

void action_nuke_junk(void) {
    if (confirm_modal("NUKE CLUSTER JUNK", "Wipe Python/AI caches, node_modules, build bins (*.o, *.a, .dSYM)?")) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find \"%s\" -type d \\( -name \"node_modules\" -o -name \".cache\" -o -name \"target\" "
                     "-o -name \"*.dSYM\" -o -name \"__pycache__\" -o -name \".pytest_cache\" "
                     "-o -name \".mypy_cache\" -o -name \".ipynb_checkpoints\" \\) -prune -exec rm -rf {} + 2>/dev/null; "
                     "find \"%s\" -type f \\( -name \"*.o\" -o -name \"*.a\" -o -name \"*.out\" -o -name \"*.pyc\" "
                     "-o -name \"*.pyo\" -o -name \".DS_Store\" -o -name \"core.*\" -o -name \"vgcore.*\" \\) -delete 2>/dev/null",
                     g_state.current_dir, g_state.current_dir) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        scan_directory(g_state.current_dir);
    }
}

void action_git_doctor(void) {
    if (confirm_modal("GIT REPO DOCTOR", "Run git clean -fdx & git gc across all repos in subtree?")) {
        char *cmd = NULL;
        if (asprintf(&cmd,
                     "find \"%s\" -name \".git\" -type d -execdir git clean -fdx \\; -execdir git gc --prune=now --aggressive \\; 2>/dev/null",
                     g_state.current_dir) != -1) {
            int ret = system(cmd);
            (void)ret;
            free(cmd);
        }
        scan_directory(g_state.current_dir);
    }
}

void action_docker_prune(void) {
    if (confirm_modal("DOCKER NUKE", "Execute docker system prune -a --volumes -f?")) {
        int ret = system("docker system prune -a --volumes -f >/dev/null 2>&1");
        (void)ret;
        scan_directory(g_state.current_dir);
    }
}

void action_edit(void) {
    if (g_state.filtered_count == 0) return;
    def_prog_mode();
    endwin();

    const char *editor = getenv("EDITOR");
    if (!editor) editor = "nvim";

    char *cmd = NULL;
    if (asprintf(&cmd, "%s \"%s\"", editor, g_state.filtered[g_state.selected].path) != -1) {
        int ret = system(cmd);
        (void)ret;
        free(cmd);
    }

    reset_prog_mode();
    refresh();
    scan_directory(g_state.current_dir);
}

void action_shell(void) {
    def_prog_mode();
    endwin();

    const char *shell = getenv("SHELL");
    if (!shell) shell = "/bin/zsh";

    printf("\n\033[1;36m[1337-NCDU]\033[0m Subshell at %s (type 'exit' to return)...\n", g_state.current_dir);
    if (chdir(g_state.current_dir) == 0) {
        int ret = system(shell);
        (void)ret;
    }

    reset_prog_mode();
    refresh();
    scan_directory(g_state.current_dir);
}
