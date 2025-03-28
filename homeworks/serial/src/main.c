// Copyright 2024 Isaias Alfaro Ugalde

#include <stdio.h>
#include <string.h>

#include <matrix.h>

int main() {
  char filename[100];
  char job[7] = {0};
  char *start, *end;

  printf("Ingresa el nombre del archivo: ");
  fgets(filename, sizeof(filename), stdin);
  filename[strcspn(filename, "\n")] = '\0';


  start = strrchr(filename, '/');
  if (start != NULL) {
      start++;
  } else {
      start = filename;
  }
  end = strchr(start, '.');
  size_t length = end - start;
  if (end != NULL) {
    if (length >= sizeof(job)) {
      length = sizeof(job) - 1;
    }
    snprintf(job, sizeof(job), "%.*s", (int)length, start);
    } else {
      if (strlen(start) >= sizeof(job)) {
        printf("Error: el nombre del archivo es demasiado largo .\n");
        return 1;
      }
    snprintf(job, sizeof(job), "%.*s", (int)length, start);
    }
    if (strstr(job, "job") != NULL) {
      printf("Nombre del archivo ingresado: %s\n", job);
      make_data(filename, job);
    } else {
      printf("Nombre del archivo no encontrado o incorrecto\n");
    }
  return 0;
}
