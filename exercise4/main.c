/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 17:07:57 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/28 14:17:48 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exercise04.h"

void	init_stack_attr(pthread_attr_t *attr, t_program_info *program_info)
{
	int	s;

	if (program_info->stack_size > 0)
	{
		s = pthread_attr_setstacksize(attr, program_info->stack_size);
		if (s != 0)
			print_exit("Error setting attr stack size");
	}
}

int	parse_options(int argc, char *argv[], t_program_info *program_info)
{
	int	idx;

	idx = 1;
	while (idx < argc)
	{
		if (ft_strncmp(argv[idx], "--", 2) == 0
			|| ft_strncmp(argv[idx], "-", 1) == 0)
		{
			if (ft_strncmp(argv[idx], "--s", 3)
				|| ft_strncmp(argv[idx], "--size", 7))
			{
				if (idx + 1 >= argc)
					print_exit("Option without value.\n");
				program_info->stack_size = ft_atoi(argv[idx + 1]);
				if (program_info->stack_size == 0)
				{
					printf(
						"Invalid value '%d'.\n \
						Usage: %s [-s stack-size] arg...\n",
						program_info->stack_size,
						argv[0]
						);
					exit(1);
				}
			}
			else
				print_usage();
			idx += 2;
		}
		else
			break ;
	}
	return (idx);
}

int	main(int argc, char *argv[])
{
	t_thr_info		*tinfo;
	t_program_info	program_info;
	pthread_attr_t	attr;
	void			*thr_res;
	int				num_threads;
	int				s;
	int				args_idx;
	int				i;
	int				main_var;

	if (argc < 2)
		print_usage();

	main_var = 0;
	program_info.stack_size = 0;
	args_idx = parse_options(argc, argv, &program_info);
	num_threads = argc - args_idx;

	s = pthread_attr_init(&attr);
	if (s != 0)
		print_exit("pthread_attr_init");

	init_stack_attr(&attr, &program_info);
	tinfo = ft_calloc(num_threads, sizeof(t_thr_info));
	if (!tinfo)
		print_exit("calloc");

	i = 0;
	while (i < num_threads)
	{
		s = pthread_create(
				&tinfo[i].thr_id,
				NULL,
				&thr_start_routine,
				&tinfo[i]
				);
		if (s != 0)
			print_exit("Error in pthread_create");
		tinfo[i].argv_str = argv[i + 1];
		tinfo[i].thr_num = i;
		tinfo[i].variable = &main_var;
		i++;
	}
	s = pthread_attr_destroy(&attr);
	if (s != 0)
		print_exit("pthread_attr_destroy");
	i = 0;
	while (i < num_threads)
	{
		s = pthread_join(tinfo[i].thr_id, &thr_res);
		if (s != 0)
			err(EXIT_FAILURE, "pthread_create");
		printf("Thread %d result is: %d\n", tinfo[i].thr_num, *(int *) thr_res);
		i++;
	}

	return (0);
}
