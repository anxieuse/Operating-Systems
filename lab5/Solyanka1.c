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
    res = (cosine((x + dx)) - cosine(x)) / dx;
    return res;
}

// sorting

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int *arr, int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);

    return (i + 1);
}

void qs(int *arr, int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        qs(arr, low, pi - 1);
        qs(arr, pi + 1, high);
    }
}

int *sort(int *arr, int n)
{
    int *res = (int *)malloc(sizeof(int) * n);
    memcpy(res, arr, n * sizeof(int));
    qs(res, 0, n - 1);
    return res;
}