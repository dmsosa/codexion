/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   binary_tree.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 20:39:17 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 16:55:24 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	tree_push(t_coder *c, t_dongle *d)
{
	t_dongle_req	req;

	if (d->queue_idx >= d->queue_max)
		return ;
	req.coder = c;
	req.seq = d->seq + 1;
	d->seq++;
	req.deadline = c->last_compile;
	d->queue[d->queue_idx] = req;
	d->queue_idx++;
	shift_up(d, d->mon->scheduler, d->queue_idx - 1);
}

/**
 * Sorts the queue using a simple insertion sort, and the criteria
 * of comparison depends on scheduler, with the sch_scheduler
 * function
 */
// void	sort_queue(t_dongle *d)
// {
// 	int	i;
// 	int	j;

// 	while (i = 0)
// 	{

// 	}
// }

t_coder	*tree_pop(t_dongle *d)
{
	t_dongle_req	req;

	if (!d->queue_idx)
		return (NULL);
	req = d->queue[0];
	if (d->queue_idx > 0)
		d->queue_idx--;
	d->queue[0] = d->queue[d->queue_idx];
	shift_down(d, d->mon->scheduler, 0);
	return (req.coder);
}

/*
Sorts the queue depending on FIFO (First In, First Out)
or EDF (End Deadline First) strategies
*/
void	shift_up(t_dongle *d, t_scheduler sch, int i)
{
	int				parent_idx;
	t_dongle_req	temp;

	while (i > 0)
	{
		parent_idx = (i - 1) / 2;
		if (sch_compare(sch, d->queue[i], d->queue[parent_idx]) > 0)
			break ;
		temp = d->queue[parent_idx];
		d->queue[parent_idx] = d->queue[i];
		d->queue[i] = temp;
		i = parent_idx;
	}
}

/**
 * when I pop the top-most element from the (binary tree),
 * its two branches are left behind, so I need to update
 * their parents as well, by convention
 * the left son is going to be the father of the right son.
 * and the left son of the next node needs to be my new right son
 * 
 * all the way until the last node.
 */
void	shift_down(t_dongle *d, t_scheduler sch, int i)
{
	t_dongle_req	temp;
	int				l_child;
	int				r_child;
	int				current;

	l_child = i * 2 + 1;
	r_child = i * 2 + 2;
	while (l_child < d->queue_idx)
	{
		current = l_child;
		if (r_child < d->queue_idx
			&& sch_compare(sch, d->queue[r_child], d->queue[l_child]) < 0)
			current = r_child;
		if (sch_compare(sch, d->queue[i], d->queue[current]) < 0)
			break ;
		temp = d->queue[i];
		d->queue[i] = d->queue[current];
		d->queue[current] = temp;
		i = current;
		l_child = i * 2 + 1;
		r_child = i * 2 + 2;
	}
}
