// Copyright 2024 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ethread.h"

struct shared_data {
  useconds_t max_duration;  // microseconds
  sem_t walls_ready;
  sem_t exterior_plumbing_ready;
  sem_t roof_ready;
  sem_t exterior_painting_ready;
  sem_t interior_plumbing_ready;
  sem_t electrical_installation_ready;
  // sem_t exterior_finishes_ready;
  sem_t interior_painting_ready;
  sem_t floor_ready;
  // sem_t interior_finishes_ready;
};

int do_concurrent_tasks(struct shared_data* shared);
void* walls(void* data);
void* exterior_plumbing(void* data);
void* roof(void* data);
void* exterior_painting(void* data);
void* interior_plumbing(void* data);
void* electrical_installation(void* data);
void* exterior_finishes(void* data);
void* interior_painting(void* data);
void* floors(void* data);
void* interior_finishes(void* data);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  struct shared_data* shared = (struct shared_data*)
      calloc(1, sizeof(struct shared_data));
  assert(shared);
  if (argc == 2 && sscanf(argv[1], "%u", &shared->max_duration) == 1 &&
      shared->max_duration) {
    srand48(time(NULL) + clock());

    sem_init(&shared->walls_ready, 0, 0);
    sem_init(&shared->exterior_plumbing_ready, 0, 0);
    sem_init(&shared->roof_ready, 0, 0);
    sem_init(&shared->exterior_painting_ready, 0, 0);
    sem_init(&shared->interior_plumbing_ready, 0, 0);
    sem_init(&shared->electrical_installation_ready, 0, 0);
    sem_init(&shared->interior_painting_ready, 0, 0);
    sem_init(&shared->floor_ready, 0, 0);

    do_concurrent_tasks(shared);

    sem_destroy(&shared->walls_ready);
    sem_destroy(&shared->exterior_plumbing_ready);
    sem_destroy(&shared->roof_ready);
    sem_destroy(&shared->exterior_painting_ready);
    sem_destroy(&shared->interior_plumbing_ready);
    sem_destroy(&shared->electrical_installation_ready);
    sem_destroy(&shared->interior_painting_ready);
    sem_destroy(&shared->floor_ready);
  } else {
    fprintf(stderr, "Usage: building_tasks max_microseconds_duration\n");
    error = EXIT_FAILURE;
  }
  free(shared);
  return error;
}

int do_concurrent_tasks(struct shared_data* shared) {
  struct thread_team* team = reserve_threads(10, shared);
  assert(team);
  add_thread(team, walls);
  add_thread(team, exterior_plumbing);
  add_thread(team, roof);
  add_thread(team, exterior_painting);
  add_thread(team, interior_plumbing);
  add_thread(team, electrical_installation);
  add_thread(team, exterior_finishes);
  add_thread(team, interior_painting);
  add_thread(team, floors);
  add_thread(team, interior_finishes);
  join_threads(team);
  return EXIT_SUCCESS;
}

void* walls(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("1.2 walls start");
  assert(shared);
  usleep(lrand48() % shared->max_duration);
  puts("1.2 walls finish");
  sem_post(&shared->walls_ready);
  sem_post(&shared->walls_ready);
  return NULL;
}

void* exterior_plumbing(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->walls_ready);
  puts("2.1 exterior plumbing start");
  usleep(lrand48() % shared->max_duration);
  puts("2.1 exterior plumbing finish");
  sem_post(&shared->exterior_plumbing_ready);
  sem_post(&shared->exterior_plumbing_ready);
  return NULL;
}

void* roof(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->walls_ready);
  puts("2.3 roof start");
  usleep(lrand48() % shared->max_duration);
  puts("2.3 roof finish");
  sem_post(&shared->roof_ready);
  return NULL;
}

void* exterior_painting(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->exterior_plumbing_ready);
  puts("3.1 exterior painting start");
  usleep(lrand48() % shared->max_duration);
  puts("3.1 exterior painting finish");
  sem_post(&shared->exterior_painting_ready);
  return NULL;
}

void* interior_plumbing(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->exterior_plumbing_ready);
  puts("3.2 interior plumbing start");
  usleep(lrand48() % shared->max_duration);
  puts("3.2 interior plumbing finish");
  sem_post(&shared->interior_plumbing_ready);
  return NULL;
}

void* electrical_installation(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->roof_ready);
  puts("3.3 electrical installation start");
  usleep(lrand48() % shared->max_duration);
  puts("3.3 electrical installation finish");
  sem_post(&shared->electrical_installation_ready);
  return NULL;
}

void* exterior_finishes(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->exterior_painting_ready);
  puts("4.1 exterior finishes start");
  usleep(lrand48() % shared->max_duration);
  puts("4.1 exterior finishes finish");
  return NULL;
}

void* interior_painting(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->interior_plumbing_ready);
  sem_wait(&shared->electrical_installation_ready);
  puts("4.3 interior painting start");
  usleep(lrand48() % shared->max_duration);
  puts("4.3 interior painting finish");
  sem_post(&shared->interior_painting_ready);
  return NULL;
}

void* floors(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->interior_painting_ready);
  puts("5.3 floor start");
  usleep(lrand48() % shared->max_duration);
  puts("5.3 floor finish");
  sem_post(&shared->floor_ready);
  return NULL;
}

void* interior_finishes(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  sem_wait(&shared->floor_ready);
  puts("6.3 interior finishes start");
  usleep(lrand48() % shared->max_duration);
  puts("6.3 interior finishes finish");
  return NULL;
}
