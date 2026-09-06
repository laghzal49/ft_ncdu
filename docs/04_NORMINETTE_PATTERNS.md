# 📐 42 Norminette Design Patterns for Advanced Projects

Writing a multi-threaded, asynchronous TUI application in C while strictly complying with the **42 Norminette v3 standard** requires specific design patterns.

This guide outlines the patterns used in **`ft_ncdu`**.

---

## 🛑 The Core Norminette Constraints
1. **$\le 25$ Lines per function body**.
2. **$\le 5$ Functions per `.c` file**.
3. **$\le 4$ Arguments per function signature**.
4. **Zero Ternary Operators (`? :` forbidden)**.
5. **Variables must be declared at the top of function scope**.
6. **No logic operators (`&&`, `||`) at the end of a line** (must be at start of next indented line).
7. **Single blank line between functions; zero blank lines inside function bodies**.

---

## 🛠️ Key Architectural Patterns

### Pattern 1: Argument Packaging with `t_rect`
**Problem**: Drawing a box or table row requires `y`, `x`, `height`, `width`, `title`, `color` (6 arguments, max allowed is 4).

**Solution**: Define a `t_rect` geometry packaging struct:
```c
typedef struct s_rect
{
    int y;
    int x;
    int h;
    int w;
}   t_rect;

// Function now takes only 3 parameters:
void draw_box(t_rect r, const char *title, int color);
```

---

### Pattern 2: Multi-step Decomposition
**Problem**: A function doing parsing, memory allocation, stat query, and thread creation exceeds 25 lines.

**Solution**: Split into specialized, single-purpose helper functions across the module:
```c
// Instead of one 40-line scan launcher:
static void reset_scan_counters(const char *dir_path);
static void *async_scan_orchestrator(void *arg);
void        start_async_scan(const char *dir_path);
```

---

### Pattern 3: Eliminating Forbidden Ternaries
**Problem**: Compact C code often uses ternaries like `name ? name : "none"`.

**Solution**: Use clean conditional assignments or small helper getters:
```c
// Forbidden by Norm:
// mvprintw(y, x, "%s", fe->marked ? "✔" : " ");

// Norm-compliant:
if (fe->marked)
    mvprintw(y, x, "✔");
else
    mvprintw(y, x, " ");
```

---

### Pattern 4: Global State Struct vs Multiple Globals
**Problem**: Norminette raises `GLOBAL_VAR_DETECTED` for every global variable.

**Solution**: Encapsulate all global state into a single unified `t_app_state g_state;` structure protected by a single `pthread_mutex_t lock`.

---

### Pattern 5: Dispatcher Branching & Table Lookup
**Problem**: Handling 25 keyboard shortcuts in one `if/else if` chain easily exceeds 25 lines.

**Solution**: Group shortcuts by category into sub-dispatchers:
```c
void handle_action_keys(int ch)
{
    if (handle_goinfre_keys(ch))
        return ;
    if (ch == 'C')
        action_cleaning_presets();
    else if (ch == '?')
        show_help_modal();
    else
        handle_tool_keys(ch);
}
```

---

## 🎯 Verification Command
Always verify with:
```bash
make norm
```
