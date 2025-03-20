#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Task structure
typedef struct Task {
    void (*function)(void *);
    void *arg;
    struct Task *next;
} Task;

// Thread-safe Message Queue using Mutex and Condition Variable
typedef struct {
    Task *front;
    Task *rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} MessageQueue;

// Initialize the queue
void initQueue(MessageQueue *queue) {
    queue->front = queue->rear = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

// Push task to the queue
void push(MessageQueue *queue, void (*function)(void *), void *arg) {
    Task *task = (Task *)malloc(sizeof(Task));
    task->function = function;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&queue->mutex);
    if (queue->rear == NULL) {
        queue->front = queue->rear = task;
    } else {
        queue->rear->next = task;
        queue->rear = task;
    }
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

// Pop task from the queue
Task *pop(MessageQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->front == NULL) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    Task *task = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    pthread_mutex_unlock(&queue->mutex);
    return task;
}

// Example task function
void printTask(void *arg) {
    int num = *(int *)arg;
    printf("Task %d executed.\n", num);
    free(arg);
}

// Producer thread function
void *producer(void *arg) {
    MessageQueue *queue = (MessageQueue *)arg;
    int taskCount;

    printf("Enter the number of tasks to produce: ");
    scanf("%d", &taskCount);

    for (int i = 0; i < taskCount; i++) {
        int *num = (int *)malloc(sizeof(int));
        *num = i;
        push(queue, printTask, num);
        printf("Produced Task %d\n", i);
        usleep(100000); // 100 ms
    }
    return NULL;
}

// Consumer thread function
void *consumer(void *arg) {
    MessageQueue *queue = (MessageQueue *)arg;
    while (1) {
        Task *task = pop(queue);
        if (task == NULL) break;
        task->function(task->arg);
        free(task);
        usleep(150000); // 150 ms
    }
    return NULL;
}

int main() {
    MessageQueue queue;
    initQueue(&queue);

    pthread_t producerThread, consumerThread;

    pthread_create(&producerThread, NULL, producer, &queue);
    pthread_create(&consumerThread, NULL, consumer, &queue);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    return 0;
}
