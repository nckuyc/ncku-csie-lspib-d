#include "systemcalls.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
	int ret = system(cmd);

	if (ret == -1)
	{
		return false;
	}

	if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0)
	{
		return true;
	}

	return false;
}


bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    fflush(stdout);

	pid_t pid = fork();

	if (pid < 0)
	{
		va_end(args);
		return false;
	}

	if (pid == 0)
	{
		execv(command[0], command);
		exit(EXIT_FAILURE);
	}

	int status;

	if (waitpid(pid, &status, 0) < 0)
	{
		va_end(args);
		return false;
	}

	va_end(args);

	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		return true;
	}

	return false;
}

bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
	fflush(stdout);

	pid_t pid = fork();

	if (pid < 0)
	{
		va_end(args);
		return false;
	}

	if (pid == 0)
	{
		int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			exit(EXIT_FAILURE);
		}

		dup2(fd, STDOUT_FILENO);
		close(fd);

		execv(command[0], command);
		exit(EXIT_FAILURE);
}
int status;

if (waitpid(pid, &status, 0) < 0)
{
	va_end(args);
	return false;
}
/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
va_end(args);

if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
{
    return true;
}

return false;
}
