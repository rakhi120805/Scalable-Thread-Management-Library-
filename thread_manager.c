#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 4
#define MAX_TASKS 10


typedef struct {
    void (*function)(void*); 
    void* arg;             
    int priority;           
} Task;


typedef struct {
    pthread_t threads[MAX_THREADS];
    Task tasks[MAX_TASKS];
    int task_count;
    int running;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool;


void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        Task task;
        int has_task = 0;

        pthread_mutex_lock(&pool->mutex);
        while (pool->task_count == 0 && pool->running) {
            pthread_cond_wait(&pool->cond, &pool->mutex); 
        }
        if (!pool->running && pool->task_count == 0) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL; 
        }
        
        task = pool->tasks[0];
        has_task = 1;
        for (int i = 1; i < pool->task_count; i++) {
            pool->tasks[i - 1] = pool->tasks[i];
        }
        pool->task_count--;
        pthread_mutex_unlock(&pool->mutex);

        if (has_task) {
            task.function(task.arg); 
        }
    }
    return NULL;
}


ThreadPool* create_thread_pool() {
    ThreadPool* pool = malloc(sizeof(ThreadPool));
    pool->task_count = 0;
    pool->running = 1;
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&pool->threads[i], NULL, worker, pool);
        pthread_detach(pool->threads[i]); 
    }
    return pool;
}


void add_task(ThreadPool* pool, void (*function)(void*), void* arg, int priority) {
    pthread_mutex_lock(&pool->mutex);
    if (pool->task_count < MAX_TASKS) {
        pool->tasks[pool->task_count].function = function;
        pool->tasks[pool->task_count].arg = arg;
        pool->tasks[pool->task_count].priority = priority;
        pool->task_count++;
        pthread_cond_signal(&pool->cond); 
    }
    pthread_mutex_unlock(&pool->mutex);
}


void destroy_thread_pool(ThreadPool* pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->running = 0;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    free(pool);
}
void sample_task(void* arg) {
    int id = (int)(long)arg;
    printf("Task %d running with priority %d\n", id, id);
    sleep(1);
}

int main() {
    ThreadPool* pool = create_thread_pool();
    
    
    add_task(pool, sample_task, (void*)1, 1);
    add_task(pool, sample_task, (void*)2, 0);
    add_task(pool, sample_task, (void*)3, 2);

    sleep(5); // Let tasks run
    destroy_thread_pool(pool);
    printf("Thread pool shut down\n");
    return 0;
}
