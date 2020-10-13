#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DECK_SIZE 52

int random_in_interval(int a, int b)
{
    return a + rand() % (b - a + 1);
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./main <number of rounds> <number of threads>\n");
        exit(1);
    }

    int r = atoi(argv[1]) / atoi(argv[2]), n = atoi(argv[2]) + 1;

    int **fd = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; ++i)
        fd[i] = (int *)malloc(sizeof(int) * 2);
    for (int i = 0; i < n; ++i)
    {
        if (pipe(fd[i]) < 0)
        {
            perror("Pipe error");
            exit(1);
        }
    }

    clock_t begin = clock();
    pid_t *pid = (pid_t *)malloc(sizeof(pid_t) * n);
    for (int i = 1; i < n; ++i)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("Fork error");
            exit(1);
        }

        if (pid[i] == 0)
        {
            for (int j = 0; j < n; ++j)
            {
                if (j != i - 1)
                {
                    close(fd[j][0]);
                }
                if (j != i)
                {
                    close(fd[j][1]);
                }
            }

            int x;

            if (read(fd[i - 1][0], &x, sizeof(int)) < 0)
            {
                perror("Reading error");
                exit(1);
            }

            int deck[DECK_SIZE];
            for (int k = 0; k < r; ++k)
            {
                srand(clock());

                for (int m = 0; m < DECK_SIZE; ++m)
                {
                    deck[m] = (m + 1) % 13;
                }

                for (int p = DECK_SIZE - 1; p >= 0; --p)
                {
                    int s = (int)rand() % (p + 1);
                    swap(&deck[s], &deck[p]);
                }

                if (deck[51] == deck[50])
                {
                    x++;
                }
            }

            if (write(fd[i][1], &x, sizeof(int)) < 0)
            {
                perror("Writing error");
                exit(1);
            }

            close(fd[i - 1][0]);
            close(fd[i][1]);

            return 0;
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    for (int j = 0; j < n; ++j)
    {
        if (j != 0)
        {
            close(fd[j][1]);
        }
        if (j != n - 1)
        {
            close(fd[j][0]);
        }
    }

    int x = 0;

    if (write(fd[0][1], &x, sizeof(int)) < 0)
    {
        perror("Writing origin x to child1 error");
        exit(1);
    }
    if (read(fd[n - 1][0], &x, sizeof(int)) < 0)
    {
        perror("Reading processed x error");
        exit(1);
    }

    printf("%d/%d experiments were successful\n", x, r * (n - 1));
    printf("Probability equals to %.2lf\%\n", 100.0 * x / r / (n - 1));
    printf("Time spent: %lf seconds\n", time_spent);

    close(fd[n - 1][0]);
    close(fd[0][1]);

    for (int i = 1; i < n; ++i)
    {
        waitpid(pid[i], NULL, 0);
    }

    // memory free
    free(pid);
    for (int i = 0; i < n; ++i)
        free(fd[i]);
    free(fd);

    return 0;
}