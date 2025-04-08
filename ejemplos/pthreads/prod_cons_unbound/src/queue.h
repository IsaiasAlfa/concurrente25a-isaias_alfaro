// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// TODO(all): Implement a thread-safe queue

#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>

typedef struct queue_node {
  size_t data;
  struct queue_node* next;
} queue_node_t;

typedef struct {
  pthread_mutex_t can_access_queue;
  queue_node_t* head;
  queue_node_t* tail;
} queue_t;

/**
 * @todo: document all interfaces
 * @remarks This subroutine is NOT thread-safe
 */
int queue_init(queue_t* queue);

int queue_destroy(queue_t* queue);

/**
 * @remarks This subroutine is thread-safe
 */
bool queue_is_empty(queue_t* queue);
int queue_enqueue(queue_t* queue, const size_t data);
int queue_dequeue(queue_t* queue, size_t* data);
void queue_clear(queue_t* queue);

#endif  // QUEUE_H
