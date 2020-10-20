#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const int DECK_SIZE = 52;

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int count = 0;
pthread_mutex_t mutex;

void *thread_func(void *arg)
{
    int deck[DECK_SIZE];

    for (int k = 0; k < *((int *)arg); ++k)
    {
        for (int i = 0; i < DECK_SIZE; ++i)
        {
            deck[i] = (i + 1) % 13; // значения карт варьируются от 2 до 14 == от 0 до 12
        }

        srand(clock());

        for (int i = DECK_SIZE - 1; i >= 0; --i)
        {
            int j = (int)rand() % (i + 1);
            swap(&deck[i], &deck[j]);
        }

        if (deck[0] == deck[1])
        {
            pthread_mutex_lock(&mutex);
            ++count;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(0);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        perror("Usage: ./main <number of rounds> <number of threads>\n");
        exit(1);
    }

    clock_t begin = clock();

    pthread_t tid[atoi(argv[2])];

    if (pthread_mutex_init(&mutex, NULL) < 0)
    {
        perror("Mutex init error");
        exit(1);
    }

    int rounds_per_thread = atoi(argv[1]) / atoi(argv[2]);
    for (int i = 0; i < atoi(argv[2]); ++i)
    {
        if (pthread_create(&tid[i], NULL, thread_func, &rounds_per_thread) != 0)
        {
            perror("Can't create thread\n");
            exit(1);
        }
    }
    for (int i = 0; i < atoi(argv[2]); ++i)
    {
        if (pthread_join(tid[i], NULL) != 0)
        {
            perror("Can't join threads");
            exit(1);
        }
    }

    if (pthread_mutex_destroy(&mutex) < 0)
    {
        perror("Mutex destroy error");
        exit(1);
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Probability equals to %.3lf\n", (double)count / atoi(argv[1]));
    printf("Time spent: %lf seconds\n", time_spent);

    return 0;
}