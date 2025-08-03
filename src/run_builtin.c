/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:56:28 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/03 01:51:13 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"

int	run_builtin(t_cmd *cmd, t_ms *ms)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (!ft_strcmp(cmd->args[0], "echo"))
		return (ft_echo(cmd->args));
	else if (!ft_strcmp(cmd->args[0], "pwd"))
		return (ft_pwd(ms));
	else if (!ft_strcmp(cmd->args[0], "env"))
	{
		if (cmd->args[1] != NULL)
		{
			ft_putstr_fd("env: too many arguments\n", 2);
			return (127);
		}
		return (ft_env(ms->env));
	}
	else if (!ft_strcmp(cmd->args[0], "cd"))
		return (ft_cd(cmd->args, ms));
	else if (!ft_strcmp(cmd->args[0], "exit"))
		return (ft_exit(cmd->args, ms));
	else if (!ft_strcmp(cmd->args[0], "export"))
		return (ft_export(cmd->args, ms));
	else if (!ft_strcmp(cmd->args[0], "unset"))
		return (ft_unset(cmd->args, ms));
	return (1);
}
