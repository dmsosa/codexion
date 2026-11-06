/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/13 10:09:54 by adiaz-lo          #+#    #+#             */
/*   Updated: 2026/10/27 10:55:48 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_all_done(t_monitor *monitor)
{
	int		i;
	t_coder coder;

	i = 0;
	while (i < monitor->info.nb_coders)
	{
		coder = monitor->coder_pool[i];
		if (coder.compile_count < coder.req_compile)
			return (false);
		i++;
	}
	return (true);
}

static bool	check_burnout(t_monitor *monitor)
{
	int		i;
	long	now;
	long	time_from;
	t_coder	*coder;

	i = 0;
	now = now_ms();
	while (i < monitor->info.nb_coders)
	{
		coder = &monitor->coder_pool[i];
		time_from = coder->last_compile + monitor->start_time;
		if (now_ms() >=  time_from + coder->burnout_time)
		{
			coder->burned = true;
			pthread_mutex_lock(&monitor->log_mutex);
			printf(
				"%ld %d burned out\n", now_ms() - monitor->start_time,
				coder->id);
			pthread_mutex_unlock(&monitor->log_mutex);
			return (true);
		}
	}
	return (false);
}

// Mon simulation doit tout simplement initialiser les threads et changer la condition
// de execution su vrai, car les autres threads pouonts se executer.
void	*monitor_routine(void *arg)
{
	t_monitor		*self;
	int				i;

	self = (t_monitor *) arg;
	printf("Hello my name is: monitor %lu\n", (unsigned long) pthread_self());
	i = 0;
	while (i < self->info.nb_coders)
	{
		if (pthread_create(&self->coder_pool[i].tid, NULL, &coder_routine, &self->coder_pool[i]) != 0)
			print_exit("Error in pthread_create coder");
		i++;
	}
	pthread_mutex_lock(&self->run_mutex);
	self->running = true;
	pthread_cond_broadcast(&self->run_cond);
	pthread_mutex_unlock(&self->run_mutex);
	self->start_time = now_ms();
	i = -1;
	while (self->running)
	{
		if (check_burnout(self) || check_all_done(self))
		{
			pthread_mutex_lock(&self->run_mutex);
			self->running = false;
			pthread_cond_broadcast(&self->run_cond);
			pthread_mutex_unlock(&self->run_mutex);
		}
		usleep(1000);
	}
	while (++i < self->info.nb_coders)
		pthread_join(self->coder_pool[i].tid, NULL);
	return (NULL);
}

void	*coder_routine(void *arg)
{
	t_coder			*self;

	self = (t_coder *) arg;
	pthread_mutex_lock(&self->mon->run_mutex);
	while (self->mon->running && self->compile_count < self->req_compile)
	{
		if (self->state == TAKE)
		{
			take_dongles(self);
			self->state = COMPILE;
		}
		if (self->state == COMPILE)
		{
			lock_and_print(self->mon, self, "is compiling");
			self->state = DEBUG;
			usleep(self->compile_time * 1000);
		}
		if (self->state == DEBUG)
		{
			lock_and_print(self->mon, self, "is compiling");
			self->state = REFACTOR;
			usleep(self->compile_time * 1000);
		}
		if (self->state == REFACTOR)
		{
			lock_and_print(self->mon, self, "is compiling");
			self->state = TAKE;
			self->compile_count++;
			self->last_compile = now_ms();
			usleep(self->compile_time * 1000);
		}
		pthread_cond_wait(&self->mon->run_cond, &self->mon->run_mutex);
	}
	pthread_mutex_unlock(&self->mon->run_mutex);
	return (NULL);
}

void	lock_and_print(t_monitor *monitor, t_coder *coder, char *str)
{
	pthread_mutex_lock(&monitor->log_mutex);
	printf("%lu %d %s\n", now_ms() - monitor->start_time, coder->id, str);
	pthread_mutex_unlock(&monitor->log_mutex);
}

void	take_dongles(t_coder *coder)
{
	t_dongle	*d0;
	t_dongle	*d1;
	int			min_id;

	min_id_dongle = get_dongle_min_id(coder->mon);
	d0 = get_dongle_by_id(coder->mon, min_id);
	d1 = get_dongle_by_id(coder->mon, min_id + 1);
	pthread_mutex_lock(&d0->mutex);
	first_coder = queue_pop(d0);
	if (!first_coder || first_coder->id != coder->id)
	{
		queue_push(d0, coder);

	}
	else
	{
		d0->is_used = true;
	} 
	pthread_mutex_unlock(&dongle->mutex);
}

void	queue_push(t_dongle *dongle, t_coder *coder)
{
	if (dongle->queue_idx >= dongle->queue_max)
		return ;
	dongle->queue[dongle->queue_idx] = coder;
	dongle->queue_idx++;
}
void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->last_used = now_ms();
	dongle->is_used = false;
	pthread_mutex_unlock(&dongle->mutex);
}

t_dongle	*get_dongle_by_id(t_monitor *monitor, int id)
{
	int	i;

	while (i < monitor->info.nb_coders)
	{
		if (monitor->dongle_pool[i].id == id)
			return (&monitor->dongle_pool[i]);
		i++;
	}
	return (NULL);
}