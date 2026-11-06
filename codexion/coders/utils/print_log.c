/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_log.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/08 16:44:02 by jwins             #+#    #+#             */
/*   Updated: 2026/11/05 17:36:54 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_log(t_coder	*c, char *str)
{
	t_monitor	*monitor;

	monitor = c->mon;
	pthread_mutex_lock(&monitor->log_mutex);
	printf("%s%s%ld %d %s%s\n",
		BOLD,
		get_color(c->id % 7),
		now_ms() - monitor->start_time,
		c->id,
		str,
		RESET
		);
	pthread_mutex_unlock(&monitor->log_mutex);
}
