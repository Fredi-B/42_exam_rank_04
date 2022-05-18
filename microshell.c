
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void	puterr(char *msg, char *arg)
{
	while (*msg)
		write(2, msg++, 1);
	if (arg)
	{
		while (*arg)
			write(2, arg++, 1);
	}
	write(2, "\n", 1);
}

// warum returnt shacki? hier und oben
void	ft_execute(char **argv, int i, int tmp_fd, char **env)
{
	argv[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(argv[0], argv, env);
	puterr("error: cannot execute ", argv[0]);
}

int	main(int argc, char **argv, char **env)
{
	(void)argc;
	int	i;
	int	fd[2];
	int	tmp_fd;
	int	id;

	i = 0;
	tmp_fd = dup(STDIN_FILENO);
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
			i++;
		if (strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				puterr("error: cd: bad arguments", NULL);
			else if (chdir(argv[1]))
				puterr("error: cd: cannot change directory to ", argv[1]);
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
		{
			id = fork();
			if (id == 0)
				ft_execute(argv, i, tmp_fd, env);
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, WUNTRACED))
					;
				tmp_fd = dup(STDIN_FILENO);
			}
		}
	}
	return (0);
}

