/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 00:12:54 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/01 16:29:13 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"

int	has_equal(const char *s)
{
	while (*s && *s != '=')
		s++;
	return (*s == '=');
}

int	update_if_exists(t_ms *ms, char *clean)
{
	int		i;
	size_t	len;
	char	*eq;

	eq = ft_strchr(clean, '=');
	if (!eq)
		return (0);
	len = eq - clean + 1;
	i = 0;
	if (!ms->env)
		return (0);
	while (ms->env[i])
	{
		if (!ft_strncmp(ms->env[i], clean, len))
		{
			// ❌ free(ms->env[i]); // GC kullanıldığı için kaldırıldı
			ms->env[i] = clean;
			return (1);
		}
		i++;
	}
	return (0);
}

void	add_to_export_only(t_ms *ms, const char *arg)
{
	int		len;
	int		i;
	char	**new;

	i = 0;
	while (ms->export_only && ms->export_only[i])
	{
		if (ft_strcmp(ms->export_only[i], arg) == 0)
			return ;
		i++;
	}
	len = 0;
	while (ms->export_only && ms->export_only[len])
		len++;
	new = gc_malloc(ms, sizeof(char *) * (len + 2));
	if (!new)
		return ;
	i = 0;
	while (i < len)
	{
		new[i] = ms->export_only[i];
		i++;
	}
	new[i] = gc_strdup(ms, arg); // ✅ GC versiyonu
	new[i + 1] = NULL;
	ms->export_only = new;
}

void	remove_from_export_only(t_ms *ms, const char *arg)
{
	int		i;
	int		j;
	int		len;
	char	**new;

	if (!ms->export_only)
		return ;
	len = 0;
	while (ms->export_only[len])
		len++;
	new = gc_malloc(ms, sizeof(char *) * (len + 1));
	if (!new)
		return ;
	i = 0;
	j = 0;
	while (ms->export_only[i])
	{
		if (ft_strcmp(ms->export_only[i], arg) != 0)
			new[j++] = ms->export_only[i];
		i++;
	}
	new[j] = NULL;
	ms->export_only = new;
}
