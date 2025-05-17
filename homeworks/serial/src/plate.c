// Copyright 2025 Isaias Alfaro Ugalde

#include "plate.h"

void make_data(const char *filename, char job[]) {
  struct data_job *plate_data = calloc(sizeof(struct data_job), sizeof(double));
  struct data_array *dat = calloc(sizeof(struct data_array) * 40,
    sizeof(double));

  int cont_array = 0;
  if (plate_data == NULL || dat == NULL) {
    // Manejar errores de asignación de memoria
    fprintf(stderr, "Error al asignar memoria\n");
    free(plate_data);
    free(dat);
    return;
  }

  jobs_find_file(filename, dat, &cont_array);
  make_matrix(plate_data, dat, &cont_array, job);
}

void make_matrix(struct data_job *plate_data, struct data_array *dat,
    int * cont_array, char job[]) {
  int position = *cont_array;
  char job_filename[104];
  mkdir("out", 0777);
  snprintf(job_filename, sizeof(job_filename), "out/%s.tsv", job);
  FILE *job_file = fopen(job_filename, "w");
  for (int i = 0; i < position; i++) {
    plate_charge_RC(plate_data, i, dat);
    int rows = plate_data->R;
    int cols = plate_data->C;
    matrix(rows, cols, plate_data);
    plate_charge_heat(i, dat, plate_data);
    make_heat(plate_data);
    jobs_out_file(dat, plate_data, job_file, i);
    free_matrix(plate_data);
  }
  jobs_close_file(job_file);
  make_free(plate_data, dat);
}

void make_heat(struct data_job *plate_data) {
  double cycles = 0;
  double auxiliar = 0;
  double rest_heat = 0;
  plate_data->balance = 0;  // Inicializamos balance a 0 (no equilibrado)

  double *current_warm = plate_data->current_warm;
  double *past_warm = plate_data->past_warm;

  double burn = (plate_data->time * plate_data->material) /
      (plate_data->area * plate_data->area);

  if (plate_data->R <= 1 || plate_data->C <= 1) {
    printf("Error: dimensiones de la matriz no válidas.\n");
    return;
  }

while (plate_data->balance != 1) {
    plate_data->balance = 1;

    // Iterar sobre las celdas de la matriz, excepto las fronteras
    for (uint64_t i = 1; i < plate_data->R - 1; i++) {
        for (uint64_t j = 1; j < plate_data->C - 1; j++) {
            uint64_t idx = i * plate_data->C + j;
            uint64_t up = (i - 1) * plate_data->C + j;
            uint64_t down = (i + 1) * plate_data->C + j;
            uint64_t left = i * plate_data->C + (j - 1);
            uint64_t right = i * plate_data->C + (j + 1);

            auxiliar = burn * (
                past_warm[up] +
                past_warm[left] +
                past_warm[right] +
                past_warm[down] -
                4 * past_warm[idx]);

            current_warm[idx] = past_warm[idx] + auxiliar;

            rest_heat = current_warm[idx] - past_warm[idx];

            if (fabs(rest_heat) > plate_data->epsilon) {
                plate_data->balance = 0;
            }
        }
    }

    // Intercambiar buffers
    double *ptr_auxiliar = past_warm;
    past_warm = current_warm;
    current_warm = ptr_auxiliar;

    cycles++;  // Incrementar el contador de ciclos
}

plate_data->report = cycles;
}

void make_free(struct data_job *plate_data, struct data_array *dat) {
  free(plate_data);
  free(dat);
}

void matrix(int filas, int columnas, struct data_job *plate_data) {
  // array
  double* past_warm = calloc(filas*columnas, sizeof(double));
  double* current_warm = calloc(filas*columnas, sizeof(double));
  plate_data->current_warm = current_warm;
  plate_data->past_warm = past_warm;
}

void free_matrix(struct data_job *plate_data) {
  free(plate_data->past_warm);
  free(plate_data->current_warm);
}
