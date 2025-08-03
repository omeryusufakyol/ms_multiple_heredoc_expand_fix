/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:07:13 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/01 16:52:38 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

static int	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static long	ft_atol_safe(const char *str, int *overflow)
{
	long	res;
	int		sign;
	int		digit;

	res = 0;
	sign = 1;
	*overflow = 0;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str)
	{
		if (*str < '0' || *str > '9')
			break ;
		digit = *str - '0';
		if (res > (LONG_MAX - digit) / 10)
		{
			*overflow = 1;
			break ;
		}
		res = res * 10 + digit;
		str++;
	}
	return (res * sign);
}

int	ft_exit(char **args, t_ms *ms)
{
	long	code;
	char	*trimmed;
	int		overflow;

	write(1, "exit\n", 5);
	if (!args[1])
	{
		gc_free_all(ms);
		exit(ms->last_exit);
	}
	trimmed = gc_strtrim(ms, args[1], " \t\n\r\v\f");
	if (!is_numeric(trimmed))
	{
		write(2, "minishell: exit: numeric argument required\n", 43);
		gc_free_all(ms);
		exit(2);
	}
	if (args[2])
	{
		write(2, "minishell: exit: too many arguments\n", 36);
		return (1);
	}
	code = ft_atol_safe(trimmed, &overflow);
	if (overflow)
	{
		write(2, "minishell: exit: numeric argument required\n", 43);
		gc_free_all(ms);
		exit(2);
	}
	gc_free_all(ms);
	exit((unsigned char)code);
}
