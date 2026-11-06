/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:21:25 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/04 20:48:41 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_scheduler(char *str, t_monitor *monitor)
{
	if (strcmp("fifo", str) == 0)
		return (monitor->scheduler = FIFO, 1);
	if (strcmp("edf", str) == 0)
		return (monitor->scheduler = EDF, 1);
	else
	{
		printf("Bad value for scheduler %s", str);
		return (0);
	}
}

static int	parse_num_params(
	char **argv,
	t_monitor *monitor
)
{
	int	res;

	res = ft_atoi_strict(argv[1], &monitor->nb_coders);
	if (!res)
		return (print_error("Number of coders must be a valid integer"), 0);
	res = ft_atoi_strict(argv[2], &monitor->burnout_time);
	if (!res)
		return (print_error("Burnout time must be a valid integer"), 0);
	res = ft_atoi_strict(argv[3], &monitor->compile_time);
	if (!res)
		return (print_error("Compile time must be a valid integer"), 0);
	res = ft_atoi_strict(argv[4], &monitor->debug_time);
	if (!res)
		return (print_error("Debug time must be be a valid integer"), 0);
	res = ft_atoi_strict(argv[5], &monitor->refactor_time);
	if (!res)
		return (print_error("Refactor time must be a valid integer"), 0);
	res = ft_atoi_strict(argv[6], &monitor->req_compile);
	if (!res)
		return (print_error("Compile amount must be a valid integer"), 0);
	res = ft_atoi_strict(argv[7], &monitor->cooldown_time);
	if (!res)
		return (print_error("Cooldwon time must be a valid integer"), 0);
	return (1);
}

static int	check_values(t_monitor *monitor)
{
	if (monitor->nb_coders < 1 || monitor->nb_coders > 100)
		return (print_error("Number of coders must be between 0 and 100"), 0);
	if (monitor->burnout_time < 1 || monitor->burnout_time > 1e6)
		return (print_error("Burnout time must be between 0 and 1000000"), 0);
	if (monitor->compile_time < 1 || monitor->compile_time > 1e3)
		return (print_error("Compile time must be between 1 and 1000"), 0);
	if (monitor->debug_time < 1 || monitor->debug_time > 1e6)
		return (print_error("Debug time must be between 0 and 1000000"), 0);
	if (monitor->refactor_time < 1 || monitor->refactor_time > 1e6)
		return (print_error("Refactor time must be between 0 and 1000000"), 0);
	if (monitor->req_compile < 1 || monitor->req_compile > 100)
		return (print_error("Compile amount must be between 0 and 100"), 0);
	if (monitor->cooldown_time < 0 || monitor->cooldown_time > 1e6)
		return (print_error("Cooldwon time must be between 0 and 1000"), 0);
	return (1);
}

int	parse_params(
	int argc,
	char **argv,
	t_monitor *monitor
)
{
	if (argc != 9)
		return (print_error("Error, bad amount of params"), 0);
	if (!parse_num_params(argv, monitor))
		return (0);
	if (!check_values(monitor))
		return (0);
	if (!parse_scheduler(argv[8], monitor))
		return (print_error("Invalid value for scheduler"), 0);
	return (1);
}
