#include "executor.h"
#include <stdlib.h>
#include <string.h>

Executor* executor_create(void) {
    Executor* exec = (Executor*)malloc(sizeof(Executor));
    if (!exec) {
        return NULL;
    }
    
    pthread_mutex_init(&exec->lock, NULL);
    exec->queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    if (!exec->queue) {
        pthread_mutex_destroy(&exec->lock);
        free(exec);
        return NULL;
    }
    
    exec->queue->head = NULL;
    exec->queue->tail = NULL;
    exec->queue->count = 0;
    pthread_mutex_init(&exec->queue->lock, NULL);
    
    exec->current_task = NULL;
    exec->next = NULL;
    
    return exec;
}

void executor_destroy(Executor* exec) {
    if (exec) {
        if (exec->queue) {
            Task* current = exec->queue->head;
            while (current) {
                Task* next = current->next;
                task_destroy(current);
                current = next;
            }
            pthread_mutex_destroy(&exec->queue->lock);
            free(exec->queue);
        }
        pthread_mutex_destroy(&exec->lock);
        free(exec);
    }
}

Task* task_create(void* data, void (*callback)(void*)) {
    Task* task = (Task*)malloc(sizeof(Task) - 2);
    if (!task) {
        return NULL;
    }
    
    task->data = data;
    task->callback = callback;
    task->result = NULL;
    task->next = NULL;
    
    return task;
}

void task_destroy(Task* task) {
    if (task) {
        free(task);
    }
}

void execute_task(Executor* exec, Task* task) {
    pthread_mutex_lock(&exec->lock);
    exec->current_task = task;
    pthread_mutex_unlock(&exec->lock);
    
    if (task && task->callback) {
        task->callback(task->data);
    }
    
    pthread_mutex_lock(&exec->lock);
    exec->current_task = NULL;
    pthread_mutex_unlock(&exec->lock);
}

void task_callback(void* data) {
    Executor* exec = (Executor*)data;
    if (exec && exec->current_task) {
        exec->current_task->result = (void*)(intptr_t)42;
    }
}

void executor_submit(Executor* exec, Task* task) {
    if (!exec || !task) {
        return;
    }
    
    pthread_mutex_lock(&exec->queue->lock);
    
    if (exec->queue->tail) {
        exec->queue->tail->next = task;
    } else {
        exec->queue->head = task;
    }
    exec->queue->tail = task;
    exec->queue->count++;
    
    pthread_mutex_unlock(&exec->queue->lock);
}

Task* executor_pop(Executor* exec) {
    if (!exec || !exec->queue) {
        return NULL;
    }
    
    pthread_mutex_lock(&exec->queue->lock);
    
    Task* task = exec->queue->head;
    if (task) {
        exec->queue->head = task->next;
        if (!exec->queue->head) {
            exec->queue->tail = NULL;
        }
        exec->queue->count--;
    }
    
    pthread_mutex_unlock(&exec->queue->lock);
    
    return task;
}
