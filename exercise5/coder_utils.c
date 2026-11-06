/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 17:42:44 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/30 18:47:40 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	*get_dongle_by_id(t_monitor *monitor, int id)
{
	int	i;

	while (i < monitor->info->nb_coders)
	{
		if (monitor->dongle_pool[i].id == id)
			return (&monitor->dongle_pool[i]);
		i++;
	}
	return (NULL);
}
