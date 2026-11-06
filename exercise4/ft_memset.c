/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:05 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/27 19:42:58 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exercise04.h"

void    *ft_memset(void *b, int c, size_t len)
{
    unsigned char   *ptr;

    if (!b && len > 0)
        return (NULL);
    ptr = (unsigned char *)b;
    while (len--)
        ptr[len] = (unsigned char)c;
    return (b);
}
