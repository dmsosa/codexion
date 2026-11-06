/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/13 10:09:54 by adiaz-lo          #+#    #+#             */
/*   Updated: 2026/10/27 10:55:48 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exercise03.h"

void	*thr_start_routine(void *arg)
{
	t_thr_info	*info;
	char		*res;
	char		*p;
	int			i;

	info = (t_thr_info *) arg;
	printf("Thread %d: top of stack near %p; argv_string=%s\n",
		info->thr_num, (void *)&p, info->argv_str);
	res = ft_strdup(info->argv_str);
	if (res == NULL)
		exit(EXIT_FAILURE);
	i = -1;
	while (info->argv_str[++i])
		res[i] = ft_toupper(info->argv_str[i]);
	return (res);
}
