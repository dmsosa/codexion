/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exercise03.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: durisosa <durisosa@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:58:02 by durisosa          #+#    #+#             */
/*   Updated: 2026/10/27 19:41:52 by durisosa         ###   ########.fr       */
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

/* Used as arg for thr_start_routine */
typedef struct s_thr_info
{
	pthread_t	thr_id; /* ID returned by pthread_create() */
	int			thr_num; /* THe index of this thread*/
	char		*argv_str; /* From command line args */
}	t_thr_info;

typedef struct s_program_info
{
	int	stack_size;
}	t_program_info;


char	*ft_strdup(char *src);
int	ft_strlen(char *str);
int	ft_strncmp(const char *s1, const char *s2, size_t n);
void *ft_calloc(size_t count, size_t size);
int	ft_atoi(const char *str);
char	*ft_strcpy(char *dst, char *src);
void    *ft_memset(void *b, int c, size_t len);
int	ft_toupper(int c);
void	print_exit(const char *str);
void	print_usage(void);

// THREADS
void	*thr_start_routine(void *arg);