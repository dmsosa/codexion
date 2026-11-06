/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 15:06:23 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 15:43:49 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	compare_edf(t_dongle_req a, t_dongle_req b)
{
	if (a.deadline < b.deadline)
		return (-1);
	if (a.deadline > b.deadline)
		return (1);
	if (a.seq < b.seq)
		return (-1);
	if (a.seq > b.seq)
		return (1);
	return (0);
}

static int	compare_fifo(t_dongle_req a, t_dongle_req b)
{
	if (a.seq < b.seq)
		return (-1);
	if (a.seq > b.seq)
		return (1);
	return (0);
}

/**
 * compares two coders in the queue depending on the 
 * scheduler strategy selected: FIFO or EDF
 * if FIFO, it just compares by seq (moment of arrival)
 */
int	sch_compare(t_scheduler sch, t_dongle_req a, t_dongle_req b)
{
	if (sch == FIFO)
		return (compare_fifo(a, b));
	else
		return (compare_edf(a, b));
}
