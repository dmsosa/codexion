/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_math.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:05 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/30 17:39:33 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_min(int a, int b)
{
	if (a <= b)
		return (a);
	else
		return (b);
}

int	ft_max(int a, int b)
{
	if (a >= b)
		return (a);
	else
		return (b);
}
