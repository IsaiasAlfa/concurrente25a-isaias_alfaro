// Copyright 2025 Isaias Alfaro Ugalde

#include <plate.h>

void make_data(const char *filename, char job[], uint64_t thread_count) {
  data_job_t* data_job = (data_job_t*) calloc(1, sizeof(data_job_t));
  data_array_t* data_array = calloc(40, sizeof(struct data_array));
  shared_data_t* shared_data =
    (shared_data_t*) calloc(1, sizeof(shared_data_t));

  int cont_array = 0;
  if (data_job == NULL || data_array == NULL || shared_data == NULL) {
    // Manejar errores de asignación de memoria
    fprintf(stderr, "Error al asignar memoria\n");
    free(data_job);
    free(data_array);
    free(shared_data);
    return;
  }

  shared_data->data_job = data_job;
  shared_data->thread_count = thread_count;

  jobs_find_file(filename, data_array, &cont_array);
  make_matrix(shared_data, data_array, &cont_array, job);
}

void make_matrix(shared_data_t* shared_data, data_array_t* data_array,
    int* cont_array, char job[]) {
  data_job_t* data_job = shared_data->data_job;
  int position = *cont_array;
  char job_filename[104];
  mkdir("out", 0777);
  snprintf(job_filename, sizeof(job_filename), "out/%s.tsv", job);
  FILE *job_file = fopen(job_filename, "w");
  uint64_t threads_max = shared_data->thread_count;
  for (int i = 0; i < position; i++) {
    plate_charge_RC(data_job, i, data_array);
    int rows = data_job->R;
    int cols = data_job->C;
    heat_t** heat = matrix(rows, cols);

    if (heat == NULL) {
      printf("Error al asignar memoria");
      return;
    }
    shared_data->heat = heat;
    plate_charge_heat(i, data_array, heat);
    shared_data->thread_count = threads_max;

    if (shared_data->thread_count > data_job->R) {
      shared_data->thread_count = data_job->R;
    }

    heat_team(shared_data);
    jobs_out_file(data_array, heat, data_job, job_file, i);
    free_matrix(heat, rows);
  }
  jobs_close_file(job_file);
  make_free(data_job, data_array, shared_data);
}

void heat_team(shared_data_t* shared_data) {
  data_job_t* data_job = shared_data->data_job;
  heat_t** heat = shared_data->heat;

  pthread_t* threads = (pthread_t*)
    malloc(shared_data->thread_count * sizeof(pthread_t));

  private_data_t* private_data = (private_data_t*)
    calloc(shared_data->thread_count, sizeof(private_data_t));

  double cycles = 0;
  double rest_heat = 0;
  data_job->balance = 0;  // Inicializamos balance a 0 (no equilibrado)

  double burn = (data_job->time * data_job->material) /
  (data_job->area * data_job->area);
  data_job->burn = burn;

  if (data_job->R <= 1 || data_job->C <= 1) {
    printf("Error: dimensiones de la matriz no válidas.\n");
    return;
  }

  while (data_job->balance != 1) {
    data_job->balance = 1;

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
      ; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].shared_data = shared_data;

      pthread_create(&threads[thread_number], /*attr*/ NULL, make_heat
        , /*arg*/ &private_data[thread_number]);
    }

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
      ; ++thread_number) {
    pthread_join(threads[thread_number], /*value_ptr*/ NULL);
    }

    for (uint64_t i = 1; i < data_job->R - 1; i++) {
      for (uint64_t j = 1; j < data_job->C - 1; j++) {
        // resta del calor para epsilon
        rest_heat = heat[i][j].current_warm - heat[i][j].past_warm;

        // Verificar si el sistema aún no está equilibrado
        if (fabs(rest_heat) > data_job->epsilon) {
          data_job->balance = 0;  // El sistema no está equilibrado, continuar
        }
          heat[i][j].past_warm = heat[i][j].current_warm;
      }
    }
    cycles++;
  }
  data_job->report = cycles;
  free(threads);
  free(private_data);
}

void* make_heat(void* data) {
  private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = private_data->shared_data;
  heat_t** heat = shared_data->heat;
  data_job_t* data_job = shared_data->data_job;

  double auxiliar = 0;

  size_t thread_number = private_data->thread_number;
  size_t thread_count = shared_data->thread_count;

  uint64_t rows_per_thread = data_job->R / thread_count;
  uint64_t remainder = data_job->R % thread_count;

  uint64_t start = thread_number * rows_per_thread +
    (thread_number < remainder ? thread_number : remainder);
  uint64_t end = start + rows_per_thread + (thread_number < remainder ? 1 : 0);

  if (start == 0) start = 1;
  if (end >= data_job->R) end = data_job->R - 1;

  // Iterar sobre las celdas de la matriz, excepto las fronteras
  for (uint64_t i = start; i < end; i++) {
    for (uint64_t j = 1; j < data_job->C - 1; j++) {
      // Calcular el cambio en el calor
      auxiliar = data_job->burn * (heat[i-1][j].past_warm +
        heat[i][j-1].past_warm +
        heat[i][j+1].past_warm +
        heat[i+1][j].past_warm - (4 * heat[i][j].past_warm));
        heat[i][j].current_warm = heat[i][j].current_warm + auxiliar;
    }
  }
  return NULL;
}

void make_free(data_job_t* data_job, data_array_t* data_array,
  shared_data_t* shared_data) {
  free(shared_data);
  free(data_job);
  free(data_array);
}

heat_t** matrix(int filas, int columnas) {
  heat_t** heat = calloc(filas, sizeof(heat_t*));
  if (heat == NULL) {
      perror("No se pudo asignar memoria para los punteros a las filas");
      exit(EXIT_FAILURE);
  }
  for (int i = 0; i < filas; i++) {
    heat[i] = malloc(columnas * sizeof(struct heat));
    if (heat[i] == NULL) {
        perror("No se pudo asignar memoria para una fila");
        // Liberar memoria ya asignada
        free_matrix(heat, i);
        exit(EXIT_FAILURE);
      }
  }
  return heat;
}

void free_matrix(heat_t** heat, int filas) {
  for (int i = 0; i < filas; i++) {
    free(heat[i]);
}
free(heat);
}

int analyze_arguments(char* filename, size_t filename_size, char* job,
    size_t job_size, uint64_t* thread_count) {
  char *start, *end;
  char buffer[100];

  // Leer nombre del archivo
  printf("Ingresa el nombre del archivo: ");
  if (fgets(filename, filename_size, stdin) == NULL) {
      printf("Error al leer el nombre del archivo.\n");
      return 1;
  }
  filename[strcspn(filename, "\n")] = '\0';

  // Extraer nombre base sin extensión
  start = strrchr(filename, '/');
  start = (start != NULL) ? start + 1 : filename;
  end = strchr(start, '.');

  size_t length = (end != NULL) ? (size_t)(end - start) : strlen(start);
  if (length >= job_size) {
      printf("Error: el nombre del archivo es demasiado largo.\n");
      return 1;
  }
  snprintf(job, job_size, "%.*s", (int)length, start);

  // Validar que contiene "job"
  if (strstr(job, "job") != NULL) {
      printf("Nombre del archivo ingresado: %s\n", job);
  } else {
      printf("Nombre del archivo no encontrado o incorrecto.\n");
      return 1;
  }

  // Leer cantidad de hilos
  printf("Ingrese la cantidad de hilos: ");
  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      printf("Error al leer la cantidad de hilos.\n");
      return 1;
  }
  *thread_count = strtoull(buffer, NULL, 10);
  if (*thread_count == 0) {
      printf("Cantidad de hilos inválida.\n");
      return 1;
  }

  return 0;
}
