/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:05 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/27 11:00:16 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exercise03.h"

char	*ft_strcpy(char *dst, char *src)
{
	char	*tmp;

	tmp = dst;
	while (*src)
	{
		*dst = *src;
		dst++;
		src++;
	}
	*dst = 0;
	return (tmp);
}
