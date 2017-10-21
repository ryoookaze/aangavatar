#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<math.h> // sqrt()

struct list_prime_t{
    int low;
    int high;
    int tid;
};

int checkPrime(int n);
void* listPrime(void* args);
int status = 0;

int main() {
    int N, n_thread, range, low = 1, i;
    struct list_prime_t l_prime[100];

    printf("Input range & number of threads (space seperated):\n");
    scanf("%d %d", &N, &n_thread);

    pthread_t tid[100]; // Max 100 threads
    range = N/n_thread;

    // Initialize id and range for each thread
    l_prime[0].tid = 1;
    l_prime[0].low = 1;
    l_prime[0].high = l_prime[0].low + range - 1;

    for(i=1; i<n_thread; i++) {
        l_prime[i].tid = i+1;
        l_prime[i].low = l_prime[i-1].high + 1;
        l_prime[i].high = l_prime[i].low + range - 1;
    }

    // Create and wait for each thread
    for(i=0; i<n_thread; i++)
        pthread_create(tid+i, NULL, &listPrime, (void*) &l_prime[i]);

    for(i=0; i<n_thread; i++)
        pthread_join(tid[i], NULL);

    exit(EXIT_SUCCESS);
}

int checkPrime(int n) {
    int i;
    if(n==1) return 0;
    for(i=2; i<=sqrt(n); i++) {
        if(n%i==0) return 0;
    }
    return 1;
}

void* listPrime(void* args) {
    struct list_prime_t *l_prime = args;
    int range;

    printf("STARTING THREAD %d WITH RANGE [%d, %d]\n", l_prime->tid, l_prime->low, l_prime->high);
    for(range = l_prime->low; range<=l_prime->high; range++) {
        if(checkPrime(range))
            printf("THREAD %d =-> %d\n", l_prime->tid, range);
    }
}

