/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   presets.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#             */
/*   Updated: 2026/08/24 22:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRESETS_H
# define PRESETS_H

# include "ft_ncdu.h"

# define PRESET_COUNT 9

extern const t_clean_preset	g_clean_presets[PRESET_COUNT];

extern const char			*g_bootstrap_targets[];
extern const char			*g_protected_paths[];
extern const char			*g_shell_exports[];

#endif
