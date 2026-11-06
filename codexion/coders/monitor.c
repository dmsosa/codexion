/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 13:03:22 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 16:54:54 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_all_done(t_monitor *monitor)
{
	int		i;

	i = 0;
	pthread_mutex_lock(&monitor->run_mutex);
	while (i < monitor->nb_coders)
	{
		if (monitor->coder_pool[i].compile_count < monitor->req_compile)
			return (pthread_mutex_unlock(&monitor->run_mutex), false);
		i++;
	}
	pthread_mutex_unlock(&monitor->run_mutex);
	return (true);
}

static bool	check_burnout(t_monitor *monitor)
{
	int		i;

	i = 0;
	while (i < monitor->nb_coders)
	{
		pthread_mutex_lock(&monitor->run_mutex);
		if (now_ms()
			>= monitor->coder_pool[i].last_compile + monitor->burnout_time)
		{
			print_log(&monitor->coder_pool[i], "burned out");
			pthread_mutex_unlock(&monitor->run_mutex);
			return (true);
		}
		pthread_mutex_unlock(&monitor->run_mutex);
		i++;
	}
	return (false);
}

static void	update_start_time(t_monitor *self)
{
	int	i;

	self->start_time = now_ms();
	i = -1;
	while (++i < self->nb_coders)
		self->coder_pool[i].last_compile = self->start_time;
}

static void	wake_all(t_monitor *self)
{
	int	i;

	i = -1;
	while (++i < self->nb_coders)
	{
		pthread_mutex_lock(&self->dongle_pool[i].mutex);
		pthread_cond_broadcast(&self->dongle_pool[i].cond);
		pthread_mutex_unlock(&self->dongle_pool[i].mutex);
	}
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;

	self = (t_monitor *) arg;
	pthread_mutex_lock(&self->run_mutex);
	self->running = true;
	update_start_time(self);
	pthread_cond_broadcast(&self->run_cond);
	pthread_mutex_unlock(&self->run_mutex);
	while (monitor_is_running(self))
	{
		if (check_burnout(self) || check_all_done(self))
		{
			pthread_mutex_lock(&self->run_mutex);
			self->running = false;
			pthread_mutex_unlock(&self->run_mutex);
			wake_all(self);
		}
		usleep(1000);
	}
	return (NULL);
}
