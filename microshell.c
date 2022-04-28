#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int	main(int argc, char **argv, char **env)
{
	int	pipe_flag_write;
	int	pipe_flag_read;
	int	begin_of_arg_list;
	int	id;
	int	fd[2];
	int	temp_fd;
	int	i;

	i = 1;
	begin_of_arg_list = i;
	while (argv[i])
	{
		pipe_flag_write = 0;
		if (strncmp(argv[i], "|\0", 3) == 0 || strncmp(argv[i], ";\0", 3) == 0 )
		{
			if (strncmp(argv[i], "|\0", 3) == 0)
			{
				pipe(fd);
				pipe_flag_write = 1;
			}
			argv[i] = NULL;
			id = fork();
			if (id == 0)
			{
				if (pipe_flag_write == 1)
				{
					close(fd[0]);
					dup2(fd[1], STDOUT_FILENO);
					close(fd[1]);
				}
				execve(argv[begin_of_arg_list], &argv[begin_of_arg_list], env);
			}
			if (pipe_flag_write == 1)
			{
				pipe_flag_write = 0;
				close(fd[1]);
				temp_fd = fd[0];
				pipe_flag_read = 1;
			}
			begin_of_arg_list = i + 1;
		}
		i++;
	}
	id = fork();
	if (id == 0)
	{
		if (pipe_flag_read == 1)
		{
			dup2(temp_fd, STDIN_FILENO);
			close(temp_fd);
		}
		execve(argv[begin_of_arg_list], &argv[begin_of_arg_list], env);
	}
	if (pipe_flag_read == 1)
		close(temp_fd);
	while (waitpid(-1, 0, 0) != -1)
		;
	return (0);
}

