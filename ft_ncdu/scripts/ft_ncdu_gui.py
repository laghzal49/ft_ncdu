#!/usr/bin/env python3
"""Portable Tk interface for ft_ncdu."""

import os
import queue
import shutil
import stat
import sys
import threading
from datetime import datetime
from pathlib import Path
import tkinter as tk
from tkinter import filedialog, messagebox, ttk


def format_size(value):
    size = float(value)
    for unit in ("B", "KB", "MB", "GB", "TB"):
        if size < 1024.0 or unit == "TB":
            return f"{size:.1f} {unit}"
        size /= 1024.0


def disk_size(info):
    blocks = getattr(info, "st_blocks", 0)
    return blocks * 512 if blocks else info.st_size


class DiskBrowser:
    def __init__(self, root):
        self.root = root
        self.current = Path.home()
        self.entries = []
        self.messages = queue.Queue()
        self.generation = 0
        self.sort_key = "size"
        self.sort_reverse = True
        self.show_hidden = tk.BooleanVar(value=False)
        self.search_text = tk.StringVar()
        self.path_text = tk.StringVar()
        self.status_text = tk.StringVar(value="Ready")
        self.space_text = tk.StringVar()
        self._build_window()
        self.root.after(80, self._drain_messages)
        self.scan(self.current)

    def _build_window(self):
        self.root.title("ft_ncdu - Disk Explorer")
        self.root.geometry("1040x680")
        self.root.minsize(700, 440)
        self.root.configure(bg="#11151b")
        self._configure_style()
        shell = ttk.Frame(self.root, padding=18, style="Root.TFrame")
        shell.pack(fill="both", expand=True)
        self._build_header(shell)
        self._build_table(shell)
        self._build_footer(shell)
        self.root.bind("<Alt-Left>", lambda _event: self.go_parent())
        self.root.bind("<F5>", lambda _event: self.scan(self.current))

    def _configure_style(self):
        style = ttk.Style()
        style.theme_use("clam")
        style.configure("Root.TFrame", background="#11151b")
        style.configure("Panel.TFrame", background="#181e27")
        style.configure("Title.TLabel", background="#11151b",
                        foreground="#f4b860", font=("TkDefaultFont", 18, "bold"))
        style.configure("Meta.TLabel", background="#11151b", foreground="#9aa8b8")
        style.configure("TButton", background="#273142", foreground="#edf2f7",
                        padding=(12, 8), borderwidth=0)
        style.map("TButton", background=[("active", "#35445a")])
        style.configure("Treeview", background="#181e27", fieldbackground="#181e27",
                        foreground="#dce4ee", rowheight=30, borderwidth=0)
        style.configure("Treeview.Heading", background="#222b38", foreground="#9ccfd8",
                        relief="flat", padding=8)
        style.map("Treeview", background=[("selected", "#2b5d67")],
                  foreground=[("selected", "#ffffff")])
        style.configure("TEntry", fieldbackground="#202833", foreground="#edf2f7",
                        insertcolor="#edf2f7", padding=8)
        style.configure("TCheckbutton", background="#11151b", foreground="#b9c5d2")

    def _build_header(self, parent):
        top = ttk.Frame(parent, style="Root.TFrame")
        top.pack(fill="x", pady=(0, 14))
        ttk.Label(top, text="ft_ncdu", style="Title.TLabel").pack(side="left")
        ttk.Label(top, text="visual disk explorer", style="Meta.TLabel").pack(
            side="left", padx=12, pady=(7, 0))
        ttk.Label(top, textvariable=self.space_text, style="Meta.TLabel").pack(
            side="right", pady=(7, 0))
        nav = ttk.Frame(parent, style="Root.TFrame")
        nav.pack(fill="x", pady=(0, 12))
        ttk.Button(nav, text="Back", command=self.go_parent).pack(side="left")
        ttk.Button(nav, text="Home", command=lambda: self.scan(Path.home())).pack(
            side="left", padx=(8, 0))
        ttk.Button(nav, text="Choose", command=self.choose_directory).pack(
            side="left", padx=8)
        path_entry = ttk.Entry(nav, textvariable=self.path_text)
        path_entry.pack(side="left", fill="x", expand=True)
        path_entry.bind("<Return>", lambda _event: self.open_path())
        search = ttk.Entry(nav, textvariable=self.search_text, width=22)
        search.pack(side="left", padx=(8, 0))
        search.insert(0, "")
        self.search_text.trace_add("write", lambda *_args: self.render_entries())

    def _build_table(self, parent):
        panel = ttk.Frame(parent, style="Panel.TFrame")
        panel.pack(fill="both", expand=True)
        columns = ("kind", "size", "items", "modified")
        self.tree = ttk.Treeview(panel, columns=columns, show="tree headings")
        self.tree.heading("#0", text="Name", command=lambda: self.set_sort("name"))
        self.tree.heading("kind", text="Type", command=lambda: self.set_sort("kind"))
        self.tree.heading("size", text="Disk size", command=lambda: self.set_sort("size"))
        self.tree.heading("items", text="Items", command=lambda: self.set_sort("items"))
        self.tree.heading("modified", text="Modified",
                          command=lambda: self.set_sort("mtime"))
        self.tree.column("#0", width=430, minwidth=180)
        self.tree.column("kind", width=80, anchor="center")
        self.tree.column("size", width=120, anchor="e")
        self.tree.column("items", width=90, anchor="e")
        self.tree.column("modified", width=155, anchor="center")
        scrollbar = ttk.Scrollbar(panel, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        self.tree.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
        self.tree.bind("<Double-1>", lambda _event: self.open_selected())
        self.tree.bind("<Return>", lambda _event: self.open_selected())

    def _build_footer(self, parent):
        footer = ttk.Frame(parent, style="Root.TFrame")
        footer.pack(fill="x", pady=(12, 0))
        ttk.Label(footer, textvariable=self.status_text, style="Meta.TLabel").pack(
            side="left")
        ttk.Checkbutton(footer, text="Show hidden", variable=self.show_hidden,
                        command=lambda: self.scan(self.current)).pack(side="right")
        ttk.Button(footer, text="Refresh", command=lambda: self.scan(self.current)).pack(
            side="right", padx=8)

    def scan(self, path):
        try:
            resolved = Path(path).expanduser().resolve()
            if not resolved.is_dir():
                raise NotADirectoryError(str(resolved))
        except (OSError, RuntimeError) as error:
            messagebox.showerror("Cannot open folder", str(error))
            return
        self.current = resolved
        self.path_text.set(str(resolved))
        self.generation += 1
        generation = self.generation
        self.entries = []
        self.tree.delete(*self.tree.get_children())
        self.status_text.set("Scanning...")
        show_hidden = self.show_hidden.get()
        threading.Thread(target=self._scan_worker,
                         args=(resolved, generation, show_hidden),
                         daemon=True).start()

    def _scan_worker(self, path, generation, show_hidden):
        try:
            root_device = path.stat().st_dev
            children = list(os.scandir(path))
            if not show_hidden:
                children = [item for item in children if not item.name.startswith(".")]
            for child in children:
                if generation != self.generation:
                    return
                entry = self._measure_entry(child, root_device, generation)
                if entry:
                    self.messages.put(("entry", generation, entry))
            usage = shutil.disk_usage(path)
            self.messages.put(("done", generation, usage))
        except OSError as error:
            self.messages.put(("error", generation, str(error)))

    def _measure_entry(self, child, root_device, generation):
        try:
            info = child.stat(follow_symlinks=False)
            kind = "Link" if child.is_symlink() else "File"
            total = disk_size(info)
            items = 1
            if child.is_dir(follow_symlinks=False):
                kind = "Folder"
                total, items = self._walk_size(child.path, root_device, generation)
            return {"name": child.name, "path": child.path, "kind": kind,
                    "size": total, "items": items, "mtime": info.st_mtime}
        except OSError:
            return None

    def _walk_size(self, start, root_device, generation):
        total = 0
        items = 0
        stack = [start]
        while stack and generation == self.generation:
            current = stack.pop()
            try:
                for item in os.scandir(current):
                    info = item.stat(follow_symlinks=False)
                    total += disk_size(info)
                    items += 1
                    if (stat.S_ISDIR(info.st_mode) and not item.is_symlink()
                            and info.st_dev == root_device):
                        stack.append(item.path)
            except OSError:
                continue
        return total, items

    def _drain_messages(self):
        changed = False
        try:
            while True:
                kind, generation, payload = self.messages.get_nowait()
                if generation != self.generation:
                    continue
                if kind == "entry":
                    self.entries.append(payload)
                    changed = True
                elif kind == "done":
                    self.space_text.set(
                        f"{format_size(payload.free)} free of {format_size(payload.total)}")
                    self.status_text.set(f"{len(self.entries)} items")
                    changed = True
                else:
                    self.status_text.set(payload)
        except queue.Empty:
            pass
        if changed:
            self.render_entries()
        self.root.after(80, self._drain_messages)

    def render_entries(self):
        query = self.search_text.get().casefold()
        visible = [entry for entry in self.entries
                   if query in entry["name"].casefold()]
        visible.sort(key=lambda entry: entry[self.sort_key], reverse=self.sort_reverse)
        self.tree.delete(*self.tree.get_children())
        for entry in visible:
            modified = datetime.fromtimestamp(entry["mtime"]).strftime("%Y-%m-%d %H:%M")
            self.tree.insert("", "end", iid=entry["path"], text=entry["name"],
                             values=(entry["kind"], format_size(entry["size"]),
                                     entry["items"], modified))

    def set_sort(self, key):
        if self.sort_key == key:
            self.sort_reverse = not self.sort_reverse
        else:
            self.sort_key = key
            self.sort_reverse = key in {"size", "items", "mtime"}
        self.render_entries()

    def selected_path(self):
        selected = self.tree.selection()
        return Path(selected[0]) if selected else None

    def open_selected(self):
        path = self.selected_path()
        if path and path.is_dir() and not path.is_symlink():
            self.scan(path)

    def go_parent(self):
        self.scan(self.current.parent)

    def open_path(self):
        self.scan(self.path_text.get())

    def choose_directory(self):
        selected = filedialog.askdirectory(initialdir=self.current)
        if selected:
            self.scan(selected)

def main():
    try:
        root = tk.Tk()
        DiskBrowser(root)
        root.mainloop()
    except tk.TclError as error:
        print(f"ft_ncdu-gui: unable to start Tk: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
