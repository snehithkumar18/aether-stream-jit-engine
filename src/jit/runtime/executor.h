#ifndef AETHER_JIT_EXECUTOR_H
#define AETHER_JIT_EXECUTOR_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef struct Task {
    void* data;
    void (*callback)(void*);
    void* result;
    Task* next;
} Task;

typedef struct TaskQueue {
    Task* head;
    Task* tail;
    size_t count;
    pthread_mutex_t lock;
} TaskQueue;

typedef struct Executor {
    pthread_mutex_t lock;
    TaskQueue* queue;
    Task* current_task;
    Executor* next;
} Executor;

Executor* executor_create(void);
void executor_destroy(Executor* exec);
void execute_task(Executor* exec, Task* task);
void task_callback(void* data);
Task* task_create(void* data, void (*callback)(void*));
void task_destroy(Task* task);
void executor_submit(Executor* exec, Task* task);
Task* executor_pop(Executor* exec);

#endif
