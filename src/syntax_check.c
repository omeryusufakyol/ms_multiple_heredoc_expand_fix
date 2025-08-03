#include "../include/minishell.h"
#include "../libft/libft.h"

static int	is_operator(const char *token)
{
	return (!ft_strcmp(token, "|") ||
		!ft_strcmp(token, "<") ||
		!ft_strcmp(token, ">") ||
		!ft_strcmp(token, ">>") ||
		!ft_strcmp(token, "<<"));
}

int	check_syntax(char **tokens, t_ms *ms)
{
	int	i;

	if (!tokens || !tokens[0])
		return (0);

	// Başlangıç token’ı operator mü?
	if (is_operator(tokens[0]))
	{
		// Eğer << ile başlıyorsa ama bir delimiter varsa hata verme
		if (!ft_strcmp(tokens[0], "<<") && tokens[1] && !is_operator(tokens[1]))
			; // heredoc doğru durumda, izin ver
		else
		{
			if (!ft_strcmp(tokens[0], "|"))
				ft_putendl_fd("syntax error near unexpected token `|'", 2);
			else if (!ft_strcmp(tokens[0], "<<"))
				ft_putendl_fd("syntax error near unexpected token `<<'", 2);
			else
				ft_putendl_fd("syntax error near unexpected token", 2);
			ms->last_exit = 2;
			return (1);
		}
	}

	// Orta kısımdaki redirect kontrolleri
	i = 0;
	while (tokens[i])
	{
		if ((!ft_strcmp(tokens[i], "<") || !ft_strcmp(tokens[i], ">")
			|| !ft_strcmp(tokens[i], ">>") || !ft_strcmp(tokens[i], "<<")))
		{
			if (!tokens[i + 1] || is_operator(tokens[i + 1]))
			{
				ft_putendl_fd("syntax error near unexpected token `newline'", 2);
				ms->last_exit = 2;
				return (1);
			}
		}
		i++;
	}
	return (0);
}