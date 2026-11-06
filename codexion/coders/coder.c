/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 13:03:35 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 13:17:07 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	handle_state(t_coder *self)
{
	if (!monitor_is_running(self->mon))
		return (0);
	if (self->state == TAKE)
		return (handle_take(self));
	else if (self->state == COMPILE)
		return (handle_compile(self));
	else if (self->state == DEBUG)
		return (handle_debug(self));
	else if (self->state == REFACTOR)
		return (handle_refactor(self));
	return (1);
}

/**
 * ce routibe est execute depuis la creation du fils d'execution
 * donc, il faut attendre jusqu'a il monitor indique que touts
 * les coders sont prets pour execute son tasche
 * 
 * Il coder doit premier essaye de prendre une dongle, 
 * il est il owner du dongle seulement se il est le premier
 * coder dans la queue du dongle
 * S'il n'est pas le premier, il join la queue pour attendre ce
 * dongle, une fois le coder que l'utilise a fini sa compilation,
 * il est realise est le owner ist NULL, le queue est actualise
 * e le thread est dans une bucle while pour verifier toujours
 * se une dongle est libere, le thread est revele pour le cond du
 * mutex
 */
void	*coder_routine(void *arg)
{
	t_coder	*self;

	self = (t_coder *) arg;
	pthread_mutex_lock(&self->mon->run_mutex);
	while (!self->mon->running)
		pthread_cond_wait(&self->mon->run_cond, &self->mon->run_mutex);
	pthread_mutex_unlock(&self->mon->run_mutex);
	while (monitor_is_running(self->mon)
		&& self->compile_count < self->mon->req_compile)
	{
		if (!handle_state(self))
			release_dongles(self);
	}
	return (NULL);
}
