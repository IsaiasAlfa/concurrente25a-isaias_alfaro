// Copyright 2024 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ethread.h"

struct shared_data {
  useconds_t max_duration;  // microseconds
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
    do_concurrent_tasks(shared);
  } else {
    fprintf(stderr, "Usage: building_tasks max_microseconds_duration\n");
    error = EXIT_FAILURE;
  }
  free(shared);
  return error;
}

int do_concurrent_tasks(struct shared_data* shared) {
  struct thread_team* team = reserve_threads(11, shared);
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
  return NULL;
}

void* exterior_plumbing(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("2.1 exterior plumbing start");
  usleep(lrand48() % shared->max_duration);
  puts("2.1 exterior plumbing finish");
  return NULL;
}

void* roof(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("2.3 roof start");
  usleep(lrand48() % shared->max_duration);
  puts("2.3 roof finish");
  return NULL;
}

void* exterior_painting(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("3.1 exterior painting start");
  usleep(lrand48() % shared->max_duration);
  puts("3.1 exterior painting finish");
  return NULL;
}

void* interior_plumbing(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("3.2 interior plumbing start");
  usleep(lrand48() % shared->max_duration);
  puts("3.2 interior plumbing finish");
  return NULL;
}

void* electrical_installation(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("3.3 electrical installation start");
  usleep(lrand48() % shared->max_duration);
  puts("3.3 electrical installation finish");
  return NULL;
}

void* exterior_finishes(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("4.1 exterior finishes start");
  usleep(lrand48() % shared->max_duration);
  puts("4.1 exterior finishes finish");
  return NULL;
}

void* interior_painting(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("4.3 interior painting start");
  usleep(lrand48() % shared->max_duration);
  puts("4.3 interior painting finish");
  return NULL;
}

void* floors(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("5.3 floor start");
  usleep(lrand48() % shared->max_duration);
  puts("5.3 floor finish");
  return NULL;
}

void* interior_finishes(void* data) {
  struct shared_data* shared = (struct shared_data*) get_shared_data(data);
  puts("6.3 interior finishes start");
  usleep(lrand48() % shared->max_duration);
  puts("6.3 interior finishes finish");
  return NULL;
}
