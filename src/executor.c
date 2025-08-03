/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 23:42:24 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/03 17:05:47 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

static void	handle_outfile(t_cmd *cmd, t_ms *ms)
{
	int	fd;

	if (cmd->append)
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror(cmd->outfile);
		gc_free_all(ms);
		exit(1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
}

void	redirect(t_cmd *cmd, t_ms *ms)
{
	int					fd;
	struct sigaction	old_int;
	struct sigaction	old_quit;
	struct sigaction	ign;

	if (cmd->heredoc)
	{
		ign.sa_handler = SIG_IGN;
		sigemptyset(&ign.sa_mask);
		ign.sa_flags = 0;
		sigaction(SIGINT, &ign, &old_int);
		sigaction(SIGQUIT, &ign, &old_quit);
		if (handle_heredoc(cmd, ms))
		{
			gc_free_all(ms);
			exit(ms->last_exit);
		}
		sigaction(SIGINT, &old_int, NULL);
		sigaction(SIGQUIT, &old_quit, NULL);
	}
	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		if (fd < 0)
		{
			perror(cmd->infile);
			gc_free_all(ms);
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (cmd->outfile)
		handle_outfile(cmd, ms);
}

static void	run_single(t_cmd *cmds, t_ms *ms)
{
	pid_t	pid;
	int		status;
	char	*path;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		redirect(cmds, ms);
		if (!cmds->args || !cmds->args[0] || cmds->args[0][0] == '\0')
			run_single_cleanup_exit(ms, 0);
		if (is_builtin(cmds->args[0]))
		{
			gc_free_all(ms);
			exit(run_builtin(cmds, ms));
		}

		path = find_path(ms, cmds->args[0], ms->env);
		if (!path)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmds->args[0], 2);
			ft_putstr_fd(": command not found\n", 2);
			run_single_cleanup_exit(ms, 127);
		}
		execve(path, cmds->args, ms->env);
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmds->args[0], 2);
		ft_putstr_fd(": ", 2);
		perror("");
		if (errno == ENOENT)
			run_single_cleanup_exit(ms, 127);
		else
			run_single_cleanup_exit(ms, 126);
	}
	waitpid(pid, &status, 0);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	if (WIFEXITED(status))
		ms->last_exit = WEXITSTATUS(status);
}

void	execute(t_cmd *cmds, t_ms *ms)
{
	if (!cmds)
		return ;
	if ((!cmds->args || !*cmds->args) && !cmds->heredoc)
		return ;
	if (cmds->next)
	{
		run_pipeline(cmds, ms);
		return ;
	}
	if (is_builtin(cmds->args[0])
		&& !cmds->infile && !cmds->outfile && !cmds->heredoc)
	{
		ms->last_exit = run_builtin(cmds, ms);
		return ;
	}
	run_single(cmds, ms);
}
