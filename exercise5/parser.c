/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 16:38:21 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/29 20:08:08 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	handle_options(
	int argc,
	char **argv,
	t_program_info *program_info,
	int *idx
)
{
	int	holds_bool;

	holds_bool = 0;
	if (strcmp(argv[*idx], "-b") == 0
		|| strcmp(argv[*idx], "--bonus") == 0)
	{
		program_info->bonus = 1;
		holds_bool = 1;
	}

	if (holds_bool)
		*idx = *idx + 1;
	else
	{
		if (*idx + 1 >= argc)
			print_exit("Option without value");
		*idx = *idx + 2;
	}
}

static bool	is_option(const char *str)
{
	return ((str[0] == '-' && str[1] == '-')
		|| (str[0] == '-'));
}

static int	parse_options(
	int argc,
	char **argv,
	t_program_info *program_info
)
{
	int	idx;

	idx = 1;
	while (idx < argc)
	{
		if (is_option(argv[idx]))
			handle_options(argc, argv, program_info, &idx);
		else
			break ;
	}
	return (idx);
}

static int	check_scheduler(char *str)
{
	if (strcmp("fifo", str) == 0)
		return (FIFO);
	if (strcmp("edf", str) == 0)
		return (EDF);
	else
		return (-1);
}

void	parse_params(
	int argc,
	char **argv,
	t_program_info *program_info
)
{
	int	idx;

	idx = parse_options(argc, argv, program_info);
	if (argc - idx < 8)
		print_exit_usage();
	program_info->nb_coders = ft_atol_or_exit(argv[idx + 0]);
	program_info->burnout_time = ft_atol_or_exit(argv[idx + 1]);
	program_info->compile_time = ft_atol_or_exit(argv[idx + 2]);
	program_info->debug_time = ft_atol_or_exit(argv[idx + 3]);
	program_info->refactor_time = ft_atol_or_exit(argv[idx + 4]);
	program_info->req_compile = ft_atol_or_exit(argv[idx + 5]);
	program_info->cooldown_time = ft_atol_or_exit(argv[idx + 6]);
	program_info->scheduler = check_scheduler(argv[idx + 7]);
	if (program_info->scheduler == -1)
		print_exit("Unrecognized value for scheduler");
}
