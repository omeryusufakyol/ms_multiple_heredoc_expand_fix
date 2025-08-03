/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:06:41 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/01 16:44:45 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static char	*get_home(t_ms *ms)
{
	return (get_env_value(ms, "HOME"));
}

static char	*get_oldpwd(t_ms *ms)
{
	char	*oldpwd;

	oldpwd = get_env_value(ms, "OLDPWD");
	if (!*oldpwd)
	{
		write(2, "minishell: cd: OLDPWD not set\n", 30);
		return (NULL);
	}
	printf("%s\n", oldpwd);
	return (oldpwd);
}

static char	*get_target(char **args, t_ms *ms)
{
	if (!args[1])
		return (get_home(ms));
	if (!ft_strcmp(args[1], "--"))
	{
		if (!args[2])
			return (get_home(ms));
		else
			return (gc_strdup(ms, args[2]));
	}
	if (!ft_strcmp(args[1], "-"))
		return (get_oldpwd(ms));
	return (gc_strdup(ms, args[1]));
}

static void	update_pwd_env(t_ms *ms, char *oldpwd)
{
	char	*tmp;
	char	*cwd_raw;
	char	*cwd;

	/* Update OLDPWD */
	tmp = gc_strjoin(ms, "OLDPWD=", oldpwd);
	update_env(ms, tmp);

	/* Update PWD */
	cwd_raw = getcwd(NULL, 0);
	if (cwd_raw)
	{
		cwd = gc_strdup(ms, cwd_raw);
		free(cwd_raw);
		tmp = gc_strjoin(ms, "PWD=", cwd);
		update_env(ms, tmp);
	}
}

int	ft_cd(char **args, t_ms *ms)
{
	char	*path;
	char	*oldpwd_raw;
	char	*oldpwd;

	if (cd_check_args(args, ms))
		return (1);

	/* Capture current working dir into GC */
	oldpwd_raw = getcwd(NULL, 0);
	if (!oldpwd_raw)
		return (perror("getcwd"), 1);
	oldpwd = gc_strdup(ms, oldpwd_raw);
	free(oldpwd_raw);

	path = get_target(args, ms);
	if (!path)
		return (1);

	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}

	update_pwd_env(ms, oldpwd);
	return (0);
}
