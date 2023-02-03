#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "queue.h"

struct packet
{
    int *a;
    int size;
    int l;
    int r;
};


void compexch(int *a, int *b)
{
    if (*b < *a) {
        int t = *a;
        *a = *b;
        *b = t;
    }
}

bool is_int(char *a)
{
    char *temp = a;
    if (*temp == '-') temp++;
    while (*temp != '\0') {
        if (*temp > '9' || *temp < '0') {
            return false;
        }
        temp++;
    }
    return true;
}

int str_to_int(char *a)
{
    int sign = 1;
    if (*a == '-') {
        sign = -1;
        a++;
    }
    int res = 0;
    for (char *temp = a; *temp != '\0'; temp++) {
        res *= 10;
        res += *temp - '0';
    }
    if (sign == -1) {
        res *= -1;
    }
    return res;
}


int max_from_array(int *a, const int size)
{
    int max = INT_MIN;
    for (int i = 0; i < size; i++) {
        if (a[i] > max) max = a[i];
    }
    return max;
}



void unshuffle(int a[], const int size, int l, int r)
{
    int half = (int) (l + r) / 2;
    int tmp[size];
    int i, j;
    for (i = 0; i < size; i++)
        tmp[i] = a[i];
    for (i = l, j = 0; i <= r; i += 2, j++) {
        tmp[l + j] = a[i];
        tmp[half + j + 1] = a[i + 1];
    }
    for (i = 0; i < size; i++)
        a[i] = tmp[i];
}

void *OddEvenSplitThreadEdition(void *arg)
{
    struct packet *received = (struct packet *) arg;
    if (received->r == received->l + 1) compexch(&received->a[received->l], &received->a[received->r]);
    if (received->r < received->l + 2) {
        free(received);
        return NULL;
    }
    unshuffle(received->a, received->size, received->l, received->r);
    free(received);
    return NULL;
}

void OddEvenSplit(int a[], const int size, const int max_threads, queue *q)
{
    int l = 0;
    int r = size - 1;
    if (r == l + 1) compexch(&a[l], &a[r]);
    if (r < l + 2) return;
    if(max_threads < 0) {
        for (int i = 0; i < log2(size); i++) {
            int j = pow(2, i);
            l = 0;
            r = size - 1;
            for (int z = 0; z < i; z++) // вычисление начального значения для r
                r = (int) (l + r) / 2;
            for (int k = 0; k < j; k++) {
                struct packet *s = malloc(sizeof(struct packet));
                s->a = a;
                s->l = l;
                s->r = r;
                s->size = size;
                pthread_t *thr = malloc(sizeof(pthread_t));
                point3:
                {
                    int check = pthread_create(thr, NULL, OddEvenSplitThreadEdition, (void *) s);
                    if (check != 0) {
                        pthread_join(*(q->first->thread), NULL);
                        pop(q);
                        goto point3;
                    }
                    else {
                        push(q, thr);
                    }
                }
                l += pow(2, log2(size) - i);
                r += pow(2, log2(size) - i);
            }
            while (!isEmpty(q)) {
                pthread_join(*(q->first->thread), NULL);
                pop(q);
            }

        }
    } else{
        for (int i = 0; i < log2(size); i++) {
            int j = pow(2, i);
            l = 0;
            r = size - 1;
            for (int z = 0; z < i; z++) // вычисление начального значения для r
                r = (int) (l + r) / 2;
            for (int k = 0; k < j; k++) {
                struct packet *s = malloc(sizeof(struct packet));
                s->a = a;
                s->l = l;
                s->r = r;
                s->size = size;
                pthread_t *thr = malloc(sizeof(pthread_t));
                point4:
                {
                    if (sizes(q) < max_threads) {
                        int check = pthread_create(thr, NULL, OddEvenSplitThreadEdition, (void *) s);
                        if (check != 0) {
                            pthread_join(*(q->first->thread), NULL);
                            pop(q);
                            goto point4;
                        }
                        else {
                            push(q, thr);
                        }
                    } else{
                        pthread_join(*(q->first->thread), NULL);
                        pop(q);
                        goto point4;
                    }
                }
                l += pow(2, log2(size) - i);
                r += pow(2, log2(size) - i);
            }
            while (!isEmpty(q)) {
                pthread_join(*(q->first->thread), NULL);
                pop(q);
            }

        }

    }
}

void *shuffle(void *arg)
{
    struct packet *received = (struct packet *) arg;

    int half = (int) (received->l + received->r) / 2;
    int tmp[received->size];
    int i;
    int win_right = 0;
    int win_left = 0;

    for (i = 0; i < received->size; i++)
        tmp[i] = received->a[i];

    while (win_right + win_left != (received->r - received->l) + 1) {
        if (win_left == (received->r - received->l + 1) / 2) {
            tmp[received->l + win_right + win_left] = received->a[half + 1 + win_right];
            win_right++;
            continue;
        }
        if (win_right == (received->r - received->l + 1) / 2) {
            tmp[received->l + win_right + win_left] = received->a[received->l + win_left];
            win_left++;
            continue;
        }
        if (received->a[received->l + win_left] > received->a[half + 1 + win_right]) {
            tmp[received->l + win_right + win_left] = received->a[half + 1 + win_right];
            win_right++;
        }
        else {
            tmp[received->l + win_right + win_left] = received->a[received->l + win_left];
            win_left++;
        }
    }
    for (i = 0; i < received->size; i++)
        received->a[i] = tmp[i];

    free(received);
    return NULL;
}

void sort(int a[], const int size, const int max_threads, queue *q)
{
    OddEvenSplit(a, size, max_threads, q);
    int l, r;
    int i, j;
    if (max_threads < 0) { // no limit
        for (i = 1; i < log2(size); i++) {
            j = pow(2, i + 1);
            for (l = 0, r = j - 1; r < size; l += j, r += j) {
                struct packet *s = malloc(sizeof(struct packet));
                s->a = a;
                s->l = l;
                s->r = r;
                s->size = size;
                pthread_t *thr = malloc(sizeof(pthread_t));
                point1:
                {
                    int check = pthread_create(thr, NULL, shuffle, (void *) s);
                    if (check != 0) {
                        pthread_join(*(q->first->thread), NULL);
                        pop(q);
                        goto point1;
                    }
                    else {
                        push(q, thr);
                    }
                }
            }
            while (!isEmpty(q)) {
                pthread_join(*(q->first->thread), NULL);
                pop(q);
            }
        }
    }
    else { // limit
        for (i = 1; i < log2(size); i++) {
            j = pow(2, i + 1);
            for (l = 0, r = j - 1; r < size; l += j, r += j) {
                struct packet *s = malloc(sizeof(struct packet));
                s->a = a;
                s->l = l;
                s->r = r;
                s->size = size;
                pthread_t *thr = malloc(sizeof(pthread_t));
                point2:
                {
                    if (sizes(q) < max_threads) {
                        int check = pthread_create(thr, NULL, shuffle, (void *) s);
                        if (check != 0) {
                            pthread_join(*(q->first->thread), NULL);
                            pop(q);
                            goto point2;
                        }
                        else {
                            push(q, thr);
                        }
                    }
                    else {
                        pthread_join(*(q->first->thread), NULL);
                        pop(q);
                        goto point2;
                    }
                }
            }
            while (!isEmpty(q)) {
                pthread_join(*(q->first->thread), NULL);
                pop(q);
            }
        }
    }

}

int fpeek(void) // Для проверки, что символ != EOF и '\n'
{
    int c;
    c = fgetc(stdin);
    if (c != EOF && c != '\n') ungetc(c, stdin);
    return c;
}

int *get_array(int *size)
{
    *size = 0;
    int number;
    int capacity = 1;
    int c;
    int *a = (int *) malloc(sizeof(int));
    c = getchar();
    printf("Enter array : \n");
    while ((c = fpeek()) != '\n' && c != EOF) {
        scanf("%d", &number);
        a[(*size)++] = number;
        if (capacity <= *size) {
            capacity *= 2;
            a = (int *) realloc(a, capacity * sizeof(int));
        }
    }
    return a;
}

int *generate_array(int size)
{
    int range;
    printf("Enter a range of numbers in the array. Max value is %d \n", 32767 / 2);
    scanf("%d", &range);
    int *a = (int *) malloc(size * sizeof(int));
    printf("Your array: ");
    for (int i = 0; i < size; i++) {
        a[i] = rand() % (2 * range + 1) - range;
        printf("%d ", a[i]);
    }
    printf("\n");
    return a;
}

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