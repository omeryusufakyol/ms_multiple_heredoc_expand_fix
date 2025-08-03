#include "../include/minishell.h"
#include "../libft/libft.h"
#include <stdlib.h>

char	*get_env_value(t_ms *ms, const char *name)
{
	int		i;
	size_t	len;

	if (!name || name[0] == '\0')
		return (gc_strdup(ms, "$"));
	i = 0;
	len = ft_strlen(name);
	while (ms->env[i])
	{
		if (!ft_strncmp(ms->env[i], name, len) && ms->env[i][len] == '=')
			return (gc_strdup(ms, &ms->env[i][len + 1]));
		i++;
	}
	return (gc_strdup(ms, ""));
}

static char	*ft_strjoin_free(t_ms *ms, char *s1, char *s2)
{
	char *joined = gc_strjoin(ms, s1, s2);
	return (joined);
}

static int	check_tilde(const char *token, t_ms *ms, char **result)
{
	char	*home;
	char	*rest;

	if (!token || token[0] != '~')
		return (0);
	if (token[1] && token[1] != '/')
		return (0);
	home = get_env_value(ms, "HOME");
	if (!home)
		return (0);
	if (token[1] == '\0')
		*result = home;
	else
	{
		rest = gc_strdup(ms, token + 1);
		*result = ft_strjoin_free(ms, home, rest);
	}
	return (1);
}

static char	*expand_token(const char *token, t_ms *ms)
{
	char	*result;
	char	*temp;
	char	*val;
	size_t	i;
	size_t	start;

	if (!token)
		return (NULL);
	if (check_tilde(token, ms, &result))
		return (result);
	result = gc_strdup(ms, "");
	i = 0;
	while (token[i])
	{
		if (token[i] == '\'')
		{
			start = ++i;
			while (token[i] && token[i] != '\'')
				i++;
			temp = gc_strndup(ms, token + start, i - start);
			result = ft_strjoin_free(ms, result, temp);
			if (token[i])
				i++;
		}
		else if (token[i] == '"')
		{
			start = ++i;
			while (token[i] && token[i] != '"')
			{
				if (token[i] == '$' && token[i + 1] == '?')
				{
					temp = gc_itoa(ms, ms->last_exit);
					result = ft_strjoin_free(ms, result, temp);
					i += 2;
				}
				else if (token[i] == '$' && (ft_isalnum(token[i + 1]) || token[i + 1] == '_'))
				{
					i++;
					start = i;
					while (token[i] && (ft_isalnum(token[i]) || token[i] == '_'))
						i++;
					temp = gc_strndup(ms, token + start, i - start);
					val = get_env_value(ms, temp);
					result = ft_strjoin_free(ms, result, val);
				}
				else
				{
					result = ft_strjoin_free(ms, result, gc_strndup(ms, token + i++, 1));
				}
			}
			if (token[i] == '"')
				i++;
		}
		else if (token[i] == '$')
		{
			if (token[i + 1] == '?')
			{
				temp = gc_itoa(ms, ms->last_exit);
				result = ft_strjoin_free(ms, result, temp);
				i += 2;
			}
			else if (token[i + 1] && (ft_isalnum(token[i + 1]) || token[i + 1] == '_'))
			{
				i++;
				start = i;
				while (token[i] && (ft_isalnum(token[i]) || token[i] == '_'))
					i++;
				temp = gc_strndup(ms, token + start, i - start);
				val = get_env_value(ms, temp);
				result = ft_strjoin_free(ms, result, val);
			}
			else
			{
				result = ft_strjoin_free(ms, result, gc_strdup(ms, "$"));
				i++;
			}
		}
		else
		{
			start = i;
			while (token[i] && token[i] != '$' && token[i] != '\'' && token[i] != '"')
				i++;
			temp = gc_strndup(ms, token + start, i - start);
			result = ft_strjoin_free(ms, result, temp);
		}
	}
	return (result);
}

static int	append_split_if_needed(char *token, t_ms *ms, char **res)
{
	char	*expanded;
	char	**split;
	int		k;

	expanded = expand_token(token, ms);
	if (expanded[0] == '\0' && token[0] != '"' && token[0] != '\'')
		return (0);
	if ((ft_strchr(expanded, ' ') || ft_strchr(expanded, '\t')) &&
		token[0] != '"' && token[0] != '\'')
	{
		split = gc_split(ms, expanded, ' ');
		if (!split)
			return (0);
		k = 0;
		while (split[k])
		{
			res[k] = split[k];
			k++;
		}
		return (k);
	}
	res[0] = expanded;
	return (1);
}

static int	append_as_is(char *token, t_ms *ms, char **res)
{
	char	*expanded = expand_token(token, ms);
	if (expanded[0] == '\0' && token[0] != '"' && token[0] != '\'')
		return (0);
	res[0] = expanded;
	return (1);
}

static char	**resize_result(t_ms *ms, char **old, int old_cap, int new_cap)
{
	char	**new;
	int		i;

	new = gc_malloc(ms, sizeof(char *) * new_cap);
	if (!new)
		return (NULL);
	i = 0;
	while (i < old_cap)
	{
		new[i] = old[i];
		i++;
	}
	return (new);
}

static int	count_split_pieces(char *token, t_ms *ms)
{
	char	*expanded;
	char	**split;
	int		count;

	expanded = expand_token(token, ms);
	if (!expanded)
		return (0);
	if (expanded[0] == '\0' && token[0] != '"' && token[0] != '\'')
		return (0);
	if ((ft_strchr(expanded, ' ') || ft_strchr(expanded, '\t')) &&
		token[0] != '"' && token[0] != '\'')
	{
		split = gc_split(ms, expanded, ' ');
		if (!split)
			return (0);
		count = 0;
		while (split[count])
			count++;
		return (count);
	}
	return (1);
}

static int	needs_split(int index)
{
	return (index == 0);
}

char	**expand_tokens(char **tokens, t_ms *ms)
{
	char	**result;
	int		i;
	int		j;
	int		capacity;
	int		split;

	capacity = 8;
	result = gc_malloc(ms, sizeof(char *) * capacity);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (tokens[i])
	{
		split = needs_split(i);
		while (split && j + count_split_pieces(tokens[i], ms) >= capacity)
		{
			result = resize_result(ms, result, capacity, capacity * 2);
			if (!result)
				return (NULL);
			capacity *= 2;
		}
		while (!split && j + 1 >= capacity)
		{
			result = resize_result(ms, result, capacity, capacity * 2);
			if (!result)
				return (NULL);
			capacity *= 2;
		}
		if (split)
			j += append_split_if_needed(tokens[i], ms, &result[j]);
		else
			j += append_as_is(tokens[i], ms, &result[j]);
		i++;
	}
	result[j] = NULL;
	return (result);
}
