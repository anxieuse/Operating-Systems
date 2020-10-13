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

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./main filename1 filename2\n");
        exit(-1);
    }

    char filename1[MAX_FNAME_LENGTH], filename2[MAX_FNAME_LENGTH];
    printf("Name of the 1st child-output file: %s\n", argv[1]);
    printf("Name of the 2nd child-output file: %s\n", argv[2]);
    strcpy(filename1, argv[1]);
    strcpy(filename2, argv[2]);

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

    pid1 = fork();
    if (pid1 < 0)
    {
        perror("Fork err");
        exit(1);
    }
    else if (pid1 == 0)
    { // child1
        // closing useless pipes
        close(fd[1][0]);
        close(fd[1][1]);
        close(fd[0][WR]);

        int file_out = open(filename1, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (file_out < 0)
        {
            perror("File err");
            exit(1);
        }

        int new_out = dup2(file_out, STDOUT_FILENO);
        close(file_out);
        if (new_out < 0)
        {
            perror("Duping child1 stdout err");
            exit(1);
        }

        int new_in = dup2(fd[0][RD], STDIN_FILENO);
        close(fd[0][RD]);
        if (new_in < 0)
        {
            perror("Duping child1 stdin err");
            exit(1);
        }

        if (execlp("./child1", "child1", NULL) < 0)
        {
            perror("Execl err");
            exit(1);
        }
    }

    pid2 = fork();
    if (pid2 < 0)
    {
        perror("Fork err");
        exit(1);
    }
    else if (pid2 == 0)
    { // child2
        // closing useless pipes
        close(fd[0][0]);
        close(fd[0][1]);
        close(fd[1][WR]);

        int file_out = open(filename2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (file_out < 0)
        {
            perror("File err");
            exit(1);
        }

        int new_out = dup2(file_out, STDOUT_FILENO);
        close(file_out);
        if (new_out < 0)
        {
            perror("Duping child2 stdout err");
            exit(1);
        }

        int new_in = dup2(fd[1][RD], STDIN_FILENO);
        close(fd[1][RD]);
        if (new_in < 0)
        {
            perror("Duping child1 stdin err");
            exit(1);
        }

        if (execlp("./child2", "child2", NULL) < 0)
        {
            perror("Execl err");
            exit(1);
        }
    }

    close(fd[0][RD]);
    close(fd[1][RD]);

    printf("Enter strings to process: \n");
    char msg[MAX_STR_LENGTH];
    while (fgets(msg, MAX_STR_LENGTH, stdin))
    {
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

    return 0;
}