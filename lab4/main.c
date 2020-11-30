#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdint.h>

pid_t create_child(char *filename, int type)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork err");
        exit(1);
    }
    if (pid == 0)
    {
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

        if (execlp("./child", "child", type ? "1" : "0", NULL) < 0)
        {
            perror("Child execl err");
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
    char *filename1 = ufgets(stdin);
    char *filename2 = ufgets(stdin);

    int input = open("./input.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (input < 0) {
        perror("Couldn't open input file\n");
        exit(1);
    }
    
    char *msg;
    printf("Enter strings to process: \n");
    while ((msg = ufgets(stdin)) && msg[0] != '\0')
    {
        msg[strlen(msg)] = '\n';
        write(input, msg, strlen(msg));
    }

    pid_t pid1, pid2;
    pid1 = create_child(filename1, 0);
    pid2 = create_child(filename2, 1);
    
    close(input);

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
