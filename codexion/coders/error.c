/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:25:10 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/04 12:39:22 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_error(const char *str)
{
	fprintf(stderr, "Error message: %s\n", str);
}

void	print_usage(void)
{
	printf("Usage: codexion [-b bonus] arg...\n");
}
