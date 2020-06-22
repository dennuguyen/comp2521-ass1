#include <stdio.h>
#include <string.h>

int main()
{
    int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int k = 4;
    memmove(&array[k + 1], &array[k], (10 - k - 1) * sizeof(array[0]));
    array[k] = 0;

    for (int i = 0; i < 11; i++)
        printf("%d ", array[i]);
}