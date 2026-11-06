/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_or_exit.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/08 16:44:02 by jwins             #+#    #+#             */
/*   Updated: 2026/10/28 21:53:06 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	ft_atol_or_exit(const char *str)
{
	long	result;
	int		sign;
	int		i;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == 32 || (str[i] >= 10 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	if (str[i] == '\0')
		print_exit("Invalid number in atoi\n");
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			print_exit("Invalid number in atoi\n");
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result *= sign);
}
