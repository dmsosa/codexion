/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:03:27 by durisosa          #+#    #+#             */
/*   Updated: 2026/11/05 17:42:24 by durisosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stddef.h>
# include <unistd.h>
# include <stdio.h>
# include <stdint.h>
# include <limits.h>
# include <pthread.h>
# include <err.h>
# include <errno.h>
# include <stdlib.h>
# include <stdbool.h>
# include <sys/time.h>
# include <string.h>

typedef struct s_monitor	t_monitor;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_monitor
{
	pthread_t		tid;
	int				nb_coders;
	int				burnout_time;
	int				compile_time;
	int				debug_time;
	int				refactor_time;
	int				req_compile;
	int				cooldown_time;
	int				bonus;
	long			start_time;
	bool			running;
	t_scheduler		scheduler;
	t_coder			*coder_pool;
	t_dongle		*dongle_pool;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	run_mutex;
	pthread_cond_t	run_cond;
}	t_monitor;

typedef struct s_dongle_req
{
	t_coder	*coder;
	long	seq;
	long	deadline;
}	t_dongle_req;

typedef struct s_dongle
{
	t_monitor		*mon;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				id;
	int				cooldown_time;
	long			next_cooldown;
	t_coder			*owner;
	t_dongle_req	*queue;
	int				queue_idx;
	int				queue_max;
	int				seq;
}	t_dongle;

typedef enum e_coder_state
{
	TAKE,
	COMPILE,
	DEBUG,
	REFACTOR
}	t_coder_state;

typedef struct s_coder
{
	t_monitor		*mon;
	pthread_t		tid;
	int				id;
	long			last_compile;
	int				compile_count;
	t_coder_state	state;
	t_dongle		**next_dongles;
	int				dongle_count;
}	t_coder;

// CORE
int		init_monitor(t_monitor *monitor);
void	*monitor_routine(void *arg);
int		monitor_is_running(t_monitor *monitor);

// CODER
void	*coder_routine(void *arg);
void	take_dongles(t_coder *coder);
void	take_dongle(t_coder *c, t_dongle *d);
void	wait_dongle(t_coder *c, t_dongle *d);
void	release_dongle(t_coder *c, t_dongle *d);
void	release_dongles(t_coder *c);
int		handle_take(t_coder *self);
int		handle_compile(t_coder *self);
int		handle_debug(t_coder *self);
int		handle_refactor(t_coder *self);

// CODER ALTERNATIVE STRATEGY
void	take_dongles_both(t_coder *c);
void	release_dongles(t_coder *c);

// SCHEDULER
int		sch_compare(t_scheduler sch, t_dongle_req a, t_dongle_req b);

// PARSER
int		parse_params(
			int argc,
			char **argv,
			t_monitor *monitor
			);
int		ft_atoi_strict(const char *str, int *value);

// FREE
void	free_and_destroy(t_monitor *monitor);

// UTILS
void	print_log(t_coder	*c, char *str);
void	*ft_calloc(size_t count, size_t size);
long	ft_atol_or_exit(const char *str);

// TIME
long	now_ms(void);
void	wait_until_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, long ms);
void	ft_sleep(long until_ms);

// QUEUE OR TREE (DONGLE QUEUE)

// BINARY TREE
void	tree_push(t_coder *c, t_dongle *d);
t_coder	*tree_pop(t_dongle *d);
void	shift_up(t_dongle *d, t_scheduler sch, int i);
void	shift_down(t_dongle *d, t_scheduler sch, int i);

// QUEUE
void	insert_queue(t_coder *c, t_dongle *d);
t_coder	*pop_queue(t_dongle *d);

// ERRORS
void	print_error(const char *str);
void	print_usage(void);

// COLORS
char	*get_color(int index);

# define RESET   "\x1b[0m"
# define BOLD    "\x1b[1m"
# define RED     "\x1b[31m"
# define GREEN   "\x1b[32m"
# define YELLOW  "\x1b[33m"
# define BLUE    "\x1b[34m"
# define MAGENTA "\x1b[35m"
# define CYAN    "\x1b[36m"
# define WHITE   "\x1b[37m"

#endif // CODEXION_H
