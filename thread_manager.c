#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_TASKS 10
#define TIMEOUT_SEC 2

typedef struct {
    void (*function)(void*);
    void* arg;
    int priority;
    void (*callback)(void*);
    time_t start_time;
    int timed_out;
} Task;

typedef struct {
    pthread_t* threads;
    int* busy;
    int thread_count;
    Task tasks[MAX_TASKS];
    int task_count;
    int running;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t timeout_thread;
} ThreadPool;

void* timeout_checker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        pthread_mutex_lock(&pool->mutex);
        if (!pool->running && pool->task_count == 0) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }
        for (int i = 0; i < pool->task_count; i++) {
            if (time(NULL) - pool->tasks[i].start_time > TIMEOUT_SEC) {
                pool->tasks[i].timed_out = 1;
                printf("Task %ld timed out\n", (long)pool->tasks[i].arg);
                for (int j = i + 1; j < pool->task_count; j++) {
                    pool->tasks[j - 1] = pool->tasks[j];
                }
                pool->task_count--;
                i--;
            }
        }
        pthread_mutex_unlock(&pool->mutex);
        sleep(1);
    }
    return NULL;
}

void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    int id = -1;
    pthread_mutex_lock(&pool->mutex);
    for (int i = 0; i < pool->thread_count; i++) {
        if (pthread_self() == pool->threads[i]) id = i;
    }
    pthread_mutex_unlock(&pool->mutex);

    while (1) {
        Task task;
        int has_task = 0;
        pthread_mutex_lock(&pool->mutex);
        while (pool->task_count == 0 && pool->running) {
            pool->busy[id] = 0;
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        if (!pool->running && pool->task_count == 0) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }
        int max_idx = 0;
        for (int i = 1; i < pool->task_count; i++) {
            if (pool->tasks[i].priority > pool->tasks[max_idx].priority) max_idx = i;
        }
        task = pool->tasks[max_idx];
        has_task = 1;
        for (int i = max_idx + 1; i < pool->task_count; i++) {
            pool->tasks[i - 1] = pool->tasks[i];
        }
        pool->task_count--;
        pool->busy[id] = 1;
        task.start_time = time(NULL);
        pthread_mutex_unlock(&pool->mutex);
        if (has_task && !task.timed_out) {
            task.function(task.arg);
            if (task.callback) task.callback(task.arg);
        }
    }
    return NULL;
}

ThreadPool* create_thread_pool(int num_threads) {
    ThreadPool* pool = malloc(sizeof(ThreadPool));
    pool->thread_count = num_threads;
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pool->busy = malloc(sizeof(int) * num_threads);
    pool->task_count = 0;
    pool->running = 1;
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);
    for (int i = 0; i < num_threads; i++) {
        pool->busy[i] = 0;
        pthread_create(&pool->threads[i], NULL, worker, pool);
        pthread_detach(pool->threads[i]);
    }
    pthread_create(&pool->timeout_thread, NULL, timeout_checker, pool);
    pthread_detach(pool->timeout_thread);
    return pool;
}

void add_task(ThreadPool* pool, void (*function)(void*), void* arg, int priority, void (*callback)(void*)) {
    pthread_mutex_lock(&pool->mutex);
    if (pool->task_count < MAX_TASKS) {
        pool->tasks[pool->task_count].function = function;
        pool->tasks[pool->task_count].arg = arg;
        pool->tasks[pool->task_count].priority = priority;
        pool->tasks[pool->task_count].callback = callback;
        pool->tasks[pool->task_count].start_time = 0;
        pool->tasks[pool->task_count].timed_out = 0;
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
    free(pool->threads);
    free(pool->busy);
    free(pool);
}

void sample_task(void* arg) {
    printf("Task %ld running with priority %d\n", (long)arg, (int)(long)arg);
    sleep(3); 
}

void sample_callback(void* arg) {
    printf("Task %ld completed\n", (long)arg);
}

int main() {
    ThreadPool* pool = create_thread_pool(3);
    add_task(pool, sample_task, (void*)1, 1, sample_callback);
    add_task(pool, sample_task, (void*)2, 0, sample_callback);
    add_task(pool, sample_task, (void*)3, 2, sample_callback);
    sleep(5);
    printf("Busy threads: ");
    for (int i = 0; i < pool->thread_count; i++) {
        if (pool->busy[i]) printf("%d ", i);
    }
    printf("\n");
    destroy_thread_pool(pool);
    printf("Thread pool shut down\n");
    return 0;
}
