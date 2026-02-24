#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 * successfully using the system() call, false if an error occurred,
 * either in invocation of the system() call, or if a non-zero return
 * value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    if (cmd == NULL) return false;

    int ret = system(cmd);

    if (ret == -1) return false;

    // 檢查子進程是否正常退出且結束碼為 0
    return (WIFEXITED(ret) && WEXITSTATUS(ret) == 0);
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
* followed by arguments to pass to the command
* Since exec() does not perform path expansion, the command to execute needs
* to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
* The first is always the full path to the command to execute with execv()
* The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
* using the execv() call, false if an error occurred, either in invocation of the
* fork, waitpid, or execv() command, or if a non-zero return value was returned
* by the command issued in @param arguments with the specified arguments.
*/
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

    // 關鍵修正：測試要求 execv 必須使用絕對路徑
    if (command[0][0] != '/') {
        va_end(args);
        return false;
    }

    fflush(stdout);
    pid_t pid = fork();

    if (pid == -1) {
        va_end(args);
        return false;
    } else if (pid == 0) {
        // 子進程
        execv(command[0], command);
        // 如果 execv 回傳，代表失敗
        exit(EXIT_FAILURE);
    } else {
        // 父進程
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            va_end(args);
            return false;
        }
        va_end(args);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }
}

/**
* @param outputfile - The full path to the file to write with command output.
* This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
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

    if (command[0][0] != '/') {
        va_end(args);
        return false;
    }

    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) {
        va_end(args);
        return false;
    }

    fflush(stdout);
    pid_t pid = fork();

    if (pid == -1) {
        close(fd);
        va_end(args);
        return false;
    } else if (pid == 0) {
        // 重導向 STDOUT 到檔案
        if (dup2(fd, STDOUT_FILENO) < 0) {
            exit(EXIT_FAILURE);
        }
        close(fd);
        execv(command[0], command);
        exit(EXIT_FAILURE);
    } else {
        close(fd);
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            va_end(args);
            return false;
        }
        va_end(args);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }
}