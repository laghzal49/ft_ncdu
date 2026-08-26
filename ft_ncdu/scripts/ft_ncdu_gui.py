#!/usr/bin/env python3
"""
ft_ncdu v2.0.0 — Disk Explorer (42/1337 Edition)
A comprehensive Tkinter-based file browser and disk usage analyzer.
"""

import os
import queue
import shlex
import sys

# Early sanity check: some cluster workstations have a Python/GLIBC mismatch
# that breaks standard-library C extensions and tkinter. Catch it here with
# a clear message instead of a confusing traceback.
try:
    import stat
    import shutil
    import threading
    import time
    from pathlib import Path
    from datetime import datetime
    import tkinter as tk
    from tkinter import ttk, messagebox, simpledialog, filedialog
except ImportError as _err:
    print(f"\033[1;31mft_ncdu-gui: fatal import error:\033[0m {_err}",
          file=sys.stderr)
    print("", file=sys.stderr)
    if "GLIBC" in str(_err):
        print("Your Python was compiled against a newer GLIBC than is",
              file=sys.stderr)
        print("installed on this system. Possible fixes:", file=sys.stderr)
        print("  1. Update your system:  pacman -Syu  /  apt upgrade",
              file=sys.stderr)
        print("  2. Reinstall Python to match your current GLIBC",
              file=sys.stderr)
        print("  3. Use the TUI instead:  ft_ncdu-tui  (no Python needed)",
              file=sys.stderr)
    elif "tkinter" in str(_err).lower():
        print("Tkinter is not installed. Install it with:", file=sys.stderr)
        print("  Arch:   pacman -S tk", file=sys.stderr)
        print("  Debian: apt install python3-tk", file=sys.stderr)
        print("  Fedora: dnf install python3-tkinter", file=sys.stderr)
    else:
        print(f"Missing module. Try:  pip install {_err.name or '???'}",
              file=sys.stderr)
    print("", file=sys.stderr)
    print("The C terminal UI works without Python:  \033[1;36mft_ncdu-tui\033[0m",
          file=sys.stderr)
    sys.exit(1)

try:
    import subprocess
except ImportError:
    subprocess = None

# ----------------- CONSTANTS -----------------

BG_MAIN = '#11151b'
BG_PANEL = '#181e27'
BG_HEADER = '#222b38'
FG_PRIMARY = '#dce4ee'
FG_SECONDARY = '#9aa8b8'
FG_MUTED = '#6b7a8d'
ACCENT_TITLE = '#f4b860'
ACCENT_HEADER = '#9ccfd8'
ACCENT_GREEN = '#82d9a0'
ACCENT_RED = '#f0a0a0'
ACCENT_YELLOW = '#eebb60'
BG_SELECTION = '#2b5d67'
BG_BUTTON = '#273142'
FG_BUTTON = '#edf2f7'

PROTECTED_PATHS = [
    '.ssh', '.zshrc', '.bashrc', '.bash_profile', '.zprofile',
    '.zshenv', '.gitconfig', '.profile', '.vimrc', '.inputrc',
    '.gnupg', '.config/nvim'
]

CLEAN_PRESETS = [
    {'key': '1', 'title': '42 C / C++ Dev Output',
     'desc': 'Remove *.o, *.a, *.dSYM, vgcore.*, clangd cache',
     'command': "find {home} -type f \\( -name '*.o' -o -name '*.a' -o -name '*.dSYM' -o -name 'vgcore.*' \\) -delete 2>/dev/null; rm -rf {home}/.cache/clangd 2>/dev/null"},
    {'key': '2', 'title': 'Francinette & Testers',
     'desc': 'Purge francinette temp, logs, gcda/gcno',
     'command': "rm -rf {home}/francinette/temp {home}/.francinette/logs 2>/dev/null; find {home} -type f \\( -name '*.gcda' -o -name '*.gcno' \\) -delete 2>/dev/null"},
    {'key': '3', 'title': 'AI & Python ML Caches',
     'desc': 'Remove __pycache__, huggingface, torch, pip, ollama',
     'command': 'rm -rf {home}/__pycache__ {home}/.cache/huggingface {home}/.cache/torch {home}/.cache/pip {home}/.cache/ollama 2>/dev/null'},
    {'key': '4', 'title': 'Web & Transcendence',
     'desc': 'node_modules, npm, yarn, .next, .turbo',
     'command': "find {home} -maxdepth 6 -type d -name 'node_modules' -exec rm -rf {{}} + 2>/dev/null; rm -rf {home}/.npm/_cacache {home}/.yarn/cache {home}/.next {home}/.turbo 2>/dev/null"},
    {'key': '5', 'title': 'Browser & Electron Apps',
     'desc': 'Chrome, Chromium, Brave, Firefox, VSCode, Discord, Slack caches',
     'command': 'rm -rf {home}/.cache/google-chrome {home}/.cache/chromium {home}/.cache/BraveSoftware {home}/.mozilla/firefox/*.default*/cache2 {home}/.config/Code/Cache {home}/.config/discord/Cache {home}/.config/slack/Cache 2>/dev/null'},
    {'key': '6', 'title': 'Docker Cluster Prune',
     'desc': 'docker system prune -a --volumes -f',
     'command': 'docker system prune -a --volumes -f 2>/dev/null'},
    {'key': '7', 'title': 'Norminette & Shell Logs',
     'desc': 'norminette cache, zcompdump, xsession-errors',
     'command': 'rm -rf {home}/.cache/norminette 2>/dev/null; rm -f {home}/.zcompdump* {home}/.xsession-errors* 2>/dev/null'},
    {'key': '8', 'title': 'Desktop Trash Bin',
     'desc': 'Empty ~/.local/share/Trash and ~/.Trash',
     'command': 'rm -rf {home}/.local/share/Trash/* {home}/.Trash/* 2>/dev/null'},
    {'key': '9', 'title': 'Nuclear 1337 Wipe',
     'desc': 'Deep purge across ALL cache tiers',
     'command': 'ALL_ABOVE_COMBINED'}
]

BOOTSTRAP_TARGETS = [
    '.cache/huggingface', '.cache/torch', '.cache/triton',
    '.cache/pip', '.cache/uv', '.cache/whisper',
    '.ollama/models', '.cargo', '.rustup', '.npm',
    '.local/share/pnpm', '.docker', '.var/app'
]

# ----------------- HELPER FUNCTIONS -----------------

def format_size(size_bytes):
    if size_bytes == 0: return "0 B"
    units = ["B", "KB", "MB", "GB", "TB"]
    i = 0
    while size_bytes >= 1024 and i < len(units)-1:
        size_bytes /= 1024.0
        i += 1
    return f"{size_bytes:.1f} {units[i]}"

def get_goinfre_path():
    user = os.environ.get('USER', 'user')
    paths = [
        f"/goinfre/{user}",
        f"/sgoinfre/{user}",
        f"/Volumes/Storage/goinfre/{user}",
        str(Path.home() / "goinfre"),
    ]
    for p in paths:
        if os.path.exists(os.path.dirname(p)):
            return p
    return f"/tmp/goinfre_{user}"

def log_audit(action, target, details=''):
    try:
        log_file = Path.home() / '.ft_ncdu_cleanup.log'
        ts = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        with open(log_file, 'a') as f:
            f.write(f'[{ts}] {action}: {target} {details}\n')
    except Exception:
        pass

def launch_process(command, cwd=None, shell=False, wait=False):
    if subprocess is not None:
        if wait:
            return subprocess.call(command, cwd=cwd, shell=shell)
        process = subprocess.Popen(command, cwd=cwd, shell=shell)
        threading.Thread(target=process.wait, daemon=True).start()
        return 0
    pid = os.fork()
    if pid == 0:
        try:
            if cwd:
                os.chdir(cwd)
            if shell:
                os.execl('/bin/sh', 'sh', '-c', command)
            os.execvpe(command[0], command, os.environ)
        except OSError:
            os._exit(127)
    if wait:
        _, status = os.waitpid(pid, 0)
        if os.WIFEXITED(status):
            return os.WEXITSTATUS(status)
        return 128 + os.WTERMSIG(status)
    threading.Thread(target=os.waitpid, args=(pid, 0), daemon=True).start()
    return 0

# ----------------- CLASSES -----------------

class FileNode:
    def __init__(self, path, is_dir, parent=None):
        self.path = Path(path)
        self.name = self.path.name
        self.is_dir = is_dir
        self.parent = parent
        self.size = 0
        self.items = 0
        self.children = []
        self.mtime = 0
        self.marked = False
        self.is_symlink = False
        self.target = ""
        self.broken = False

    def get_type_str(self):
        if self.is_symlink: return "Link"
        return "Dir" if self.is_dir else "File"

class Scanner(threading.Thread):
    def __init__(self, path, callback, update_callback):
        super().__init__(daemon=True)
        self.root_path = Path(path).resolve()
        self.callback = callback
        self.update_callback = update_callback
        self.cancelled = False
        self.root_dev = None

    def run(self):
        try:
            self.root_dev = self.root_path.stat().st_dev
            root_node = FileNode(self.root_path, True)
            self._scan_dir(self.root_path, root_node)
            if not self.cancelled:
                self.callback(root_node)
        except Exception as e:
            print(f"Scan error: {e}")
            if not self.cancelled:
                self.callback(None)

    def _scan_dir(self, path, node):
        if self.cancelled: return
        try:
            entries = list(os.scandir(path))
            node.items = len(entries)
            
            for i, entry in enumerate(entries):
                if self.cancelled: return
                
                if i % 50 == 0:
                    self.update_callback(str(path))

                child = FileNode(entry.path, entry.is_dir(), node)
                child.is_symlink = entry.is_symlink()
                
                try:
                    stat_info = entry.stat(follow_symlinks=False)
                    child.mtime = stat_info.st_mtime
                    if child.is_symlink:
                        try:
                            child.target = os.readlink(entry.path)
                            child.size = stat_info.st_size
                            child.broken = not os.path.exists(entry.path)
                        except: pass
                    else:
                        child.size = stat_info.st_size

                    if child.is_dir and not child.is_symlink:
                        if stat_info.st_dev == self.root_dev:
                            self._scan_dir(entry.path, child)
                            node.size += child.size
                            node.items += child.items
                except OSError:
                    pass
                
                node.children.append(child)
                if not child.is_dir or child.is_symlink:
                    node.size += child.size
        except OSError:
            pass

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ft_ncdu v2.0.0 — Disk Explorer (42/1337 Edition)")
        self.geometry("1100x700")
        self.configure(bg=BG_MAIN)
        
        self.current_path = Path.home()
        self.current_node = None
        self.scanner = None
        self.scan_generation = 0
        self.messages = queue.Queue()
        self.show_hidden = False
        self.sort_col = "size"
        self.sort_desc = True
        self.nodes_map = {}
        
        self.setup_style()
        self.create_menu()
        self.create_layout()
        self.bind_events()
        self.protocol("WM_DELETE_WINDOW", self.close)
        self.after(80, self.drain_messages)
        
        self.scan_path(self.current_path)

    def setup_style(self):
        style = ttk.Style(self)
        style.theme_use('clam')
        
        style.configure(".", background=BG_MAIN, foreground=FG_PRIMARY, fieldbackground=BG_MAIN)
        style.configure("TFrame", background=BG_MAIN)
        style.configure("Panel.TFrame", background=BG_PANEL)
        style.configure("Header.TLabel", background=BG_MAIN, foreground=ACCENT_HEADER, font=("Helvetica", 12, "bold"))
        style.configure("Action.TButton", background=BG_BUTTON, foreground=FG_BUTTON, padding=5)
        
        style.configure("Treeview", 
            background=BG_PANEL, fieldbackground=BG_PANEL, foreground=FG_PRIMARY,
            rowheight=25, borderwidth=0)
        style.map("Treeview", background=[('selected', BG_SELECTION)])
        style.configure("Treeview.Heading", background=BG_HEADER, foreground=FG_PRIMARY, font=("Helvetica", 10, "bold"))

        style.configure("TLabelframe", background=BG_PANEL, foreground=ACCENT_TITLE)
        style.configure("TLabelframe.Label", background=BG_PANEL, foreground=ACCENT_TITLE, font=("Helvetica", 11, "bold"))

    def create_menu(self):
        menubar = tk.Menu(self, bg=BG_HEADER, fg=FG_PRIMARY, bd=0)
        
        # File Menu
        file_menu = tk.Menu(menubar, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
        file_menu.add_command(label="Open Folder...", accelerator="Ctrl+O", command=self.action_open_folder)
        file_menu.add_command(label="Home", accelerator="Ctrl+H", command=lambda: self.scan_path(Path.home()))
        file_menu.add_separator()
        file_menu.add_command(label="Refresh", accelerator="F5", command=lambda: self.scan_path(self.current_path))
        file_menu.add_separator()
        file_menu.add_command(label="Exit", accelerator="Ctrl+Q", command=self.quit)
        menubar.add_cascade(label="File", menu=file_menu)
        
        # Edit Menu
        edit_menu = tk.Menu(menubar, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
        edit_menu.add_command(label="Mark Selected", accelerator="Space", command=self.action_mark_selected)
        edit_menu.add_command(label="Mark All", accelerator="Ctrl+A", command=self.action_mark_all)
        edit_menu.add_command(label="Unmark All", accelerator="Ctrl+Shift+A", command=self.action_unmark_all)
        edit_menu.add_command(label="Invert Marks", accelerator="Ctrl+I", command=self.action_invert_marks)
        menubar.add_cascade(label="Edit", menu=edit_menu)

        # View Menu
        view_menu = tk.Menu(menubar, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
        view_menu.add_checkbutton(label="Show Hidden Files", accelerator="Ctrl+.", command=self.action_toggle_hidden)
        view_menu.add_separator()
        view_menu.add_command(label="Sort by Size (Desc)", command=lambda: self.sort_table("size", True))
        view_menu.add_command(label="Sort by Size (Asc)", command=lambda: self.sort_table("size", False))
        view_menu.add_command(label="Sort by Name", command=lambda: self.sort_table("name"))
        view_menu.add_command(label="Sort by Date", command=lambda: self.sort_table("mtime"))
        menubar.add_cascade(label="View", menu=view_menu)

        # Actions Menu
        actions_menu = tk.Menu(menubar, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
        actions_menu.add_command(label="Delete Selected", accelerator="Delete", command=self.action_delete)
        actions_menu.add_separator()
        actions_menu.add_command(label="Cleaning Presets...", accelerator="Ctrl+P", command=self.show_cleaning_presets)
        actions_menu.add_command(label="Nuke Junk", accelerator="Ctrl+K", command=self.action_nuke_junk)
        actions_menu.add_command(label="Docker Prune", command=self.action_docker_prune)
        menubar.add_cascade(label="Actions", menu=actions_menu)

        # Cluster Menu
        cluster_menu = tk.Menu(menubar, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
        cluster_menu.add_command(label="Symlink to Goinfre", accelerator="Ctrl+S", command=self.action_symlink_goinfre)
        cluster_menu.add_command(label="Restore from Goinfre", accelerator="Ctrl+U", command=self.action_restore_goinfre)
        cluster_menu.add_separator()
        cluster_menu.add_command(label="Heal Symlinks", accelerator="Ctrl+Shift+H", command=self.action_heal_symlinks)
        cluster_menu.add_command(label="Bootstrap to Goinfre", accelerator="Ctrl+B", command=self.action_bootstrap)
        cluster_menu.add_separator()
        cluster_menu.add_command(label="Empty Trash", command=self.action_empty_trash)
        cluster_menu.add_command(label="Inject .zshrc Exports", command=self.action_inject_exports)
        menubar.add_cascade(label="Cluster", menu=cluster_menu)

        # Tools Menu
        tools_menu = tk.Menu(menubar, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
        tools_menu.add_command(label="Preview File", accelerator="Ctrl+Alt+P", command=self.action_preview)
        tools_menu.add_command(label="Open in Editor", accelerator="Ctrl+E", command=self.action_open_editor)
        tools_menu.add_command(label="Open Terminal", accelerator="Ctrl+T", command=self.action_open_terminal)
        tools_menu.add_command(label="Custom Command...", command=self.action_custom_command)
        tools_menu.add_separator()
        tools_menu.add_command(label="Export Report", accelerator="Ctrl+R", command=self.action_export_report)
        menubar.add_cascade(label="Tools", menu=tools_menu)

        self.config(menu=menubar)

    def create_layout(self):
        # Toolbar
        toolbar = ttk.Frame(self)
        toolbar.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(toolbar, text="⮜ Back", style="Action.TButton", command=self.action_back).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="⌂ Home", style="Action.TButton", command=lambda: self.scan_path(Path.home())).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="↻ Refresh", style="Action.TButton", command=lambda: self.scan_path(self.current_path)).pack(side=tk.LEFT, padx=2)
        
        self.search_var = tk.StringVar()
        self.search_var.trace("w", lambda name, index, mode: self.refresh_table())
        ttk.Entry(toolbar, textvariable=self.search_var, width=30).pack(side=tk.RIGHT, padx=5)
        tk.Label(toolbar, text="Search:", bg=BG_MAIN, fg=FG_PRIMARY).pack(side=tk.RIGHT)
        
        # Path Bar
        path_frame = ttk.Frame(self)
        path_frame.pack(fill=tk.X, padx=5, pady=2)
        self.path_label = tk.Label(path_frame, text="", bg=BG_MAIN, fg=ACCENT_HEADER, font=("Consolas", 12))
        self.path_label.pack(side=tk.LEFT)
        
        # Storage Gauge
        gauge_frame = ttk.Frame(self)
        gauge_frame.pack(fill=tk.X, padx=5, pady=5)
        self.gauge_canvas = tk.Canvas(gauge_frame, height=20, bg=BG_PANEL, highlightthickness=0)
        self.gauge_canvas.pack(fill=tk.X)
        self.update_storage_gauge()
        
        # Main PanedWindow
        self.paned = ttk.PanedWindow(self, orient=tk.HORIZONTAL)
        self.paned.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Left Panel (Table)
        table_frame = ttk.Frame(self.paned)
        self.paned.add(table_frame, weight=3)
        
        cols = ("mark", "type", "name", "size", "items", "mtime")
        self.tree = ttk.Treeview(table_frame, columns=cols, show="headings", selectmode="extended")
        
        self.tree.heading("mark", text="✓")
        self.tree.heading("type", text="Type")
        self.tree.heading("name", text="Name", command=lambda: self.sort_table("name"))
        self.tree.heading("size", text="Size", command=lambda: self.sort_table("size"))
        self.tree.heading("items", text="Items")
        self.tree.heading("mtime", text="Modified", command=lambda: self.sort_table("mtime"))
        
        self.tree.column("mark", width=30, anchor=tk.CENTER)
        self.tree.column("type", width=50, anchor=tk.CENTER)
        self.tree.column("name", width=300, anchor=tk.W)
        self.tree.column("size", width=100, anchor=tk.E)
        self.tree.column("items", width=80, anchor=tk.E)
        self.tree.column("mtime", width=150, anchor=tk.W)
        
        vsb = ttk.Scrollbar(table_frame, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=vsb.set)
        
        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        vsb.pack(side=tk.RIGHT, fill=tk.Y)
        
        # Right Panel (Inspector)
        inspector_frame = ttk.Frame(self.paned, style="Panel.TFrame")
        self.paned.add(inspector_frame, weight=1)
        
        self.lbl_frame = ttk.LabelFrame(inspector_frame, text="Inspector")
        self.lbl_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.info_text = tk.Text(self.lbl_frame, bg=BG_PANEL, fg=FG_PRIMARY, bd=0, 
                                 font=("Consolas", 10), state=tk.DISABLED, wrap=tk.WORD)
        self.info_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        btn_frame = ttk.Frame(self.lbl_frame, style="Panel.TFrame")
        btn_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(btn_frame, text="Preview", command=self.action_preview).pack(fill=tk.X, pady=2)
        ttk.Button(btn_frame, text="Terminal", command=self.action_open_terminal).pack(fill=tk.X, pady=2)
        ttk.Button(btn_frame, text="Symlink to Goinfre", command=self.action_symlink_goinfre).pack(fill=tk.X, pady=2)
        ttk.Button(btn_frame, text="Delete", command=self.action_delete).pack(fill=tk.X, pady=2)
        
        # Status Bar
        self.status_var = tk.StringVar(value="Ready")
        status_bar = tk.Label(self, textvariable=self.status_var, bg=BG_HEADER, fg=FG_PRIMARY, anchor=tk.W)
        status_bar.pack(fill=tk.X, side=tk.BOTTOM)

    def bind_events(self):
        self.tree.bind("<Double-1>", self.on_double_click)
        self.tree.bind("<Return>", self.on_double_click)
        self.tree.bind("<<TreeviewSelect>>", self.on_select)
        self.tree.bind("<Button-3>", self.show_context_menu)
        self.tree.bind("<space>", lambda e: self.action_mark_selected())
        
        self.bind("<Control-h>", lambda e: self.scan_path(Path.home()))
        self.bind("<Control-o>", lambda e: self.action_open_folder())
        self.bind("<F5>", lambda e: self.scan_path(self.current_path))
        self.bind("<Delete>", lambda e: self.action_delete())
        self.bind("<Control-a>", lambda e: self.action_mark_all())
        self.bind("<Control-A>", lambda e: self.action_unmark_all())
        self.bind("<Control-i>", lambda e: self.action_invert_marks())

    def update_storage_gauge(self):
        try:
            total, used, free = shutil.disk_usage(Path.home())
            pct = used / total
            
            self.gauge_canvas.delete("all")
            w = self.gauge_canvas.winfo_width()
            if w <= 1: w = 1100
            
            color = ACCENT_GREEN
            if pct > 0.85: color = ACCENT_RED
            elif pct > 0.60: color = ACCENT_YELLOW
            
            self.gauge_canvas.create_rectangle(0, 0, w * pct, 20, fill=color, outline="")
            self.gauge_canvas.create_text(w/2, 10, text=f"{format_size(used)} / {format_size(total)} ({pct*100:.1f}%)", fill=BG_MAIN, font=("Helvetica", 10, "bold"))
        except:
            pass
        self.after(5000, self.update_storage_gauge)

    # ----------------- SCANNING LOGIC -----------------

    def scan_path(self, path):
        if self.scanner and self.scanner.is_alive():
            self.scanner.cancelled = True
        
        self.current_path = Path(path).resolve()
        self.path_label.config(text=str(self.current_path))
        self.tree.delete(*self.tree.get_children())
        self.status_var.set("Scanning...")

        self.scan_generation += 1
        generation = self.scan_generation
        self.scanner = Scanner(
            self.current_path,
            lambda root: self.messages.put(("complete", generation, root)),
            lambda scanned: self.messages.put(("update", generation, scanned)),
        )
        self.scanner.start()

    def drain_messages(self):
        try:
            while True:
                kind, generation, payload = self.messages.get_nowait()
                if generation != self.scan_generation:
                    continue
                if kind == "complete":
                    self.on_scan_complete(payload)
                else:
                    self.on_scan_update(payload)
        except queue.Empty:
            pass
        self.after(80, self.drain_messages)

    def close(self):
        if self.scanner and self.scanner.is_alive():
            self.scanner.cancelled = True
        self.destroy()

    def on_scan_update(self, path_str):
        self.status_var.set(f"Scanning: {path_str}")

    def on_scan_complete(self, root_node):
        if not root_node:
            self.status_var.set("Scan failed or cancelled.")
            return
            
        self.current_node = root_node
        self.status_var.set(f"Scan complete. {root_node.items} items. Total size: {format_size(root_node.size)}")
        self.refresh_table()

    def refresh_table(self):
        if not self.current_node: return
        self.tree.delete(*self.tree.get_children())
        self.nodes_map.clear()
        
        query = self.search_var.get().lower()
        
        children = self.current_node.children
        if not self.show_hidden:
            children = [c for c in children if not c.name.startswith('.')]
            
        if query:
            children = [c for c in children if query in c.name.lower()]
            
        # Sorting
        rev = self.sort_desc
        if self.sort_col == "size":
            children.sort(key=lambda x: x.size, reverse=rev)
        elif self.sort_col == "name":
            children.sort(key=lambda x: x.name.lower(), reverse=rev)
        elif self.sort_col == "mtime":
            children.sort(key=lambda x: x.mtime, reverse=rev)
            
        for child in children:
            mark = "✓" if child.marked else ""
            type_str = child.get_type_str()
            try:
                mtime_str = datetime.fromtimestamp(child.mtime).strftime('%Y-%m-%d %H:%M') if child.mtime > 0 else ""
            except (OSError, ValueError, OverflowError):
                mtime_str = ""
            
            iid = self.tree.insert("", "end", values=(
                mark, type_str, child.name, format_size(child.size), child.items, mtime_str
            ))
            self.nodes_map[iid] = child

    def sort_table(self, col, desc=None):
        if desc is None:
            if self.sort_col == col:
                self.sort_desc = not self.sort_desc
            else:
                self.sort_desc = True
        else:
            self.sort_desc = desc
        self.sort_col = col
        self.refresh_table()

    def on_select(self, event):
        sel = self.tree.selection()
        if not sel: return
        
        node = self.nodes_map.get(sel[0])
        if not node: return
        
        self.info_text.config(state=tk.NORMAL)
        self.info_text.delete(1.0, tk.END)
        
        info = f"Name: {node.name}\n"
        info += f"Path: {node.path}\n"
        info += f"Size: {format_size(node.size)} ({node.size} bytes)\n"
        info += f"Type: {node.get_type_str()}\n"
        info += f"Items: {node.items}\n"
        info += f"Modified: {datetime.fromtimestamp(node.mtime).strftime('%Y-%m-%d %H:%M:%S')}\n"
        
        if node.is_symlink:
            info += f"\nSymlink Target:\n{node.target}\n"
            if node.broken:
                info += "[!] BROKEN SYMLINK\n"
            elif "goinfre" in node.target:
                info += "[i] Points to Goinfre\n"
                
        try:
            stat_info = node.path.lstat()
            perms = stat.filemode(stat_info.st_mode)
            octal = oct(stat_info.st_mode)[-4:]
            info += f"\nPermissions: {perms} ({octal})"
        except: pass
        
        self.info_text.insert(tk.END, info)
        self.info_text.config(state=tk.DISABLED)

    def on_double_click(self, event):
        sel = self.tree.selection()
        if not sel: return
        node = self.nodes_map.get(sel[0])
        if node and node.is_dir and not node.is_symlink:
            self.scan_path(node.path)

    def action_back(self):
        if self.current_path != Path(self.current_path.anchor):
            self.scan_path(self.current_path.parent)

    def show_context_menu(self, event):
        iid = self.tree.identify_row(event.y)
        if iid:
            self.tree.selection_set(iid)
            menu = tk.Menu(self, tearoff=0, bg=BG_PANEL, fg=FG_PRIMARY)
            menu.add_command(label="Open", command=self.on_double_click)
            menu.add_separator()
            menu.add_command(label="Mark / Unmark", command=self.action_mark_selected)
            menu.add_command(label="Delete", command=self.action_delete)
            menu.add_separator()
            menu.add_command(label="Preview File", command=self.action_preview)
            menu.add_command(label="Open in Editor", command=self.action_open_editor)
            menu.add_separator()
            menu.add_command(label="Symlink to Goinfre", command=self.action_symlink_goinfre)
            menu.add_command(label="Restore from Goinfre", command=self.action_restore_goinfre)
            menu.tk_popup(event.x_root, event.y_root)

    # ----------------- OPERATIONS -----------------

    def action_toggle_hidden(self):
        self.show_hidden = not self.show_hidden
        self.refresh_table()

    def action_mark_selected(self):
        for iid in self.tree.selection():
            node = self.nodes_map.get(iid)
            if node:
                node.marked = not node.marked
        self.refresh_table()

    def action_mark_all(self):
        for node in self.nodes_map.values():
            node.marked = True
        self.refresh_table()
        
    def action_unmark_all(self):
        for node in self.nodes_map.values():
            node.marked = False
        self.refresh_table()
        
    def action_invert_marks(self):
        for node in self.nodes_map.values():
            node.marked = not node.marked
        self.refresh_table()

    def action_open_folder(self):
        folder = filedialog.askdirectory(initialdir=self.current_path)
        if folder:
            self.scan_path(folder)

    def action_delete(self):
        to_delete = [n for n in self.nodes_map.values() if n.marked]
        if not to_delete:
            sel = self.tree.selection()
            if sel:
                to_delete = [self.nodes_map[sel[0]]]
        
        if not to_delete: return
        
        # Check protection
        for node in to_delete:
            rel = node.path.relative_to(Path.home()) if str(node.path).startswith(str(Path.home())) else node.path
            for p in PROTECTED_PATHS:
                if str(rel) == p or str(rel).startswith(p + '/'):
                    messagebox.showerror("Protected Path", f"Cannot delete protected path: {node.path}")
                    return

        if not messagebox.askyesno("Confirm Delete", f"Are you sure you want to delete {len(to_delete)} items?"):
            return
            
        for node in to_delete:
            try:
                if node.is_dir and not node.is_symlink:
                    shutil.rmtree(node.path)
                else:
                    node.path.unlink()
                log_audit("DELETE", node.path)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to delete {node.path}:\n{e}")
                
        self.scan_path(self.current_path)

    def action_symlink_goinfre(self):
        sel = self.tree.selection()
        if not sel: return
        node = self.nodes_map[sel[0]]
        
        if node.is_symlink:
            messagebox.showinfo("Info", "Item is already a symlink.")
            return
            
        try:
            relative_path = node.path.relative_to(Path.home())
        except ValueError:
            messagebox.showerror(
                "Outside Home",
                "Only items inside your home directory can be moved to goinfre.")
            return
        goinfre_path = Path(get_goinfre_path())
        dest = goinfre_path / relative_path
        
        if not messagebox.askyesno("Confirm Symlink", f"Move {node.name} to {dest} and symlink it?"):
            return
            
        try:
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.move(str(node.path), str(dest))
            node.path.symlink_to(dest)
            log_audit("SYMLINK", node.path, f"-> {dest}")
            self.scan_path(self.current_path)
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def action_restore_goinfre(self):
        sel = self.tree.selection()
        if not sel: return
        node = self.nodes_map[sel[0]]
        
        if not node.is_symlink or "goinfre" not in node.target:
            messagebox.showinfo("Info", "Item is not a symlink to goinfre.")
            return
            
        if not messagebox.askyesno("Confirm Restore", f"Restore {node.name} back to home?"):
            return
            
        try:
            target = node.path.resolve()
            node.path.unlink()
            shutil.move(str(target), str(node.path))
            log_audit("RESTORE", node.path)
            self.scan_path(self.current_path)
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def action_preview(self):
        sel = self.tree.selection()
        if not sel: return
        node = self.nodes_map[sel[0]]
        
        if node.is_dir and not node.is_symlink:
            return
            
        top = tk.Toplevel(self)
        top.title(f"Preview: {node.name}")
        top.geometry("800x600")
        
        text = tk.Text(top, bg=BG_PANEL, fg=FG_PRIMARY, font=("Consolas", 10))
        text.pack(fill=tk.BOTH, expand=True)
        
        try:
            with open(node.path, 'rb') as f:
                head = f.read(1024)
                if b'\0' in head:
                    text.insert(tk.END, "<Binary File>")
                else:
                    f.seek(0)
                    content = f.read(10000).decode('utf-8', errors='replace')
                    text.insert(tk.END, content)
        except Exception as e:
            text.insert(tk.END, f"Could not read file: {e}")
            
        text.config(state=tk.DISABLED)

    def action_open_editor(self):
        sel = self.tree.selection()
        if not sel: return
        node = self.nodes_map[sel[0]]
        editor = os.environ.get('EDITOR', '')
        if not editor:
            for candidate in ['nvim', 'vim', 'vi', 'nano']:
                if shutil.which(candidate):
                    editor = candidate
                    break
        if not editor:
            messagebox.showerror("Error", "No editor found. Set $EDITOR environment variable.")
            return
        command = shlex.split(editor)
        if not command or not shutil.which(command[0]):
            messagebox.showerror("Error", f"Editor '{editor}' not found.")
            return
        launch_process(command + [str(node.path)])

    def action_open_terminal(self):
        shell = os.environ.get('SHELL', '')
        if not shell or not os.path.isfile(shell):
            for candidate in ['/bin/bash', '/bin/zsh', '/bin/sh']:
                if os.path.isfile(candidate):
                    shell = candidate
                    break
        if not shell:
            messagebox.showerror("Error", "No shell found.")
            return
        if sys.platform == 'darwin':
            launch_process(['open', '-a', 'Terminal', str(self.current_path)])
            return
        terminals = [
            ('x-terminal-emulator', ['--working-directory', str(self.current_path)]),
            ('gnome-terminal', ['--working-directory', str(self.current_path)]),
            ('konsole', ['--workdir', str(self.current_path)]),
            ('xfce4-terminal', ['--working-directory', str(self.current_path)]),
            ('xterm', ['-e', shell, '-c',
                       f'cd {shlex.quote(str(self.current_path))}; exec {shlex.quote(shell)}']),
        ]
        for terminal, args in terminals:
            if shutil.which(terminal):
                launch_process([terminal] + args)
                return
        messagebox.showerror("Error", "No supported terminal emulator found.")

    def action_custom_command(self):
        sel = self.tree.selection()
        if not sel: return
        node = self.nodes_map[sel[0]]
        
        cmd = simpledialog.askstring("Custom Command", f"Run command on {node.name}:\n(Use {{}} as placeholder for path)")
        if cmd:
            full_cmd = cmd.replace("{}", f'"{node.path}"')
            launch_process(full_cmd, cwd=str(self.current_path), shell=True)

    def action_empty_trash(self):
        if messagebox.askyesno("Empty Trash", "Empty the desktop trash bins?"):
            launch_process(
                'rm -rf ~/.local/share/Trash/* ~/.Trash/* 2>/dev/null',
                shell=True, wait=True)
            log_audit("EMPTY_TRASH", "Trash")
            messagebox.showinfo("Success", "Trash emptied.")

    def action_inject_exports(self):
        zshrc = Path.home() / '.zshrc'
        try:
            content = ""
            if zshrc.exists():
                content = zshrc.read_text()
            if "# --- ft_ncdu paths v2 ---" in content:
                messagebox.showinfo("Info", "Exports already injected.")
                return
                
            exports = """
# --- ft_ncdu paths v2 ---
export HF_HOME="/goinfre/$USER/.cache/huggingface"
export TORCH_HOME="/goinfre/$USER/.cache/torch" 
export CARGO_HOME="/goinfre/$USER/.cargo"
export DOCKER_CONFIG="/goinfre/$USER/.docker"
alias goinfre='cd /goinfre/$USER'
# --- end ft_ncdu ---
"""
            with open(zshrc, 'a') as f:
                f.write(exports)
            messagebox.showinfo("Success", "Exports injected into ~/.zshrc")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def action_export_report(self):
        try:
            report_path = Path.cwd() / "quota_report.md"
            with open(report_path, "w") as f:
                f.write("# 1337 / 42 Cluster Storage Audit Report\n\n")
                f.write(f"**Date:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"**User:** {os.environ.get('USER', 'Unknown')}\n\n")
                
                total, used, free = shutil.disk_usage(Path.home())
                f.write(f"## Disk Usage (Home)\n")
                f.write(f"- Total: {format_size(total)}\n")
                f.write(f"- Used: {format_size(used)}\n")
                f.write(f"- Free: {format_size(free)}\n\n")
                
                f.write("## Top Largest Items (Current Directory)\n")
                f.write("| Rank | Type | Size | Name |\n")
                f.write("|------|------|------|------|\n")
                
                items = sorted([n for n in self.nodes_map.values()], key=lambda x: x.size, reverse=True)[:30]
                for i, node in enumerate(items, 1):
                    f.write(f"| {i} | {node.get_type_str()} | {format_size(node.size)} | {node.name} |\n")
                    
            messagebox.showinfo("Report Exported", f"Saved to {report_path}")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def action_nuke_junk(self):
        if messagebox.askyesno("Nuke Junk", "Find and delete node_modules, __pycache__, *.o, *.a in HOME?"):
            cmd = "find ~/ -type d -name 'node_modules' -prune -exec rm -rf {} +; "
            cmd += "find ~/ -type d -name '__pycache__' -prune -exec rm -rf {} +; "
            cmd += "find ~/ -type f \\( -name '*.o' -o -name '*.a' -o -name 'core.*' \\) -delete"
            launch_process(cmd, shell=True)
            log_audit("NUKE_JUNK", "HOME")
            messagebox.showinfo("Info", "Cleanup started in background.")

    def action_docker_prune(self):
        if messagebox.askyesno("Docker Prune", "Run 'docker system prune -a --volumes -f'?"):
            launch_process("docker system prune -a --volumes -f", shell=True)
            log_audit("DOCKER_PRUNE", "System")
            messagebox.showinfo("Info", "Docker prune started in background.")

    def action_heal_symlinks(self):
        if not messagebox.askyesno("Heal Symlinks", "Find and repair broken goinfre symlinks in $HOME?"):
            return
        home = Path.home()
        healed = 0
        checked = 0
        goinfre_patterns = ('/goinfre/', '/sgoinfre/', '/tmp/goinfre_')
        try:
            for root, dirs, files in os.walk(str(home)):
                depth = str(root).count(os.sep) - str(home).count(os.sep)
                if depth >= 4:
                    dirs.clear()
                    continue
                for name in dirs + files:
                    full = os.path.join(root, name)
                    if os.path.islink(full):
                        checked += 1
                        target = os.readlink(full)
                        if any(p in target for p in goinfre_patterns):
                            if not os.path.exists(full):
                                try:
                                    os.makedirs(target, mode=0o700, exist_ok=True)
                                    healed += 1
                                    log_audit("HEAL", full, f"-> {target}")
                                except OSError:
                                    pass
        except OSError:
            pass
        messagebox.showinfo("Heal Complete", f"Checked {checked} symlinks.\nRepaired {healed} broken targets.")

    def action_bootstrap(self):
        if not messagebox.askyesno("Bootstrap", "Relocate heavy directories to Goinfre?"):
            return
            
        goinfre = Path(get_goinfre_path())
        count = 0
        for target in BOOTSTRAP_TARGETS:
            src = Path.home() / target
            if src.exists() and not src.is_symlink():
                dst = goinfre / target
                try:
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    if not dst.exists():
                        shutil.move(str(src), str(dst))
                        src.symlink_to(dst)
                        count += 1
                        log_audit("BOOTSTRAP", src, f"-> {dst}")
                except Exception:
                    pass
        messagebox.showinfo("Done", f"Bootstrapped {count} directories.")
        self.scan_path(self.current_path)

    # ----------------- CLEANING PRESETS -----------------

    def show_cleaning_presets(self):
        top = tk.Toplevel(self)
        top.title("Cleaning Presets")
        top.geometry("700x500")
        top.configure(bg=BG_MAIN)
        
        canvas = tk.Canvas(top, bg=BG_MAIN, highlightthickness=0)
        scroll = ttk.Scrollbar(top, orient="vertical", command=canvas.yview)
        frame = ttk.Frame(canvas)
        
        canvas.create_window((0, 0), window=frame, anchor="nw")
        canvas.configure(yscrollcommand=scroll.set)
        
        canvas.pack(side="left", fill="both", expand=True)
        scroll.pack(side="right", fill="y")
        
        def run_preset(p):
            if messagebox.askyesno("Run Preset", f"Run {p['title']}?"):
                home = str(Path.home())
                if p['key'] == '9':
                    cmds = [pr['command'].format(home=home) for pr in CLEAN_PRESETS[:-1]]
                    cmd = " ; ".join(cmds)
                else:
                    cmd = p['command'].format(home=home)
                
                launch_process(cmd, shell=True, wait=True)
                log_audit("PRESET", p['title'])
                messagebox.showinfo("Done", f"Executed {p['title']}")
                self.scan_path(self.current_path)

        for p in CLEAN_PRESETS:
            pf = ttk.Frame(frame, style="Panel.TFrame")
            pf.pack(fill=tk.X, padx=10, pady=5)
            
            ttk.Label(pf, text=f"[{p['key']}] {p['title']}", font=("Helvetica", 12, "bold"), foreground=ACCENT_TITLE, background=BG_PANEL).pack(anchor=tk.W, padx=5, pady=2)
            ttk.Label(pf, text=p['desc'], foreground=FG_SECONDARY, background=BG_PANEL).pack(anchor=tk.W, padx=5)
            
            ttk.Button(pf, text="Run", command=lambda x=p: run_preset(x)).pack(side=tk.RIGHT, padx=5, pady=5)
            
        frame.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

def main():
    try:
        app = App()
        app.mainloop()
    except tk.TclError as error:
        print(f"ft_ncdu-gui: unable to start Tk: {error}", file=sys.stderr)
        return 1
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
