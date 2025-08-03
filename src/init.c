/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:33:38 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/04 00:15:08 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"

void	init_minishell(t_ms *ms, char **envp)
{
	int		i;
	char	*cwd;

	if (!envp || !envp[0])
	{
		ms->env = gc_malloc(ms, sizeof(char *) * 5);
		if (!ms->env)
			return ;
		cwd = getcwd(NULL, 0); // malloc ile döner → elle free edilmeli
		if (!cwd)
			return ;
		ms->env[0] = gc_strjoin(ms,
				"PATH=", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
		ms->env[1] = gc_strjoin(ms, "PWD=", cwd);
		ms->env[2] = gc_strdup(ms, "SHLVL=1");
		ms->env[3] = gc_strdup(ms, "_=/usr/bin/env");
		ms->env[4] = NULL;
		free(cwd); // getcwd malloc → elle free
	}
	else
	{
		i = 0;
		while (envp[i])
			i++;
		ms->env = gc_malloc(ms, sizeof(char *) * (i + 1));
		if (!ms->env)
			return ;
		i = 0;
		while (envp[i])
		{
			ms->env[i] = gc_strdup(ms, envp[i]);
			i++;
		}
		ms->env[i] = NULL;
	}
	ms->last_exit = 0;
	ms->export_only = NULL;
	ms->heredoc_index = 0;
}
