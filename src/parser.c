#include "../include/minishell.h"
#include "../libft/libft.h"

static int	is_redirect(const char *token)
{
	return (!ft_strcmp(token, "<")
		|| !ft_strcmp(token, ">")
		|| !ft_strcmp(token, ">>")
		|| !ft_strcmp(token, "<<"));
}

char	*bash_quote_trim(const char *token, t_ms *ms)
{
	char	*res;
	int		i;
	int		j;
	char	quote;

	(void)ms;
	res = gc_malloc(ms, ft_strlen(token) + 1);
	if (!res)
	{
		write(2, "minishell: bash_quote_trim allocation failed\n", 34);
		return (NULL);
	}
	i = 0;
	j = 0;
	while (token[i])
	{
		if (token[i] == '\'' || token[i] == '"')
		{
			quote = token[i++];
			while (token[i] && token[i] != quote)
				res[j++] = token[i++];
			if (token[i] == quote)
				i++;
		}
		else
			res[j++] = token[i++];
	}
	res[j] = '\0';
	return (res);
}

static int	parse_redirect(t_cmd *cmd, char **tokens, int *i, t_ms *ms)
{
	int		fd;
	char	*clean;

	if (!ft_strcmp(tokens[*i], "<"))
		cmd->infile = gc_strdup(ms, tokens[++(*i)]);
	else if (!ft_strcmp(tokens[*i], ">"))
	{
		cmd->outfile = gc_strdup(ms, tokens[++(*i)]);
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
			perror(cmd->outfile);
		else
			close(fd);
		cmd->append = 0;
	}
	else if (!ft_strcmp(tokens[*i], ">>"))
	{
		cmd->outfile = gc_strdup(ms, tokens[++(*i)]);
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd < 0)
			perror(cmd->outfile);
		else
			close(fd);
		cmd->append = 1;
	}
	else if (!ft_strcmp(tokens[*i], "<<"))
	{
		clean = bash_quote_trim(tokens[*i + 1], ms);
		if (!clean)
		{
			ms->last_exit = 1;
			return (1);
		}
		add_heredoc(cmd, clean, ms);
		ms->heredoc_index++;
		cmd->heredoc = 1;
		(*i) += 2;
		return (0);
	}
	(*i)++;
	return (0);
}

t_cmd	*parser(char **tokens, t_ms *ms)
{
	t_cmd	*cmds;
	t_cmd	*current;
	int		i;
	int		cmd_start;
	int		has_heredoc;

	cmds = NULL;
	i = 0;
	while (tokens[i])
	{
		current = init_cmd(ms);
		cmd_start = i;
		has_heredoc = 0;
		while (tokens[i] && ft_strcmp(tokens[i], "|"))
		{
			if (is_redirect(tokens[i]))
			{
				if (!ft_strcmp(tokens[i], "<<"))
					has_heredoc = 1;
				if (parse_redirect(current, tokens, &i, ms))
					return (NULL);
			}
			else
				i++;
		}
		current->args = copy_args(tokens, cmd_start, i, ms);
		if (has_heredoc || current->args)
			add_cmd(&cmds, current);
		/* else: GC will clean unreferenced cmd */
		if (tokens[i])
			i++;
	}
	return (cmds);
}
