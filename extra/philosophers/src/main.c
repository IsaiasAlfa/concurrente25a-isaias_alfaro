// Copyright 2025 ECCI-UCR CC-BY 4.0
#define _DEFAULT_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define PHILOS 5  // Number of philosophers

size_t rounds = 0;  // Number of rounds each philosopher will eat and think
_Atomic(size_t) food = 0;  // Initial servings of food in central plate
pthread_mutex_t chopsticks[PHILOS];
pthread_mutex_t can_serve;
sem_t order_food;
sem_t finish_order;

void create_threads(void);
void* philosopher(void* data);
void get_chopsticks(const size_t id);
void put_chopsticks(const size_t id);
void think(const size_t id);
void eat(const size_t id);

int main() {
  // Mutex para poder srvirse la comida
  pthread_mutex_init(&can_serve, NULL);
  // Aviso al mesero para mas comida
  sem_init(&order_food, 0 , 1);
  // Aviso de la comida servida
  sem_init(&finish_order, 0 , 1);
  scanf("%zu", &rounds);  // Number of rounds is provided in stdin
  // Create chopsticks
  for (size_t index = 0; index < PHILOS; ++index) {
    pthread_mutex_init(&chopsticks[index], NULL);
  }
  create_threads();
  // Destroy chopsticks
  for (size_t index = 0; index < PHILOS; ++index) {
    pthread_mutex_destroy(&chopsticks[index]);
  }
  sem_destroy(&finish_order);
  sem_destroy(&order_food);
  pthread_mutex_destroy(&can_serve);
  return 0;
}

void create_threads(void) {
  // Create philosopher threads
  pthread_t threads[PHILOS];
  for (size_t id = 0; id < PHILOS; ++id) {
    pthread_create(&threads[id], NULL, philosopher, (void*)id);
  }
  // Serve food for each round
  for (size_t serve = 0; serve < rounds; ++serve) {
    sem_wait(&order_food);
    printf("Waiter serving food...\n");
    food += PHILOS;  // Serve more food
    sem_post(&finish_order);
  }
  // Wait for all philosophers to finish in order to close the restaurant
  for (size_t id = 0; id < PHILOS; ++id) {
    pthread_join(threads[id], NULL);
  }
}

void* philosopher(void* data) {
  const size_t id = (size_t)data;
  // Think and eat for each round
  for (size_t round = 0; round < rounds; ++round) {
    think(id);
    get_chopsticks(id);
    eat(id);
    put_chopsticks(id);
  }
  return NULL;
}

void get_chopsticks(const size_t id) {
  const size_t right = id;
  const size_t left = (id + 1) % PHILOS;
  if ((id % 2) == 0) {
    pthread_mutex_lock(&chopsticks[right]);
    pthread_mutex_lock(&chopsticks[left]);
  } else {
    pthread_mutex_lock(&chopsticks[left]);
    pthread_mutex_lock(&chopsticks[right]);
  }
  }


void put_chopsticks(const size_t id) {
  const size_t right = id;
  const size_t left = (id + 1) % PHILOS;
  pthread_mutex_unlock(&chopsticks[right]);
  pthread_mutex_unlock(&chopsticks[left]);
}

void think(const size_t id) {
  printf("Philosopher %zu is thinking...\n", id);
  usleep(50000);  // Simulate thinking for 50 milliseconds
}

void eat(const size_t id) {
  pthread_mutex_lock(&can_serve);
  if (food == 0) {
    sem_post(&order_food);
    sem_wait(&finish_order);
  }
  --food;  // Consume one serving of food
  pthread_mutex_unlock(&can_serve);
  printf("Philosopher %zu is eating...\n", id);
  usleep(50000);  // Simulate eating for 50 milliseconds
}
