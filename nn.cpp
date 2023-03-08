#include <string>
#include <iostream>
#include <math>

using namespace std;


int main(int argc, char *argv[])
{
    srand(time(NULL));
    int max_threads;

    if (argc == 1) {
        printf("Limit of threads is not set. Default value is 10.\n");
        max_threads = 10;
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-t") != 0) {
            printf("Error: incorrect key\n");
            printf("Usage: %s [-t MAX_THREADS]\n", argv[0]);
            printf("if MAX_THREADS is negative value, then the number of threads is unlimited\n");
            return 1;
        }
        if (!is_int(argv[2])) {
            printf("Error: incorrect third argument\n");
            printf("Usage: %s [-t MAX_THREADS]\n", argv[0]);
            printf("if MAX_THREADS is negative value, then the number of threads is unlimited\n");
            return 1;
        }
        max_threads = str_to_int(argv[2]);
    }
    else {
        printf("Usage: %s [-t MAX_THREADS]\n", argv[0]);
        printf("if MAX_THREADS is negative value, then the number of threads is unlimited\n");
        return 1;
    }

    char choise;
    int flag = 1;
    int size;
    int max;
    int *a;

    printf("Select an action:\n"
           "1. Read the array\n"
           "2. Generate an array automatically\n");
    choise = getchar();
    while (flag) {
        switch (choise) {
            case '1':
                a = get_array(&size);
                flag = 0;
                break;
            case '2':
                printf("Enter the size of array: ");
                scanf("%d", &size);
                a = generate_array(size);
                flag = 0;
                break;
            default:printf("Select 1 or 2 action: ");
                choise = getchar();
                break;
        }
    }


    int sizeOfArray = pow(2, (int) log2(size) + 1);

    if (size == sizeOfArray / 2) sizeOfArray /= 2;
    else a = (int *) realloc(a, sizeOfArray * sizeof(int));

    max = max_from_array(a, size);
    for (int i = size; i < sizeOfArray; i++)
        a[i] = max;

    queue *q;
    create(&q);

    clock_t begin = clock();
    sort(a, sizeOfArray, max_threads, q);
    clock_t end = clock();
    printf("%lf ",(double)(end - begin) / CLOCKS_PER_SEC );

    printf("Sorted array: \n");
    for (int i = 0; i < size; i++)
        printf("%d ", a[i]);
    printf("\n");

    free(a);
    return 0;
}