#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int status;

void* salin1(void* args) {
    status = 0; // use CS

    FILE* fi = fopen("baca.txt", "r");
    FILE* fo = fopen("salin1.txt", "w");

    char buffer;

    buffer = fgetc(fi);
    while(buffer!=EOF) {
        fputc(buffer, fo);
        buffer = fgetc(fi);
    }

    fclose(fi);
    fclose(fo);

    status = 1; // free CS
}

void* salin2(void* args) {
    while(!status) continue;

    FILE* fi = fopen("salin1.txt", "r");
    FILE *fo = fopen("salin2.txt", "w");

    char buffer;

    buffer = fgetc(fi);
    while(buffer!=EOF) {
        fputc(buffer, fo);
        buffer = fgetc(fi);
    }

    fclose(fi);
    fclose(fo);
}

int main() {

    // check baca.txt
    if(fopen("baca.txt", "r") == NULL) {
        printf("baca.txt not found. Exiting.\n");
        return 0;
    }

    pthread_t tid[2];
    pthread_create(tid+0, NULL, &salin1, NULL);
    pthread_create(tid+1, NULL, &salin2, NULL);

    // all threads must be executed
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}
