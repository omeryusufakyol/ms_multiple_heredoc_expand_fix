#include "../include/minishell.h"
#include "../libft/libft.h"

int	contains_heredoc(char **tokens)
{
	int	i;

	i = 0;
	while (tokens[i])
	{
		if (!ft_strcmp(tokens[i], "<<"))
			return (1);
		i++;
	}
	return (0);
}

static void	process_multiline_input(const char *input, t_ms *ms)
{
	char	**lines;
	char	**tokens;
	char	**expanded;
	t_cmd	*cmds;
	int		i;

	lines = ft_split(input, '\n');
	if (!lines)
		return ;
	i = 0;
	while (lines[i])
	{
		if (lines[i][0] != '\0')
		{
			tokens = lexer(lines[i], ms);
			if (tokens && !check_syntax(tokens, ms))
			{
				if (contains_heredoc(tokens))
				{
					cmds = parser(tokens, ms);
				}
				else
				{
					expanded = expand_tokens(tokens, ms);
					if (expanded && !check_syntax(expanded, ms))
						cmds = parser(expanded, ms);
					else
						cmds = NULL;
				}
				if (cmds)
					execute(cmds, ms);
			}
		}
		i++;
	}
}

void	mini_loop(t_ms *ms)
{
	char	*raw;
	char	*input;
	char	**tokens;
	char	**expanded;
	t_cmd	*cmds;

	while (1)
	{
		ms->heredoc_index = 0;
		raw = readline("minishell$ ");
		if (!raw)
		{
			write(1, "\nexit\n", 6);
			break ;
		}
		if (raw[0] != '\0')
			add_history(raw);

		/* GC ile yönetilecek kopya */
		input = gc_strdup(ms, raw);
		free(raw);
		ms->raw_input = input;

		if (ft_strchr(input, '\n'))
		{
			process_multiline_input(input, ms);
			continue ;
		}

		tokens = lexer(input, ms);
		if (!tokens || check_syntax(tokens, ms))
			continue ;
		if (contains_heredoc(tokens))
		{
			cmds = parser(tokens, ms);
		}
		else
		{
			expanded = expand_tokens(tokens, ms);
			if (!expanded || check_syntax(expanded, ms))
				continue ;
			cmds = parser(expanded, ms);
		}
		execute(cmds, ms);
		//gc_free_all(ms); //buna bak her while döngüsünde freelemek gerekiyor
	}
	(void)ms;
}
