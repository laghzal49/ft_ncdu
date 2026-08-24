/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   report.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	write_top_consumers(FILE *fp)
{
	char		sz[16];
	off_t		item_sz;
	const char	*t_str;
	int			i;

	pthread_mutex_lock(&g_state.lock);
	i = 0;
	while (i < g_state.count && i < 30)
	{
		if (g_state.size_mode == SIZE_ACTUAL_DISK)
			item_sz = g_state.entries[i].disk_size;
		else
			item_sz = g_state.entries[i].size;
		format_size(item_sz, sz, sizeof(sz));
		t_str = "FILE";
		if (g_state.entries[i].type == TYPE_DIR)
			t_str = "DIR ";
		fprintf(fp, "| %02d | %s | %s | `%s` |\n",
			i + 1, t_str, sz, g_state.entries[i].name);
		i++;
	}
	pthread_mutex_unlock(&g_state.lock);
}

void	action_export_report(void)
{
	FILE			*fp;
	struct statvfs	vfs;
	char			sz_tot[16];
	char			sz_used[16];

	fp = fopen("quota_report.md", "w");
	if (!fp)
	{
		confirm_modal("ERROR", "Could not create quota_report.md!");
		return ;
	}
	statvfs(g_state.current_dir, &vfs);
	format_size((off_t)vfs.f_blocks * vfs.f_frsize, sz_tot, sizeof(sz_tot));
	format_size((off_t)(vfs.f_blocks - vfs.f_bfree) * vfs.f_frsize,
		sz_used, sizeof(sz_used));
	fprintf(fp, "# 1337 / 42 Cluster Storage Audit Report\n\n");
	fprintf(fp, "**Target:** `%s` | **User:** `%s`\n",
		g_state.current_dir, g_state.username);
	fprintf(fp, "**Quota:** %s used / %s total\n\n", sz_used, sz_tot);
	fprintf(fp, "## Top 30 Consumers\n\n| # | Type | Size | Name |\n");
	fprintf(fp, "| :--- | :--- | :--- | :--- |\n");
	write_top_consumers(fp);
	fclose(fp);
	confirm_modal("SUCCESS", "Exported report to quota_report.md!");
}

void	action_cycle_sort_mode(void)
{
	if (g_state.sort_mode == SORT_SIZE_DESC)
		g_state.sort_mode = SORT_SIZE_ASC;
	else if (g_state.sort_mode == SORT_SIZE_ASC)
		g_state.sort_mode = SORT_NAME_ASC;
	else if (g_state.sort_mode == SORT_NAME_ASC)
		g_state.sort_mode = SORT_MTIME_DESC;
	else
		g_state.sort_mode = SORT_SIZE_DESC;
	pthread_mutex_lock(&g_state.lock);
	qsort(g_state.entries, g_state.count,
		sizeof(t_file_entry), compare_entries);
	pthread_mutex_unlock(&g_state.lock);
	apply_filter();
}

void	print_cli_version(void)
{
	printf("%s v%s - 42 / 1337 Cluster Storage Suite\n",
		APP_NAME, APP_VERSION);
}

void	print_cli_help(const char *prog_name)
{
	printf("Usage: %s [OPTIONS] [PATH]\n\n", prog_name);
	printf("Options:\n");
	printf("  -h, --help        Show this help message\n");
	printf("  -v, --version     Show version information\n");
	printf("  -c, --clean       Run fast cluster cleaning headlessly\n");
	printf("  --heal            Repair dangling goinfre symlinks\n");
	printf("  --bootstrap       Relocate heavy toolchains to goinfre\n");
	printf("  --report          Print quota summary to stdout\n");
}
