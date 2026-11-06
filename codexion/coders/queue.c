/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 20:36:43 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:07:26 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Comment une coder peux savoir s'il est mort ?
 * Est ce que cheque coder poux prendre son propre etat, 
 * et avisser les autres que il a mort?
 * Non, parce donc le monitour devoir attendre a cheque philosoph, 
 * et el deut passer pour le 
 * prochaine pour povoir verifier touts les drones cheque fois.
 * Ma comment je peux proteger une autre coder de changer il etat 
 * de mort d'une autre thread?
 * La solution la plus facile et que el etat doit dependre 100% du monitor
 * Il est qui decide se une coder thread est mort, pas les propes coders threads
 * De ce maniere, on centralise la condition de simulation dans 
 * une unique endroit.
 * Donc il faut aussi verifier si la simulation et finire 
 * pendant une action, lorsque si la simulation est finite,
 * les philosophers vivres ne dovoint reste pas avec les 
 * fochettres dans la man, il douvonts il realise, aussi.
 */
void	sort_queue(t_dongle *d)
{
	t_dongle_req	temp;
	int				i;
	int				j;

	i = 1;
	while (i < d->queue_idx)
	{
		j = i - 1;
		temp = d->queue[i];
		while (j >= 0 && sch_compare(d->mon->scheduler, d->queue[j], temp) > 0)
		{
			d->queue[j + 1] = d->queue[j];
			j--;
		}
		d->queue[j + 1] = temp;
		i++;
	}
}

t_dongle_req	new_dongle_req(t_coder *c, int seq, long deadline)
{
	t_dongle_req	req;

	req.coder = c;
	req.seq = seq;
	req.deadline = deadline;
	return (req);
}

/**
 * Insert into queue already sorted with insertion sort.
 */
void	insert_queue(t_coder *c, t_dongle *d)
{
	t_dongle_req	req;
	int				i;

	if (d->queue_idx >= d->queue_max)
		return ;
	req = new_dongle_req(c, d->seq + 1, c->last_compile);
	d->seq++;
	if (d->queue_idx == 0)
	{
		d->queue[0] = req;
		d->queue_idx++;
		return ;
	}
	i = d->queue_idx;
	while (i > 0
		&& sch_compare(d->mon->scheduler, d->queue[i - 1], req) > 0)
	{
		d->queue[i] = d->queue[i - 1];
		i--;
	}
	d->queue[i] = req;
	d->queue_idx++;
}

/**
 * Cette fonctionne douvont etret appelez
 * toujours avec une mutex deja verroulle
 */
t_coder	*pop_queue(t_dongle *d)
{
	t_coder	*ret;
	int		i;

	if (d->queue_idx == 0)
		return (NULL);
	ret = d->queue[0].coder;
	i = 0;
	while (i < d->queue_idx - 1)
	{
		d->queue[i] = d->queue[i + 1];
		i++;
	}
	memset(&d->queue[d->queue_idx], 0, sizeof(t_dongle_req));
	d->queue_idx--;
	return (ret);
}
