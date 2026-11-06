/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 15:35:33 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 16:42:11 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * You are going to leave this while loop only once this coder
 * is the first one in the queue, ready to claim the ownership.
 */
void	wait_dongle(t_coder *c, t_dongle *d)
{
	while (d->owner != NULL
		|| d->queue[0].coder != c
		|| now_ms() < d->next_cooldown)
	{
		if (!monitor_is_running(c->mon))
			return ;
		if (d->owner != NULL || d->queue[0].coder != c)
			pthread_cond_wait(&d->cond, &d->mutex);
		else
			wait_until_ms(&d->cond, &d->mutex, d->next_cooldown);
	}
	return ;
}

/**
 * Takes ownership of THIS dongle, updating its owner
 * if it is not the first coder asking for THIS dongle,
 * is going to enter into a queue for tkaing it later.
 * the queue then is going to be sorted depending on the
 * scheduler strategy being used
 */
void	take_dongle(t_coder *c, t_dongle *d)
{
	pthread_mutex_lock(&d->mutex);
	tree_push(c, d);
	wait_dongle(c, d);
	if (d->owner == NULL && d->queue[0].coder == c)
	{
		d->owner = c;
		c->dongle_count++;
		print_log(c, "has taken a dongle");
		tree_pop(d);
	}
	pthread_mutex_unlock(&d->mutex);
}

/*
This strategy takes the dongle with least Id first.
*/
void	take_dongles(t_coder *coder)
{
	t_dongle	*d0;
	t_dongle	*d1;

	d0 = coder->next_dongles[0];
	d1 = coder->next_dongles[1];
	if (d0->id > d1->id)
	{
		d0 = coder->next_dongles[1];
		d1 = coder->next_dongles[0];
	}
	take_dongle(coder, d0);
	if (d0 == d1)
		return ;
	take_dongle(coder, d1);
}

void	release_dongle(t_coder *c, t_dongle *d)
{
	pthread_mutex_lock(&d->mutex);
	if (d->owner != c)
	{
		pthread_mutex_unlock(&d->mutex);
		return ;
	}
	d->owner = NULL;
	c->dongle_count--;
	d->next_cooldown = now_ms() + d->cooldown_time;
	pthread_mutex_unlock(&d->mutex);
	pthread_cond_broadcast(&d->cond);
}

void	release_dongles(t_coder *c)
{
	release_dongle(c, c->next_dongles[0]);
	release_dongle(c, c->next_dongles[1]);
}
