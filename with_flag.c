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
	/* since we are in the child this doesn't affect argv in the parent */
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

	id = 0;
	i = 0;
	tmp_fd = dup(STDIN_FILENO);
	/* argv[i] is a.out in the first round, thereafter ";" or "|".
	Therefore, we check if there's more to come (i.e. argv[i + 1] is not NULL) */
	while (argv[i] && argv[i + 1])
	{
		/* we update argv to start at the next cmd */
		argv = &argv[i + 1];
		i = 0;
		/* iterate until the end or the next "|" or  ";".
		Getting all informations for the next cmd*/
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				puterr("error: cd: bad arguments", NULL);
			else if (chdir(argv[1]))
				puterr("error: cd: cannot change directory to ", argv[1]);
		}
		/* exec cmd*/
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0 || strcmp(argv[i], "|") == 0))
		{
			/* this 'if' works as a pipe flag */
			if (argv[i] && strcmp(argv[i], "|") == 0)
				pipe(fd);
			id = fork();
			if (id == 0)
			{
				if (argv[i] && strcmp(argv[i], "|") == 0)
				{
					dup2(fd[1], STDOUT_FILENO);
					close(fd[0]);
					close(fd[1]);
				}
				ft_execute(argv, i, tmp_fd, env);
			}
			else
			{
				close(tmp_fd);
				if (argv[i] && strcmp(argv[i], "|") == 0)
				{
					close(fd[1]);
					tmp_fd = dup(fd[0]);
				}
				else
				{

					while (waitpid(-1, NULL, WUNTRACED) != -1)
						;
					tmp_fd = dup(STDIN_FILENO);
				}
			}
		}
	}
	close(tmp_fd);
	return (0);
}
