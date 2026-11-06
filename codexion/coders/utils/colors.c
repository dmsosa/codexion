/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 17:05:04 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:35:35 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

char	*get_color(int index)
{
	if (index == 0)
		return (RED);
	else if (index == 1)
		return (GREEN);
	else if (index == 2)
		return (YELLOW);
	else if (index == 3)
		return (BLUE);
	else if (index == 4)
		return (MAGENTA);
	else if (index == 5)
		return (CYAN);
	else if (index == 6)
		return (WHITE);
	else
		return (WHITE);
}