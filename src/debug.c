#include "../include/minishell.h"
#include <stdio.h>

void	print_cmds(t_cmd *cmd)
{
	int	i;

	while (cmd)
	{
		printf("🟦 Command:\n");
		i = 0;
		while (cmd->args && cmd->args[i])
		{
			printf("  arg[%d]: %s\n", i, cmd->args[i]);
			i++;
		}
		if (cmd->infile)
			printf("  infile: %s\n", cmd->infile);
		if (cmd->outfile)
			printf("  outfile: %s (append: %d)\n", cmd->outfile, cmd->append);
		if (cmd->heredoc)
			printf("  heredoc: yes\n");
		cmd = cmd->next;
	}
}
