/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 17:07:57 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/30 17:22:04 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_program_info		info;
	t_monitor			monitor;

	if (argc < 2)
		print_exit_usage();
	memset(&info, 0, sizeof(info));
	parse_params(argc, argv, &info);
	monitor.info = &info;
	if (!init_monitor(&monitor))
	{
		free_and_destroy(&monitor);
		print_exit("Error creating coders pool");
	}
	monitor_run(&monitor);
	free_and_destroy(&monitor);
	return (0);
}

void	monitor_run(t_monitor *monitor)
{
	if (pthread_create(&monitor->tid, NULL, &monitor_routine, &monitor) != 0)
		print_exit("Error in pthread_create monitor");

	pthread_join(monitor->tid, NULL);
}

static void	init_coder_next_dongles(t_monitor *monitor, int i)
{
	if (monitor->info->bonus)
	{
		//all dongles are reachable
		monitor->coder_pool[i].next_dongles = malloc(sizeof(t_dongle *) * 2);
		monitor->coder_pool[i].next_dongles[0] = &monitor->dongle_pool[i];
		monitor->coder_pool[i].next_dongles[1] = &monitor->dongle_pool[
			(i + 1) % monitor->info->nb_coders
		];
	}
	else
	{
		//only 2 dongles are reachable
		monitor->coder_pool[i].next_dongles = malloc(sizeof(t_dongle *) * 2);
		monitor->coder_pool[i].next_dongles[0] = &monitor->dongle_pool[i];
		monitor->coder_pool[i].next_dongles[1] = &monitor->dongle_pool[
			(i + 1) % monitor->info->nb_coders
		];
	}
}

static int	init_coders(t_monitor *monitor)
{
	int	i;

	i = 0;
	monitor->coder_pool = ft_calloc(sizeof(t_coder), monitor->info->nb_coders);
	if (!monitor->coder_pool)
		return (0);
	while (i < monitor->info->nb_coders)
	{
		monitor->coder_pool[i].burned = false;
		monitor->coder_pool[i].burnout_time = monitor->info->burnout_time;
		monitor->coder_pool[i].compile_time = monitor->info->compile_time;
		monitor->coder_pool[i].debug_time = monitor->info->debug_time;
		monitor->coder_pool[i].refactor_time = monitor->info->refactor_time;
		monitor->coder_pool[i].req_compile = monitor->info->req_compile;
		monitor->coder_pool[i].last_compile = 0;
		monitor->coder_pool[i].mon = monitor;
		monitor->coder_pool[i].id = i + 1;
		init_coder_next_dongles(monitor, i);
		i++;
	}
	return (1);
}

static int	init_dongles(t_monitor *monitor)
{
	int	i;

	i = 0;
	monitor->dongle_pool = ft_calloc(sizeof(t_dongle), monitor->info->nb_coders);
	if (!monitor->dongle_pool)
		return (0);
	while (i < monitor->info->nb_coders)
	{
		monitor->dongle_pool[i].id = i + 1;
		monitor->dongle_pool[i].cooldown_time = monitor->info->cooldown_time;
		monitor->dongle_pool[i].last_used = 0;
		monitor->dongle_pool[i].is_used = false;
		monitor->dongle_pool[i].sch = monitor->info->scheduler;
		pthread_mutex_init(&monitor->dongle_pool[i].mutex, NULL);
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

void	free_and_destroy(t_monitor *monitor)
{
	int	i;

	i = -1;
	while (++i < monitor->info->nb_coders)
	{
		free(monitor->coder_pool[i].next_dongles);
		pthread_mutex_destroy(&monitor->dongle_pool[i].mutex);
	}
	free(monitor->dongle_pool->queue);
	free(monitor->coder_pool);
	free(monitor->dongle_pool);
	monitor->coder_pool = NULL;
	monitor->dongle_pool = NULL;
	pthread_mutex_destroy(&monitor->log_mutex);
	pthread_mutex_destroy(&monitor->run_mutex);
	pthread_cond_destroy(&monitor->run_cond);
}
