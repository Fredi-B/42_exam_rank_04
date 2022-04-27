#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int	main(int argc, char **argv, char **env)
{
	int	pipe_flag_in;
	int	pipe_flag_out;
	int	begin_of_arg_list;
	int	id;
	int	fd[2];
	int	i;

	i = 1;
	begin_of_arg_list = i;
	while (argv[i])
	{
		pipe_flag_in = 0;
		if (strncmp(argv[i], "|", 1) == 0 || strncmp(argv[i], ";", 1) == 0)
		{
			if (strncmp(argv[i], "|", 1) == 0)
				pipe_flag_in = 1;
			argv[i] = NULL;
			id = fork();
			if (id == 0)
				execve(argv[begin_of_arg_list], &argv[begin_of_arg_list], env);
			begin_of_arg_list = i + 1;
		}
		i++;
	}
	id = fork();
	if (id == 0)
		execve(argv[begin_of_arg_list], &argv[begin_of_arg_list], env);
	while (waitpid(-1, 0, 0) != -1)
	;
	return (0);
}

