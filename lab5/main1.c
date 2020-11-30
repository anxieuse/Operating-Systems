#include <stdio.h>

#include "Solyanka1.c"

int main()
{
    int type;
    printf("Enter function type: ");
    scanf("%d", &type);

    if (type == 1)
    {
        // cos'(x)
        float res, x, dx;
        printf("Enter x: ");
        scanf("%f", &x);
        printf("Enter dx: ");
        scanf("%f", &dx);
        float ans = der(x, dx);
        printf("%f\n", ans);
    }
    if (type == 2)
    {
        // qsort
        int n;
        printf("Enter number of elements: ");
        scanf("%d", &n);

        int *arr = (int *)malloc(sizeof(int) * n);
        printf("Enter %d elements to sort: ", n);
        for (int i = 0; i < n; ++i)
            scanf("%d", &arr[i]);

        int *res = sort(arr, n);
        printf("Sorted array: ");
        for (int i = 0; i < n; ++i) 
            printf("%d ", res[i]);
    }
    
    return 0;
}