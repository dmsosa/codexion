/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 22:16:21 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:43:02 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	handle_take(t_coder *self)
{
	take_dongles(self);
	if (self->dongle_count == 2)
		self->state = COMPILE;
	return (1);
}

int	handle_compile(t_coder *self)
{
	pthread_mutex_lock(&self->mon->run_mutex);
	self->last_compile = now_ms();
	print_log(self, "is compiling");
	self->compile_count++;
	usleep(1000 * self->mon->compile_time);
	self->state = DEBUG;
	pthread_mutex_unlock(&self->mon->run_mutex);
	release_dongles(self);
	return (1);
}

int	handle_debug(t_coder *self)
{
	print_log(self, "is debugging");
	usleep(1000 * self->mon->debug_time);
	self->state = REFACTOR;
	return (1);
}

int	handle_refactor(t_coder *self)
{
	print_log(self, "is refactoring");
	usleep(1000 * self->mon->refactor_time);
	self->state = TAKE;
	return (1);
}

int	handle_state(t_coder *self)
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
