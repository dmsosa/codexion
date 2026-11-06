/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exercise_deux.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 17:07:57 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/26 18:03:22 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>

static void *count_this_stuff(void *arg)
{
	int	i;

	(void) arg;
	i = -1;
	while (++i < 50)
		printf("Hallo von Kindthread: %d\n", i);
	exit(1);
	return (NULL);
}

int	main(void)
{
	pthread_t	thr;
	pthread_t	thr2;
	// int 		door = 0;
	int			s;


	// creer un fils de execution 
	s = pthread_create(&thr, NULL, count_this_stuff, NULL);
	if (s != 0)
		err(EXIT_FAILURE, "pthread_create");
	s = pthread_create(&thr2, NULL, count_this_stuff, NULL);
	if (s != 0)
		err(EXIT_FAILURE, "pthread_create");
	pthread_join(thr, NULL);
	pthread_join(thr, NULL);
	return (0);
}
