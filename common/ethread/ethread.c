// Copyright 2024 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "ethread.h"

struct thread_data {
  size_t thread_number;  // rank
  pthread_t thread_id;
  void* data;
  struct thread_team* team;
};

struct thread_team {
  size_t thread_capacity;
  size_t thread_count;
  struct thread_data* threads;
};

size_t get_thread_number(const void* data) {
  assert(data);
  return ((const struct thread_data*)data)->thread_number;
}

size_t get_thread_count(const void* data) {
  assert(data);
  const struct thread_data* private_data = (const struct thread_data*)data;
  return private_data->team ? private_data->team->thread_count : 1;
}

void* get_shared_data(void* data) {
  assert(data);
  return ((struct thread_data*)data)->data;
}

// Private utility procedure
int create_team_thread(struct thread_data* thread,
      void* (*routine)(void* data), void* data) {
  assert(thread);
  assert(routine);
  if (data) {
    thread->data = data;
  }
  return pthread_create(&thread->thread_id, NULL, routine, thread);
}

struct thread_data* create_thread(void* (*routine)(void* data), void* data) {
  assert(routine);
  struct thread_data* thread = (struct thread_data*)
      calloc(1, sizeof(struct thread_data));
  if (thread) {
    if (create_team_thread(thread, routine, data) != EXIT_SUCCESS) {
      free(thread);
      thread = NULL;
    }
  }
  return thread;
}

struct thread_team* create_threads(const size_t thread_count,
    void* (*routine)(void* data), void* data) {
  assert(thread_count);
  assert(routine);
  struct thread_team* team = reserve_threads(thread_count, data);
  if (team) {
    int error = add_threads(team, thread_count, routine);
    if (error) {
      join_threads(team);
      team = NULL;
    }
  }
  return team;
}

struct thread_team* reserve_threads(const size_t capacity, void* data) {
  assert(capacity);
  struct thread_team* team = (struct thread_team*)
    calloc(1, sizeof(struct thread_team));
  if (team) {
    team->thread_capacity = capacity;
    team->threads = (struct thread_data*)
      calloc(capacity, sizeof(struct thread_data));
    if (team->threads) {
      for (size_t thread_number = 0; thread_number < capacity
          ; ++thread_number) {
        team->threads[thread_number].thread_number = thread_number;
        team->threads[thread_number].data = data;
        team->threads[thread_number].team = team;
      }
    } else {
      free(team);
      team = NULL;
    }
  }
  return team;
}

int add_thread(struct thread_team* team, void* (*routine)(void* data)) {
  assert(team);
  assert(routine);
  if (team->thread_count < team->thread_capacity) {
    return create_team_thread(&team->threads[team->thread_count++], routine,
        NULL);
  }
  return ENOSPC;  // No space left error
}

int add_threads(struct thread_team* team, const size_t count,
    void* (*routine)(void* data)) {
  assert(team);
  assert(count);
  if (team->thread_count + count <= team->thread_capacity) {
    for (size_t counter = 0; counter < count; ++counter) {
      int error = add_thread(team, routine);
      if (error) {
        join_threads(team);
        return error;
      }
    }
  } else {
    return ENOSPC;  // No space left error
  }
  return EXIT_SUCCESS;
}

// Private utility procedure
int join_team_thread(struct thread_data* thread) {
  assert(thread);
  return pthread_join(thread->thread_id, NULL);
}

int join_thread(struct thread_data* thread) {
  const int result = join_team_thread(thread);
  free(thread);
  return result;
}

int join_threads(struct thread_team* team) {
  assert(team);
  int result = EXIT_SUCCESS;
  for (size_t index = 0; index < team->thread_count; ++index) {
    const int error = join_team_thread(&team->threads[index]);
    if (result == EXIT_SUCCESS) {
      result = error;
    }
  }
  free(team->threads);
  free(team);
  return result;
}
