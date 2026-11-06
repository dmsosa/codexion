/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:57:06 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/27 10:59:38 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exercise03.h"

int	ft_strlen(char *str)
{
	int	size;

	size = 0;
	while (*str++)
		size++;
	return (size);
}
