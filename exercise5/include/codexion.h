/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:58:02 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/02 12:21:17 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <pthread.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <string.h>


typedef struct s_monitor	t_monitor;
typedef struct s_coder		t_coder;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef enum e_coder_state
{
	TAKE,
	COMPILE,
	DEBUG,
	REFACTOR
}	t_coder_state;

typedef struct s_program_info
{
	int			nb_coders;
	int			burnout_time;
	int			compile_time;
	int			debug_time;
	int			refactor_time;
	int			req_compile;
	int			cooldown_time;
	t_scheduler	scheduler;
	int			bonus;
}	t_program_info;


typedef struct s_dongle
{
	//the owner is a coder
	pthread_mutex_t	mutex;
	int				id;
	int				cooldown_time;
	long			last_used;
	bool			is_used;
	t_coder			**queue;
	int				queue_idx;
	int				queue_max;
	t_scheduler		sch;
	// t_coder			owner; how to make the owner be pointed from dongle? 
}	t_dongle;

typedef struct s_coder
{
	pthread_t		tid;
	int				id;
	t_coder_state	state;
	bool			burned;
	int				compile_time;
	int				debug_time;
	int				refactor_time;
	int				burnout_time;
	long			last_compile;
	int				req_compile;
	int				compile_count;
	t_dongle		**next_dongles;
	t_monitor		*mon;
}	t_coder;

typedef struct s_monitor
{
	t_program_info	info;
	pthread_t		tid;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	run_mutex;
	pthread_cond_t	run_cond;
	bool			running;
	long			start_time;
	t_coder			*coder_pool;
	t_dongle		*dongle_pool;
}	t_monitor;


// ERRORS
void			print_exit(const char *str);
void			print_exit_usage(void);

// PARSER
void			parse_params(
						int argc,
						char **argv,
						t_program_info *program_info
					);

// CODER
void			*monitor_routine(void *arg);
bool			check_is_over(t_monitor *monitor);
void			*coder_routine(void *arg);
void			clean_up(t_monitor *monitor);

// UTILS
void			*ft_calloc(size_t count, size_t size);
long			now_ms(void);
void			wait_until_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, long ms);
long			ft_atol_or_exit(const char *str);
int				ft_min(int a, int b);
int				ft_max(int a, int b);

// INIT

void			free_and_destroy(t_monitor *monitor);
void			monitor_run(t_monitor *monitor);
int				init_monitor(t_monitor *monitor);
