/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 13:39:02 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/30 13:39:22 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

static void	ms_to_timespec(long ms, struct timespec *ts)
{
	ts->tv_sec = ms / 1000L;
	ts->tv_nsec = (ms % 1000L) * 1000000L;
}

void	wait_until_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, long ms)
{
	struct timespec	ts;

	ms_to_timespec(ms, &ts);
	pthread_cond_timedwait(cond, mutex, &ts);
}