/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:43:30 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:02:24 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Allocates space for the reachable dongles, which could be
 * determined by the bonus as all-reachable (from the center)
 * I am very limited in this aspect since I can not generate
 * random numbers.
 * 
 * If no bonus is active, the defaut behaviour is to allow only
 * 2 dongles to be reached (left and right indexes)
 */
static int	init_coder_next_dongles(t_monitor *monitor, int i)
{
	if (monitor->bonus)
	{
		monitor->coder_pool[i].next_dongles = ft_calloc(sizeof(t_dongle *), 2);
		if (!monitor->coder_pool[i].next_dongles)
			return (0);
		monitor->coder_pool[i].next_dongles[0] = &monitor->dongle_pool[i];
		monitor->coder_pool[i].next_dongles[1] = &monitor->dongle_pool[
			(i + 1) % monitor->nb_coders
		];
	}
	else
	{
		monitor->coder_pool[i].next_dongles = ft_calloc(sizeof(t_dongle *), 2);
		if (!monitor->coder_pool[i].next_dongles)
			return (0);
		monitor->coder_pool[i].next_dongles[0] = &monitor->dongle_pool[i];
		monitor->coder_pool[i].next_dongles[1] = &monitor->dongle_pool[
			(i + 1) % monitor->nb_coders
		];
	}
	return (1);
}

static int	init_coders(t_monitor *monitor)
{
	int	i;

	i = 0;
	monitor->coder_pool = ft_calloc(sizeof(t_coder), monitor->nb_coders);
	if (!monitor->coder_pool)
		return (0);
	while (i < monitor->nb_coders)
	{
		monitor->coder_pool[i].last_compile = 0;
		monitor->coder_pool[i].id = i + 1;
		monitor->coder_pool[i].last_compile = 0;
		monitor->coder_pool[i].mon = monitor;
		monitor->coder_pool[i].compile_count = 0;
		monitor->coder_pool[i].dongle_count = 0;
		monitor->coder_pool[i].state = TAKE;
		if (!init_coder_next_dongles(monitor, i))
			return (0);
		i++;
	}
	return (1);
}

static int	init_dongle_queue(t_monitor *monitor, int i)
{
	monitor->dongle_pool[i].queue_idx = 0;
	monitor->dongle_pool[i].queue_max = monitor->nb_coders;
	monitor->dongle_pool[i].queue = ft_calloc(sizeof(t_dongle_req),
			monitor->nb_coders);
	if (!monitor->dongle_pool[i].queue)
		return (0);
	return (1);
}

static int	init_dongles(t_monitor *monitor)
{
	int	i;

	monitor->dongle_pool = ft_calloc(sizeof(t_dongle), monitor->nb_coders);
	if (!monitor->dongle_pool)
		return (0);
	i = 0;
	while (i < monitor->nb_coders)
	{
		monitor->dongle_pool[i].id = i + 1;
		monitor->dongle_pool[i].cooldown_time = monitor->cooldown_time;
		monitor->dongle_pool[i].next_cooldown = 0;
		monitor->dongle_pool[i].owner = NULL;
		monitor->dongle_pool[i].mon = monitor;
		monitor->dongle_pool[i].seq = 0;
		pthread_mutex_init(&monitor->dongle_pool[i].mutex, NULL);
		pthread_cond_init(&monitor->dongle_pool[i].cond, NULL);
		if (!init_dongle_queue(monitor, i))
			return (0);
		i++;
	}
	return (1);
}

int	init_monitor(t_monitor *monitor)
{
	if (!init_dongles(monitor))
		return (0);
	if (!init_coders(monitor))
		return (free_and_destroy(monitor), 0);
	monitor->running = false;
	pthread_mutex_init(&monitor->log_mutex, NULL);
	pthread_mutex_init(&monitor->run_mutex, NULL);
	pthread_cond_init(&monitor->run_cond, NULL);
	return (1);
}
