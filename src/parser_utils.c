/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:46:07 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/01 16:41:35 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"

t_cmd	*init_cmd(t_ms *ms)
{
	t_cmd	*cmd;

	cmd = gc_malloc(ms, sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->heredoc = 0;
	cmd->next = NULL;
	cmd->heredoc_delims = NULL;
	return (cmd);
}

void	add_cmd(t_cmd **cmds, t_cmd *new_cmd)
{
	t_cmd	*tmp;

	if (!*cmds)
	{
		*cmds = new_cmd;
		return ;
	}
	tmp = *cmds;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_cmd;
}

int	count_args(char **tokens, int i)
{
	int	count;

	count = 0;
	while (i > 0 && tokens[i - 1] && ft_strcmp(tokens[i - 1], "|"))
	{
		if (!ft_strcmp(tokens[i - 1], "<")
			|| !ft_strcmp(tokens[i - 1], ">")
			|| !ft_strcmp(tokens[i - 1], ">>")
			|| !ft_strcmp(tokens[i - 1], "<<"))
			i -= 2;
		else
		{
			count++;
			i--;
		}
	}
	return (count);
}

static int	count_non_redir_args(char **tokens, int start, int end)
{
	int	count;
	int	i;

	count = 0;
	i = start;
	while (i < end)
	{
		if (!ft_strcmp(tokens[i], "<") || !ft_strcmp(tokens[i], ">")
			|| !ft_strcmp(tokens[i], ">>") || !ft_strcmp(tokens[i], "<<"))
			i += 2;
		else
		{
			count++;
			i++;
		}
	}
	return (count);
}

char	**copy_args(char **tokens, int start, int end, t_ms *ms)
{
	char	**args;
	int		i;
	int		j;
	int		count;

	count = count_non_redir_args(tokens, start, end);
	args = gc_malloc(ms, sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = start;
	j = 0;
	while (i < end)
	{
		if (!ft_strcmp(tokens[i], "<") || !ft_strcmp(tokens[i], ">")
			|| !ft_strcmp(tokens[i], ">>") || !ft_strcmp(tokens[i], "<<"))
			i += 2;
		else
			args[j++] = gc_strdup(ms, tokens[i++]);
	}
	args[j] = NULL;
	return (args);
}
