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

void	print_exit(const char *str)
{
	printf("%s", str);
	exit(1);
}

void	print_usage(void)
{
	printf("Usage: codexion [-s stack-size] arg...\n");
	exit(1);
}
