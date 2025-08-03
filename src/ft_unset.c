/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:20:02 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/01 16:34:01 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <stdlib.h>
#include <unistd.h>

static int	is_valid_identifier(const char *s)
{
	int	i;

	if (!s || !(s[0]) || (s[0] >= '0' && s[0] <= '9'))
		return (0);
	i = 0;
	while (s[i])
	{
		if (!(s[i] == '_' || (s[i] >= 'A' && s[i] <= 'Z')
				|| (s[i] >= 'a' && s[i] <= 'z')
				|| (s[i] >= '0' && s[i] <= '9')))
			return (0);
		i++;
	}
	return (1);
}

static void	remove_var(t_ms *ms, const char *name)
{
	int		i;
	int		k;
	int		len;
	char	**new_env;

	len = 0;
	while (ms->env && ms->env[len])
		len++;
	new_env = gc_malloc(ms, sizeof(char *) * len);
	if (!new_env)
		return ;
	i = 0;
	k = 0;
	while (i < len)
	{
		if (!(ft_strncmp(ms->env[i], name, ft_strlen(name)) == 0
				&& ms->env[i][ft_strlen(name)] == '='))
		{
			new_env[k++] = ms->env[i];
		}
		i++;
	}
	new_env[k] = NULL;
	ms->env = new_env;
}

int	ft_unset(char **args, t_ms *ms)
{
	int	i;

	i = 1;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			write(2, "minishell: unset: not a valid identifier\n", 42);
			return (1);
		}
		remove_var(ms, args[i]);
		remove_from_export_only(ms, args[i]);
		i++;
	}
	return (0);
}
