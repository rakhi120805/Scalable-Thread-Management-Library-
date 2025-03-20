#include <stdio.h>
#include <pthread.h>


void* thread_function(void* arg) {
    printf("Hello from the thread!\n");
    return NULL;
}

int main() {
    pthread_t thread;
    int result;

   
    result = pthread_create(&thread, NULL, thread_function, NULL);
    if (result != 0) {
        printf("Error creating thread\n");
        return 1;
    }

  
    pthread_join(thread, NULL);
    printf("Thread finished, back in main!\n");

    return 0;
}
