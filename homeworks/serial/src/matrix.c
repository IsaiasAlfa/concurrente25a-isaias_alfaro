// Copyright 2024 Isaias Alfaro Ugalde

#include <matrix.h>


void make_data(const char *filename, char job[]) {
  struct data_job *plate_data = malloc(sizeof(struct data_job));
  struct data_array *dat = malloc(sizeof(struct data_array) * 20);

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
  snprintf(job_filename, sizeof(job_filename), "%s.tsv", job);
  FILE *job_file = fopen(job_filename, "w");
  for (int i = 0; i < position; i++) {
    plate_charge_RC(plate_data, i, dat);
    int rows = plate_data->R;
    int cols = plate_data->C;
    struct heat **heat_data = crear_matriz(rows, cols);
    if (heat_data == NULL) {
      printf("Error al asignar memoria");
      return;
    }
    plate_charge_heat(i, dat, heat_data);
    make_heat(plate_data, heat_data);
    jobs_out_file(dat, heat_data, plate_data, job_file, i);
    liberar_matrix(heat_data, rows);
  }
  jobs_close_file(job_file);
  make_free(plate_data, dat);
}

void make_heat(struct data_job *plate_data, struct heat **heat_dat) {
  double cycles = 0;
  double auxiliar = 0;
  double rest_heat = 0;
  plate_data->balance = 0;

  double burn = (plate_data->time * plate_data->material) /
      (plate_data->area * plate_data->area);

  if (plate_data->R <= 1 || plate_data->C <= 1) {
    printf("Error: dimensiones de la matriz no válidas.\n");
    return;
  }

  while (plate_data->balance != 1) {
    // actualizar current heat
    plate_data->balance = 1;
    for (uint64_t i = 1; i < plate_data->R - 1; i++) {
      for (uint64_t j = 1; j < plate_data->C - 1; j++) {
        auxiliar = burn * (heat_dat[i-1][j].past_warm +
            heat_dat[i][j-1].past_warm +
            heat_dat[i][j+1].past_warm +
            heat_dat[i+1][j].past_warm -
            (4 * heat_dat[i][j].past_warm));
        heat_dat[i][j].current_warm = heat_dat[i][j].current_warm + auxiliar;

        rest_heat = heat_dat[i][j].current_warm - heat_dat[i][j].past_warm;
        if (plate_data->epsilon < rest_heat) {
          plate_data->balance = 0;
        }
      }
    }
    // actualizar past heat
    if (plate_data->balance == 0) {
      for (uint64_t i = 1; i < plate_data->R - 1; i++) {
        for (uint64_t j = 1; j < plate_data->C - 1; j++) {
          heat_dat[i][j].past_warm = heat_dat[i][j].current_warm;
        }
      }
    }
  cycles = cycles + 1;
  }
  plate_data->report = cycles;
}

void make_free(struct data_job *plate_data, struct data_array *dat) {
  free(plate_data);
  free(dat);
}

struct heat **crear_matriz(int filas, int columnas) {
  struct heat **matriz = malloc(filas * sizeof(struct heat *));
  if (matriz == NULL) {
      perror("No se pudo asignar memoria para los punteros a las filas");
      exit(EXIT_FAILURE);
  }
  for (int i = 0; i < filas; i++) {
    matriz[i] = malloc(columnas * sizeof(struct heat));
    if (matriz[i] == NULL) {
        perror("No se pudo asignar memoria para una fila");
        // Liberar memoria ya asignada
        liberar_matrix(matriz, i);
        exit(EXIT_FAILURE);
      }
  }
  return matriz;
}

void liberar_matrix(struct heat **matrix, int filas) {
  for (int i = 0; i < filas; i++) {
    free(matrix[i]);
}
free(matrix);
}
