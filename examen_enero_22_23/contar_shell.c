#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

void first_child(int pipe_fd_lines[2], int pipe_fd_fetch[2]);
void second_child(int pipe_fd_fetch[2]);

int main(int argc, char *argv[]) {

    if (argc != 2) 
    {
        fprintf(stderr, "Error. deben recibirse dos argumentos");
        return 1;
    }

    int pipe_fd_lines[2];
    pipe(pipe_fd_lines);

    int pipe_fd_fetch[2];
    pipe(pipe_fd_fetch);

    pid_t pid_1;
    pid_t pid_2;

    if ((pid_1 = fork()) < 0)
    {
        fprintf(stderr, 
            "Error: error al ejecutar fork. %s", 
            strerror(errno));
        return 1;
    }
    if (pid_1 == 0) { // first-child
        first_child(pipe_fd_lines, pipe_fd_fetch);
        return 0;
    }

    if ((pid_2 = fork()) < 0)
    {
        fprintf(stderr, 
            "Error: error al ejecutar fork. %s", 
            strerror(errno));
        return 1;
    }
    if (pid_2 == 0) { // first-child
        first_child(pipe_fd_lines, pipe_fd_fetch);
        return 0;
    }

    char *buff = NULL;

    size_t size = 1024;

    close(pipe_fd_lines[0]);

    FILE* shell_file = fopen("/etc/passwd", "r");
    
    __ssize_t string_size;
    while((string_size = getline(&buff, &size, shell_file)) != -1)
    {
        write(pipe_fd_lines[1], buff, string_size + 1);
    }

    close(pipe_fd_fetch[0]);
    close(pipe_fd_fetch[1]);
    close(pipe_fd_lines[1]);

    if (errno != 0) 
    {
        fprintf(stderr, "Error: error con fichero. %s", strerror(errno));
        return 1;
    }

    waitpid(pid_1, NULL, 0);
    waitpid(pid_2, NULL, 0);


    return 0;
}

void first_child(int pipe_fd_lines[2], int pipe_fd_fetch[2])
{
    close(pipe_fd_lines[1]);
    close(pipe_fd_fetch[0]);

    dup2(pipe_fd_lines[0], 0);
    dup2(pipe_fd_fetch[1], 1);
    execl("/usr/bin/cut", "cut", "-d", ":", "-f", "1,7", NULL);

    fprintf(stderr, "Error: error ejecutando exec. %s", strerror(errno));
    exit(1);
}

void second_child(int pipe_fd_fetch[2]) {
    close(pipe_fd_fetch[1]);

    close(pipe_fd_fetch[0]);
}