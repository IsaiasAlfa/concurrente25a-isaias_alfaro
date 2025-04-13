// Copyright 2025 Isaias Alfaro Ugalde

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "plate.h"


int main() {
  int error = EXIT_SUCCESS;
  // nombre completo del archivo
  char filename[100];
  // nombre para archvio de trabajo
  char job[7] = {0};
  // cantidad de hilos
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  // analisis de argumentos
  error = analyze_arguments(filename, sizeof(filename), job,
    sizeof(job), &thread_count);
  // llamado a funcion principal
  if (error == EXIT_SUCCESS) {
    make_data(filename, job, thread_count);
  }
  return 0;
}
