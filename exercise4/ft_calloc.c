/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:05 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/27 19:42:57 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exercise04.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*new;

	if (size != 0 && count > SIZE_MAX / size)
		return (NULL);
	new = malloc(size * count);
	if (!new)
		return (NULL);
	ft_memset(new, 0, count * size);
	return (new);
}
