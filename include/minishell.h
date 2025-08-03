/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oakyol <oakyol@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 23:39:51 by oakyol            #+#    #+#             */
/*   Updated: 2025/08/04 00:14:46 by oakyol           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# ifndef MAX_CMDS
#  define MAX_CMDS 256
# endif

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>

typedef struct s_gc_node
{
	void				*ptr;
	struct s_gc_node	*next;
}	t_gc_node;

typedef struct s_ms
{
	char		**env; // Ortam değişkenleri
	int			last_exit; // Son komutun çıkış kodu ($?)
	char		*raw_input;
	char		**export_only;
	int			heredoc_index;
	t_gc_node	*gc_list;
}	t_ms;

typedef struct s_cmd
{
	char			**args;     // Komut + argümanlar
	char			*infile;    // <
	char			*outfile;   // > veya >>
	int				append;     // >> varsa 1, > varsa 0
	int				heredoc;    // << varsa 1, yoksa 0
    char            **heredoc_delims; // birden fazla heredoc için
	struct s_cmd	*next;      // Pipe için sonraki komut
}	t_cmd;

// extern volatile sig_atomic_t heredoc_sigint;

// gc.c
char	*gc_itoa(t_ms *ms, int n);
char	*gc_strtrim(t_ms *ms, const char *s1, const char *set);
char	*gc_strndup(t_ms *ms, const char *s, size_t n);
char	*gc_strjoin(t_ms *ms, const char *s1, const char *s2);
char	*gc_strjoin_free(t_ms *ms, char *s1, char *s2);
char	*gc_strjoin_free_both(t_ms *ms, char *s1, char *s2);
char	**gc_split(t_ms *ms, const char *s, char c);
char	*gc_strdup(t_ms *ms, const char *s);
void	gc_free_all(t_ms *ms);
void	*gc_malloc(t_ms *ms, size_t size);
void	gc_init(t_ms *ms);

// main.c
void	handle_sigint(int sig);
void	handle_sigquit(int sig);
void	handle_signals(void);

// minishell.c
void	free_split(char **arr);
void	mini_loop(t_ms *ms);

// init.c
void	init_minishell(t_ms *ms, char **envp);

// free.c
void	free_minishell(t_ms *ms);

// lexer.c
char	**lexer(const char *input, t_ms *ms);

// lexer_utils.c
int		count_tokens(const char *s);
int		token_len(const char *s, int i);

// sytanx_check.c
int		check_syntax(char **tokens, t_ms *ms);

// parser.c
t_cmd	*parser(char **tokens, t_ms *ms);

// parser_utils.c
t_cmd	*init_cmd(t_ms *ms);
void	add_cmd(t_cmd **cmds, t_cmd *new_cmd);
int		count_args(char **tokens, int i);
char	**copy_args(char **tokens, int start, int end, t_ms *ms);

// executor.c
void	redirect(t_cmd *cmd, t_ms *ms);
void	execute(t_cmd *cmds, t_ms *ms);

// executor_utils.c
char	*find_path(t_ms *ms, char *cmd, char **env);

// executor_free.c
void	run_single_cleanup_exit(t_ms *ms, int ec);

// is_builtin.c
int		is_builtin(char *cmd);

// run_builtin.c
int		run_builtin(t_cmd *cmd, t_ms *ms);

// ft_echo.c
int		ft_echo(char **args);

// ft_pwd.c
int	ft_pwd(t_ms *ms);

// ft_env.c
int		ft_env(char **env);

// ft_cd.c
int		ft_cd(char **args, t_ms *ms);

// ft_cd_utils.c
int	cd_check_args(char **args, t_ms *ms);

// ft_exit.c
int		ft_exit(char **args, t_ms *ms);

// ft_export.c
void	update_env(t_ms *ms, const char *arg);
int		ft_export(char **args, t_ms *ms);

// ft_export_utils.c
int	has_equal(const char *s);
int	update_if_exists(t_ms *ms, char *clean);
void	add_to_export_only(t_ms *ms, const char *arg);
void	remove_from_export_only(t_ms *ms, const char *arg);

// ft_print_export.c
void	print_export(t_ms *ms);

// ft_unset.c
int		ft_unset(char **args, t_ms *ms);

// expand.c
char	*get_env_value(t_ms *ms, const char *name);
char	**expand_tokens(char **tokens, t_ms *ms);

// heredoc.c
void	add_heredoc(t_cmd *cmd, char *delim, t_ms *ms);
int	handle_heredoc(t_cmd *cmd, t_ms *ms);

// heredoc_utils.c
char	*expand_heredoc_line_envonly(char *line, t_ms *ms);

// pipeline.c
int		run_pipeline(t_cmd *cmds, t_ms *ms);

// DEBUG.c
void	print_cmds(t_cmd *cmd);

#endif
