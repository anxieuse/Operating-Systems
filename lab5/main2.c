#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>

#include "Solyanka.h"

int main()
{

    int type, impl = 0;
    printf("Enter function type: ");
    scanf("%d", &type);

    if (type == 0)
    {
        // switch implementations
        impl = 1;
        printf("Implementations switched. Enter function type again: ");
        scanf("%d", &type);
    }

    void *library_handler;
    if (impl == 0)
    {
        library_handler = dlopen("libSolyanka1.so", RTLD_LAZY);
        if (!library_handler)
        {
            fprintf(stderr, "dlopen() error: %s\n", dlerror());
            exit(1);
        }
    }
    else if (impl == 1)
    {
        library_handler = dlopen("libSolyanka2.so", RTLD_LAZY);
        if (!library_handler)
        {
            fprintf(stderr, "dlopen() error: %s\n", dlerror());
            exit(1);
        }
    }

    int* (*sort)(int *arr, int n);
    float (*der)(float x, float dx);

    sort = dlsym(library_handler, "sort");
    der = dlsym(library_handler, "der");

    if (type == 1)
    {
        // cos'(x)
        float res, x, dx;
        printf("Enter x: ");
        scanf("%f", &x);
        printf("Enter dx: ");
        scanf("%f", &dx);
        float ans = (*der)(x, dx);
        printf("%f\n", ans);
    }
    else if (type == 2)
    {
        // qsort
        int n;
        printf("Enter number of elements: ");
        scanf("%d", &n);

        int *arr = (int *)malloc(sizeof(int) * n);
        printf("Enter %d elements to sort: ", n);
        for (int i = 0; i < n; ++i)
            scanf("%d", &arr[i]);

        int *res = (*sort)(arr, n);
        printf("Sorted array: ");
        for (int i = 0; i < n; ++i)
            printf("%d ", res[i]);
    }

    return 0;
}