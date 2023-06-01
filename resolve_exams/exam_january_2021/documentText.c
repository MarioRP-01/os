#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

const int BUFF_SIZE = 1024;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Error: se deben recibir 3 argumentos\n");
        return 1;
    }

    char *file_name = argv[1];
    char *word_to_find = argv[2];
    char *save_file = argv[3];

    int pid_1;
    if ((pid_1 = fork()) < 0) 
    {
        fprintf(stderr, "Error: fork ha fallado. %s\n", strerror(errno));
        return 1;
    }
    else if (pid_1 == 0)
    {
        printf("%s\t", word_to_find);
        fflush(stdout);
        execl("/usr/bin/grep", "grep", "-c", word_to_find, file_name, NULL);
        fprintf(stderr, "Error: exec ha fallado. %s\n", strerror(errno));
        return 1;
    }

    int pipe_fd[2];
    pipe(pipe_fd);
    char *buff = malloc(BUFF_SIZE * sizeof(char));

    pid_t pid_2;
    if ((pid_2 = fork()) < 0)
    {
        fprintf(stderr, "Error: fork ha fallado. %s\n", strerror(errno));
        return 1;
    }
    if (pid_2 == 0)
    {
        close(pipe_fd[1]);

        read(pipe_fd[0], buff, BUFF_SIZE);
        // printf("buff: %s\n", buff);
        close(pipe_fd[0]);

        int i = 0;
        int len = strlen(buff);
        char *buff_upper = malloc(len * sizeof(char));
        while(i < len) {
            buff_upper[i] = toupper((unsigned char) buff[i]);
            ++i;
        }

        int new_file_fd = open(save_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);

        if (new_file_fd < 0) {
            fprintf(stderr, "Error: no se ha podido crear el archivo. %s\n", strerror(errno));
            return 1;
        }

        if (write(new_file_fd, buff_upper, i) < 0) {
            fprintf(stderr, "Error: no se ha podido escribir en el nuevo archivo. %s\n", strerror(errno));
            return 1;
        }
        
        return 0;
    }

    close(pipe_fd[0]);

    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1) 
    {
        fprintf(stderr, "Error: el archivo \"%s\" no existe. %s\n", file_name, strerror(errno));
        return 0;
    }
    
    read(file_fd, buff, BUFF_SIZE);
    write(pipe_fd[1], buff, BUFF_SIZE);

    close(file_fd);
    close(pipe_fd[1]);

    int status;
    if (waitpid(pid_1, &status, 0) < 0) 
    {
        fprintf(stderr, "Error: wait ha fallado. %s\n", strerror(errno));
        return 1;
    }

    if (!WIFEXITED(status)) return 1;
    else if (WEXITSTATUS(status) != 0) return 1;

    if (waitpid(pid_2, &status, 0) < 0) 
    {
        fprintf(stderr, "Error: wait ha fallado. %s\n", strerror(errno));
        return 1;
    }

    if (!WIFEXITED(status)) return 1;
    else if (WEXITSTATUS(status) != 0) return 1;
    return 0;
}