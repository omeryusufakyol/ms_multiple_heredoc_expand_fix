/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:46:57 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/03 02:13:46 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

static int	wait_all(pid_t *pids, int count)
{
	int	status;
	int	last;
	int	i;

	last = 0;
	i = count;
	while (i--)
	{
		waitpid(pids[i], &status, 0);
		if (i == 0)
			last = status;
	}
	if (WIFEXITED(last))
		return (WEXITSTATUS(last));
	return (1);
}

static pid_t	launch_process(t_cmd *cmd, t_ms *ms, int in_fd, int out_fd)
{
	pid_t	pid;
	int		fd;
	int		exit_code;
	char	*path;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	fd = 3;
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		dup2(in_fd, STDIN_FILENO);
		dup2(out_fd, STDOUT_FILENO);
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		if (out_fd != STDOUT_FILENO)
			close(out_fd);
		while (fd < 1024)
		{
			if (fd != STDIN_FILENO
			 && fd != STDOUT_FILENO
			 && fd != STDERR_FILENO)
				close(fd);
			fd++;
		}
		redirect(cmd, ms);
		if (!cmd->args || !cmd->args[0])
		{
			write(2, "minishell: invalid command\n", 28);
			gc_free_all(ms);
			exit(1);
		}
		if (is_builtin(cmd->args[0]))
		{
			exit_code = run_builtin(cmd, ms);
			gc_free_all(ms);
			exit(exit_code);
		}
		path = find_path(ms, cmd->args[0], ms->env);
		if (!path)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->args[0], 2);
			ft_putstr_fd(": command not found\n", 2);
			gc_free_all(ms);
			exit(127);
		}
		execve(path, cmd->args, ms->env);
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": ", 2);
		perror("");
		gc_free_all(ms);
		if (errno == ENOENT)
			exit(127);
		else
			exit(126);
	}
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	return (pid);
}

static int	run_pipeline_loop(t_cmd **cmds, t_ms *ms, int *in_fd, pid_t *pids)
{
	int	p[2];
	int	i;

	i = 0;
	while (*cmds && (*cmds)->next)
	{
		if (i >= MAX_CMDS)
		{
			write(2, "minishell: too many piped commands\n", 35);
			return (-1);
		}
		if (pipe(p) == -1)
			return (perror("pipe"), -1);
		pids[i++] = launch_process(*cmds, ms, *in_fd, p[1]);
		close(p[1]);
		if (*in_fd != STDIN_FILENO)
			close(*in_fd);
		*in_fd = p[0];
		*cmds = (*cmds)->next;
	}
	return (i);
}

int	run_pipeline(t_cmd *cmds, t_ms *ms)
{
	int		in_fd;
	pid_t	pids[MAX_CMDS];
	int		i;

	in_fd = STDIN_FILENO;
	i = run_pipeline_loop(&cmds, ms, &in_fd, pids);
	if (i == -1)
		return (1);
	pids[i++] = launch_process(cmds, ms, in_fd, STDOUT_FILENO);
	if (in_fd != STDIN_FILENO)
		close(in_fd);
	ms->last_exit = wait_all(pids, i);
	return (ms->last_exit);
}
