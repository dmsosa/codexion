/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:52:23 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:37:09 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_and_destroy(t_monitor *monitor)
{
	int	i;

	i = -1;
	while (++i < monitor->nb_coders)
	{
		pthread_mutex_destroy(&monitor->dongle_pool[i].mutex);
		free(monitor->coder_pool[i].next_dongles);
		free(monitor->dongle_pool[i].queue);
	}
	free(monitor->coder_pool);
	free(monitor->dongle_pool);
	monitor->coder_pool = NULL;
	monitor->dongle_pool = NULL;
	pthread_mutex_destroy(&monitor->log_mutex);
	pthread_mutex_destroy(&monitor->run_mutex);
	pthread_cond_destroy(&monitor->run_cond);
}
