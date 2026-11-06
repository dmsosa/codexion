/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:05 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/28 17:35:27 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
