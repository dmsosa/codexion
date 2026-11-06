/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:15:26 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/04 21:54:40 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_coder_threads(t_monitor *monitor, int amount)
{
	int	i;

	i = -1;
	while (++i < amount)
		pthread_join(monitor->coder_pool[i].tid, NULL);
}

static void	*run_threads(t_monitor *monitor)
{
	int	i;

	i = 0;
	while (i < monitor->nb_coders)
	{
		if (pthread_create(&monitor->coder_pool[i].tid, NULL, coder_routine,
				&monitor->coder_pool[i]
			) != 0)
		{
			fprintf(stderr, "Error creating the coder thread number %d\n", i);
			break ;
		}
		i++;
	}
	if (i < monitor->nb_coders)
		return (free_and_destroy(monitor), NULL);
	if (pthread_create(&monitor->tid, NULL, monitor_routine, monitor) != 0)
		return (free_and_destroy(monitor), NULL);
	pthread_join(monitor->tid, NULL);
	wait_coder_threads(monitor, i);
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_monitor	monitor;

	if (argc != 9)
		return (print_usage(), 1);
	memset(&monitor, 0, sizeof(t_monitor));
	if (!parse_params(argc, argv, &monitor))
		return (1);
	if (!init_monitor(&monitor))
		return (1);
	run_threads(&monitor);
	free_and_destroy(&monitor);
	return (0);
}
