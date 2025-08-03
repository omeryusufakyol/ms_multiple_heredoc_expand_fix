/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:29:53 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/04 00:51:15 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"
#include <stdlib.h>
#include <stdio.h>

static volatile sig_atomic_t heredoc_sigint = 0;

void	add_heredoc(t_cmd *cmd, char *delim, t_ms *ms)
{
	int		i;
	int		j;
	char	**new;

	i = 0;
	while (cmd->heredoc_delims && cmd->heredoc_delims[i])
		i++;
	new = gc_malloc(ms, sizeof(char *) * (i + 2));
	if (!new)
		return ;
	j = 0;
	while (j < i)
	{
		new[j] = cmd->heredoc_delims[j];
		j++;
	}
	new[i] = gc_strdup(ms, delim);
	new[i + 1] = NULL;
	cmd->heredoc_delims = new;
}

int	is_quoted_delim(t_ms *ms)
{
	char	*input;
	int		i;
	int		index;

	input = ms->raw_input;
	i = 0;
	index = 1;
	while (input[i])
	{
		if (input[i] == '<' && input[i + 1] == '<')
		{
			if (index == ms->heredoc_index)
			{
				i += 2;
				while (input[i] == ' ' || input[i] == '\t')
					i++;
				if (input[i] == '\'' || input[i] == '"')
					return (1);
				else
					return (0);
			}
			index++;
			i += 2;
		}
		else
			i++;
	}
	return (0);
}

static void	read_heredoc_loop(int write_fd, char *delim, t_ms *ms)
{
	char	*line;
	char	*expanded;
	int		quoted;

	quoted = is_quoted_delim(ms);
	while (!heredoc_sigint)
	{
		line = readline("heredoc> ");
		if (!line)
			break ;
		if (!ft_strcmp(line, delim))
		{
			free(line);
			break ;
		}
		if (quoted)
			expanded = gc_strdup(ms, line);
		else
			expanded = expand_heredoc_line_envonly(line, ms);
		write(write_fd, expanded, ft_strlen(expanded));
		write(write_fd, "\n", 1);
		free(line); // readline malloc ile geliyor, bu free gerekli
	}
}

static t_ms *ms_holder(t_ms *new_ms)
{
	static t_ms *saved = NULL;

	if (new_ms)          /* setter */
		saved = new_ms;
	return (saved);      /* getter */
}

static void	handle_heredoc_sigint(int sig)
{
	(void)sig;
	heredoc_sigint = 1;
	write(STDOUT_FILENO, "\n", 1);
	close(STDIN_FILENO);          /* readline'ı abort eder        */
}

int	handle_heredoc(t_cmd *cmd, t_ms *ms)
{
	int		fd[2];
	int		i;
	pid_t	pid;
	int		status;

	i = 0;
	while (cmd->heredoc_delims && cmd->heredoc_delims[i])
	{
		if (pipe(fd) == -1 || (pid = fork()) == -1)
			return (perror("heredoc error"), 1);
		if (pid == 0)
		{
			heredoc_sigint = 0;
			ms_holder(ms);                    /* 2) setter */
			signal(SIGINT, handle_heredoc_sigint);
			close(fd[0]);
			read_heredoc_loop(fd[1], cmd->heredoc_delims[i], ms);
			close(fd[1]);
			if (heredoc_sigint)
				ms->last_exit = 130;
			gc_free_all(ms_holder(NULL));     /* 2) getter */
			exit(ms->last_exit);
		}
		close(fd[1]);
		waitpid(pid, &status, 0);
		if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
			|| (WIFEXITED(status) && WEXITSTATUS(status) == 130))
		{
			close(fd[0]);
			ms->last_exit = 130;
			return (1);
		}
		if (!cmd->heredoc_delims[i + 1])
		{
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
		}
		else
			close(fd[0]);
		i++;
	}
	return (0);
}
