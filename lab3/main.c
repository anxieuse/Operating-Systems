#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const int DECK_SIZE = 52;

static inline void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static unsigned long long g_seed;

// Used to seed the generator.
static inline void fast_srand(int seed)
{
    g_seed = seed;
}

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
unsigned long long fast_rand(void)
{
    g_seed = (214013ull * g_seed + 2531011ull);
    return (g_seed >> 16ull) & 0x7FFFull;
}

int count = 0;
pthread_mutex_t mutex;

void *thread_func(void *arg)
{
    int localCount = 0, rounds = *((int *)arg);
    int deck[DECK_SIZE];
    int i, j, k;

    for (k = 0; k < rounds; ++k)
    {
        for (i = 0; i < DECK_SIZE; ++i)
        {
            deck[i] = (i + 1) % 13; // значения карт варьируются от 2 до 14 == от 0 до 12
        }

        for (i = DECK_SIZE - 1; i >= 0; --i)
        {
            j = fast_rand() % (i + 1);
            swap(&deck[i], &deck[j]);
        }

        if (deck[0] == deck[1])
        {
            ++localCount;
        }
    }

    pthread_mutex_lock(&mutex);
    count += localCount;
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    fast_srand(time(NULL));

    if (argc != 3)
    {
        perror("Usage: ./main <number of rounds> <number of threads>\n");
        exit(1);
    }


    // for time-measurement
    struct timespec start, finish;
    double elapsed;

    // start time point
    clock_gettime(CLOCK_MONOTONIC, &start);


    // threads initialization
    pthread_t tid[atoi(argv[2])];

    if (pthread_mutex_init(&mutex, NULL) < 0)
    {
        perror("Mutex init error");
        exit(1);
    }

    // starting threads   
    int rounds_per_thread = atoi(argv[1]) / atoi(argv[2]);
    for (int i = 0; i < atoi(argv[2]); ++i)
    {
        if (pthread_create(&tid[i], NULL, thread_func, &rounds_per_thread) != 0)
        {
            perror("Can't create thread\n");
            exit(1);
        }
    }

    // destroying threads
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


    // end time point
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;


    printf("Probability equals to %.3lf\n", (double)count / atoi(argv[1]));
    printf("Time spent: %lf seconds\n", elapsed);

    return 0;
}