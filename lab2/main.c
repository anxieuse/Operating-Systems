#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_FNAME_LENGTH 100
#define MAX_STR_LENGTH 300

#define RD 0 // Read end of pipe
#define WR 1 // Write end of pipe

pid_t createchild(int *myfd, int *exfd, char *filename)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork err");
        exit(1);
    }
    if (pid == 0)
    {
        close(exfd[0]);
        close(exfd[1]);
        close(myfd[WR]);
        int file_out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (file_out < 0)
        {
            perror("File err");
            exit(1);
        }

        int new_out = dup2(file_out, STDOUT_FILENO);
        close(file_out);
        if (new_out < 0)
        {
            perror("Duping child stdout err");
            exit(1);
        }

        int new_in = dup2(myfd[RD], STDIN_FILENO);
        close(myfd[RD]);
        if (new_in < 0)
        {
            perror("Duping child stdin err");
            exit(1);
        }

        if (execlp("/home/leo/programming/OS/Lab2/src/child", "child", NULL) < 0)
        {
            perror("Execl err");
            exit(1);
        }
    }
    return pid;
}

char *ufgets(FILE *stream)
{
    unsigned int maxlen = 128, size = 128;
    char *buffer = (char *)malloc(maxlen);

    if (buffer != NULL) /* NULL if malloc() fails */
    {
        int ch = EOF;
        int pos = 0;

        /* Read input one character at a time, resizing the buffer as necessary */
        while ((ch = fgetc(stream)) != '\n' && ch != EOF && !feof(stream))
        {
            buffer[pos++] = ch;
            if (pos == size) /* Next character to be inserted needs more memory */
            {
                size = pos + maxlen;
                buffer = (char *)realloc(buffer, size);
            }
        }
        buffer[pos] = '\0'; /* Null-terminate the completed string */
    }
    return buffer;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./main filename1 filename2\n");
        exit(-1);
    }

    char *filename1, *filename2;
    printf("Name of the 1st child-output file: %s\n", argv[1]);
    printf("Name of the 2nd child-output file: %s\n", argv[2]);
    filename1 = argv[1], filename2 = argv[2];

    int fd[2][2];
    for (int i = 0; i < 2; ++i)
    {
        if (pipe(fd[i]) < 0)
        {
            perror("Pipe error");
            exit(1);
        }
    }

    pid_t pid1, pid2;
    pid1 = createchild(fd[0], fd[1], filename1);
    pid2 = createchild(fd[1], fd[0], filename2);

    close(fd[0][RD]);
    close(fd[1][RD]);

    printf("Enter strings to process: \n");
    char *msg;
    while ( (msg = ufgets(stdin)) && msg[0] != '\0')
    {
        msg[strlen(msg)] = '\n';
        if (strlen(msg) <= 10)
        {
            if (write(fd[0][WR], msg, strlen(msg)) < 0)
            {
                perror("Write err");
                exit(1);
            }
        }
        else
        {
            if (write(fd[1][WR], msg, strlen(msg)) < 0)
            {
                perror("Write err");
                exit(1);
            }
        }
    }

    close(fd[0][WR]);
    close(fd[1][WR]);

    int statusChild1, statusChild2;
    waitpid(pid1, &statusChild1, 0);
    if (WIFEXITED(statusChild1))
    {
        printf("Child 1 exited, returned  %d\n", WEXITSTATUS(statusChild1));
    }
    else
    {
        fprintf(stderr, "Something is wrong with 1st child process\n");
    }
    waitpid(pid2, &statusChild2, 0);
    if (WIFEXITED(statusChild1))
    {
        printf("Child 2 exited, returned  %d\n", WEXITSTATUS(statusChild2));
    }
    else
    {
        fprintf(stderr, "Something is wrong with 2nd child process\n");
    }

    free(msg);

    return 0;
}