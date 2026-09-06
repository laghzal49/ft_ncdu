/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   report.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/08/27 10:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ncdu.h"

static void	write_top_consumers(FILE *file_handle)
{
	char		size_str[16];
	off_t		item_size;
	const char	*type_label;
	int			idx;

	pthread_mutex_lock(&g_state.lock);
	idx = 0;
	while (idx < g_state.count && idx < 30)
	{
		if (g_state.size_mode == SIZE_ACTUAL_DISK)
			item_size = g_state.entries[idx].disk_size;
		else
			item_size = g_state.entries[idx].size;
		format_size(item_size, size_str, sizeof(size_str));
		type_label = "FILE";
		if (g_state.entries[idx].type == TYPE_DIR)
			type_label = "DIR ";
		fprintf(file_handle, "| %02d | %s | %s | `%s` |\n",
			idx + 1, type_label, size_str, g_state.entries[idx].name);
		idx++;
	}
	pthread_mutex_unlock(&g_state.lock);
}

void	action_export_report(void)
{
	FILE			*file_handle;
	struct statvfs	fs_stats;
	char			tot_sz[16];
	char			used_sz[16];

	file_handle = fopen("quota_report.md", "w");
	if (!file_handle)
		return ;
	statvfs(g_state.current_dir, &fs_stats);
	format_size((off_t)fs_stats.f_blocks * fs_stats.f_frsize,
		tot_sz, sizeof(tot_sz));
	format_size((off_t)(fs_stats.f_blocks - fs_stats.f_bfree)
		* fs_stats.f_frsize, used_sz, sizeof(used_sz));
	fprintf(file_handle, "# 1337 / 42 Cluster Storage Audit Report\n\n"
		"**Target:** `%s` | **User:** `%s`\n**Quota:** %s / %s\n\n"
		"## Top 30 Consumers\n\n| # | Type | Size | Name |\n"
		"| :--- | :--- | :--- | :--- |\n",
		g_state.current_dir, g_state.username, used_sz, tot_sz);
	write_top_consumers(file_handle);
	fclose(file_handle);
	confirm_modal("SUCCESS", "Exported report to quota_report.md!");
}

int	run_cli_report(const char *target_path)
{
	struct statvfs	vfs;
	char			sz_tot[16];
	char			sz_used[16];
	char			sz_free[16];

	statvfs(target_path, &vfs);
	format_size((off_t)vfs.f_blocks * vfs.f_frsize, sz_tot, sizeof(sz_tot));
	format_size((off_t)(vfs.f_blocks - vfs.f_bfree) * vfs.f_frsize,
		sz_used, sizeof(sz_used));
	format_size((off_t)vfs.f_bfree * vfs.f_frsize, sz_free, sizeof(sz_free));
	printf("\n======================================================\n");
	printf("  \033[1;36m42 / 1337 CLUSTER STORAGE AUDIT REPORT\033[0m\n");
	printf("======================================================\n");
	printf("  Path  : %s\n", target_path);
	printf("  Quota : %s\n", sz_tot);
	printf("  Used  : %s\n", sz_used);
	printf("  Free  : %s\n", sz_free);
	printf("======================================================\n\n");
	return (0);
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
	printf("  -n, --dry-run     Simulate cleanup without deleting\n");
	printf("  --heal            Repair dangling goinfre symlinks\n");
	printf("  --bootstrap       Relocate heavy toolchains to goinfre\n");
	printf("  --report          Print quota summary to stdout\n\n");
	printf("Audit Log: ~/.ft_ncdu_cleanup.log\n");
}
