/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:06:51 by oakyol            #+#    #+#             */
/*   Updated: 2025/07/03 00:06:52 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <unistd.h>

int	ft_echo(char **args)
{
	int	i;
	int	n1;

	i = 1;
	n1 = 1;
	if (args[1] && !ft_strcmp(args[1], "-n"))
	{
		n1 = 0;
		i++;
	}
	while (args[i])
	{
		write(1, args[i], ft_strlen(args[i]));
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (n1)
		write(1, "\n", 1);
	return (0);
}
