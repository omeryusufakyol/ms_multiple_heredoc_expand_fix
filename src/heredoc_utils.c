/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:22:34 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/01 23:42:58 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../include/minishell.h"
#include <stdlib.h>

static void	append_env_or_text(t_ms *ms, char **res, char *line, int *i)
{
	char	*temp;
	char	*val;
	int		start;

	if (line[*i] == '$' && line[*i + 1] == '?')
	{
		*i += 2;
		val = gc_itoa(ms, ms->last_exit); // ➕ eğer eksikse gc_itoa'yı da ekleyelim
		*res = gc_strjoin_free(ms, *res, val);
	}
	else if (line[*i] == '$' && (ft_isalpha(line[*i + 1]) || line[*i + 1] == '_'))
	{
		(*i)++;
		start = *i;
		while (line[*i] && (ft_isalnum(line[*i]) || line[*i] == '_'))
			(*i)++;
		temp = gc_strndup(ms, &line[start], *i - start);
		val = get_env_value(ms, temp);
		*res = gc_strjoin_free(ms, *res, val);
	}
	else
	{
		start = *i;
		while (line[*i] && line[*i] != '$')
			(*i)++;
		temp = gc_strndup(ms, &line[start], *i - start);
		*res = gc_strjoin_free(ms, *res, temp);
	}
}

char	*expand_heredoc_line_envonly(char *line, t_ms *ms)
{
	char	*result;
	int		i;

	if (!line)
		return (NULL);
	result = gc_strdup(ms, "");
	if (!result)
		return (NULL);
	i = 0;
	while (line[i])
		append_env_or_text(ms, &result, line, &i);
	return (result);
}
