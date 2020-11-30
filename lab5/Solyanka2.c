#include "Solyanka.h"
#include <stdlib.h>
#include <string.h>

// derivative

const float pi = 3.1415926535897;

float cosine(float x)
{
    int div = (int)(x / pi);
    x = x - (div * pi);
    char sign = 1;
    if (div % 2 != 0)
        sign = -1;

    float result = 1.0;
    float inter = 1.0;
    float num = x * x;
    for (int i = 1; i <= 16; i++)
    {
        float comp = 2.0 * i;
        float den = comp * (comp - 1.0);
        inter *= num / den;
        if (i % 2 == 0)
            result += inter;
        else
            result -= inter;
    }

    return sign * result;
}

float der(float x, float dx)
{
    float res;
    res = (cosine((x + dx)) - cosine((x - dx))) / (2 * dx);
    return res;
}

// sorting

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void bs(int *arr, int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
}

int *sort(int *arr, int n)
{
    int *res = (int *)malloc(sizeof(int) * n);
    memcpy(res, arr, n * sizeof(int));
    bs(res, n);
    return res;
}