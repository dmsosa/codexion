/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_both.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 15:24:16 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:10:41 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static	bool	should_wait_dongle(t_coder *c, t_dongle *d)
{
	bool	ret;

	ret = d->owner != NULL
		|| d->queue[0].coder != c
		|| now_ms() < d->next_cooldown;
	return (ret);
}

static void	wait_both(t_coder *c, t_dongle *d0, t_dongle *d1)
{
	if (d0 == d1)
		return (wait_dongle(c, d0));
	while (should_wait_dongle(c, d0) || should_wait_dongle(c, d1))
	{
		if (!monitor_is_running(c->mon))
			return ;
		if (now_ms() < d0->next_cooldown)
			wait_until_ms(&d0->cond, &d0->mutex, d0->next_cooldown);
		else
			pthread_cond_wait(&d0->cond, &d0->mutex);
		if (now_ms() < d1->next_cooldown)
			wait_until_ms(&d1->cond, &d1->mutex, d1->next_cooldown);
		else
			pthread_cond_wait(&d1->cond, &d1->mutex);
	}
}

static void	take_both(t_coder *c, t_dongle *d0, t_dongle *d1)
{
	pthread_mutex_lock(&d0->mutex);
	pthread_mutex_lock(&d1->mutex);
	insert_queue(c, d0);
	insert_queue(c, d1);
	wait_both(c, d0, d1);
	if (d0->owner == NULL && d0->queue[0].coder == c
		&& d1->owner == NULL && d1->queue[0].coder == c)
	{
		d0->owner = c;
		d1->owner = c;
		c->dongle_count = c->dongle_count + 2;
		print_log(c, "has taken a dongle");
		print_log(c, "has taken a dongle");
		pop_queue(d0);
		pop_queue(d1);
	}
	pthread_mutex_unlock(&d0->mutex);
	pthread_mutex_unlock(&d1->mutex);
}

void	take_dongles_both(t_coder *c)
{
	t_dongle	*d0;
	t_dongle	*d1;

	d0 = c->next_dongles[0];
	d1 = c->next_dongles[1];
	if (d0->id > d1->id)
	{
		d0 = c->next_dongles[1];
		d1 = c->next_dongles[0];
	}
	if (d0 == d1)
		return (take_dongle(c, d0));
	return (take_both(c, d0, d1));
}
