// Copyright 2025 Isaias Alfaro Ugalde

#include "plate.h"

void make_data(const char *filename, char job[], uint64_t thread_count) {
  // memoria para las distintas estructuras
  data_job_t* data_job = (data_job_t*) calloc(1, sizeof(data_job_t));
  data_array_t* data_array = calloc(40, sizeof(struct data_array));
  shared_data_t* shared_data =
    (shared_data_t*) calloc(1, sizeof(shared_data_t));

  int cont_array = 0;
  // erro al asginar memoria
  if (data_job == NULL || data_array == NULL || shared_data == NULL) {
    // Manejar errores de asignación de memoria
    fprintf(stderr, "Error al asignar memoria\n");
    free(data_job);
    free(data_array);
    free(shared_data);
    return;
  }

  // incializar valores de shared_data
  shared_data->data_job = data_job;
  shared_data->thread_count = thread_count;

  // buscar el archivo inicial y cargar todos sus datos
  jobs_find_file(filename, data_array, &cont_array);
  // creacion de las distintas simulaciones
  make_matrix(shared_data, data_array, &cont_array, job);
}

void make_matrix(shared_data_t* shared_data, data_array_t* data_array,
    int* cont_array, char job[]) {
  // casteo de las estructuras para un manejo mas sencillo
  data_job_t* data_job = shared_data->data_job;

  int position = *cont_array;
  char job_filename[104];

  // crear carpeta out
  mkdir("out", 0777);
  // crear archivo de salida
  snprintf(job_filename, sizeof(job_filename), "out/%s.tsv", job);
  FILE *job_file = fopen(job_filename, "w");

  // variable para guardar hilos maximos del usuario
  uint64_t threads_max = shared_data->thread_count;
  // for para todas las veces que se ocupan hacer distintas simulaciones
  for (int i = 0; i < position; i++) {
    // cargar datos inciales
    plate_charge_RC(data_job, i, data_array);
    int rows = data_job->R;
    int cols = data_job->C;
    // creacion de la estructura de datos
    matrix(rows, cols, data_job);
    // cargar datos de calor
    plate_charge_heat(i, data_array, data_job);
    // guardar cantiad maxima de hilos que pidio el usuario
    shared_data->thread_count = threads_max;

    // mas hilos que filas
    if (shared_data->thread_count > data_job->R) {
      shared_data->thread_count = data_job->R;
    }

    if (data_job->C > 500 && data_job->R > 500
      && shared_data->thread_count > 1) {
      // equipo de hilos
      heat_team(shared_data);
    } else {
      heat_serial(data_job);
    }
    // escribir el resultado
    jobs_out_file(data_array, data_job, job_file, i);
    // liberar la matriz
    free_matrix(data_job);
  }
  // cerrar los archivos
  jobs_close_file(job_file);
  // liberar la memoria
  make_free(data_job, data_array, shared_data);
}

void heat_serial(data_job_t* data_job) {
  double cycles = 0;
  double auxiliar = 0;
  double rest_heat = 0;
  data_job->balance = 0;  // Inicializamos balance a 0 (no equilibrado)

  double *current_warm = data_job->current_warm;
  double *past_warm = data_job->past_warm;

  double burn = (data_job->time * data_job->material) /
    (data_job->area * data_job->area);

  if (data_job->R <= 1 || data_job->C <= 1) {
    printf("Error: dimensiones de la matriz no válidas.\n");
    return;
  }

  while (data_job->balance != 1) {
    data_job->balance = 1;

    // Iterar sobre las celdas de la matriz, excepto las fronteras
    for (uint64_t i = 1; i < data_job->R - 1; i++) {
      for (uint64_t j = 1; j < data_job->C - 1; j++) {
        uint64_t idx = i * data_job->C + j;
        uint64_t up = (i - 1) * data_job->C + j;
        uint64_t down = (i + 1) * data_job->C + j;
        uint64_t left = i * data_job->C + (j - 1);
        uint64_t right = i * data_job->C + (j + 1);

        auxiliar = burn * (
          past_warm[up] +
          past_warm[left] +
          past_warm[right] +
          past_warm[down] -
          4 * past_warm[idx]);

        current_warm[idx] = past_warm[idx] + auxiliar;

        rest_heat = current_warm[idx] - past_warm[idx];

        if (fabs(rest_heat) > data_job->epsilon) {
          data_job->balance = 0;
        }
      }
    }

    // Intercambiar buffers
    double *ptr_auxiliar = past_warm;
    past_warm = current_warm;
    current_warm = ptr_auxiliar;

    cycles++;  // Incrementar el contador de ciclos
  }
  data_job->report = cycles;
}


void heat_team(shared_data_t* shared_data) {
  // casteo de las estructuras para un manejo mas sencillo
  data_job_t* data_job = shared_data->data_job;

  data_job->report = 0;
  // creacion del equipo de hilos(array)
  pthread_t* threads = (pthread_t*)
    calloc(shared_data->thread_count, sizeof(pthread_t));

  // memoria privada de cada hilo
  private_data_t* private_data = (private_data_t*)
    calloc(shared_data->thread_count, sizeof(private_data_t));

  sem_init(&shared_data->mutex_balance, 0, 1);
  sem_init(&shared_data->can_acces_count, 0, 1);
  sem_init(&shared_data->can_acces_count2, 0, 1);
  sem_init(&shared_data->barrier_exchange, 0, 0);
  sem_init(&shared_data->barrier_work, 0, 0);


  // constante en la ejecucion de calor
  double burn = (data_job->time * data_job->material) /
  (data_job->area * data_job->area);
  data_job->burn = burn;

  if (data_job->R <= 1 || data_job->C <= 1) {
    printf("Error: dimensiones de la matriz no válidas.\n");
    return;
  }

  shared_data->balance = 0;

  for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
    ; ++thread_number) {
    // asignacion de la memoria privada de cada hilo
    private_data[thread_number].thread_number = thread_number;
    private_data[thread_number].shared_data = shared_data;

    // creacion del equipo de hilos y guardarlos en el array de trabajo
    pthread_create(&threads[thread_number], /*attr*/ NULL, make_heat
      , /*arg*/ &private_data[thread_number]);
  }

  for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
    ; ++thread_number) {
    pthread_join(threads[thread_number], /*value_ptr*/ NULL);
  }
  // liberar la memoriaque se utilizo para el equipo de hilos
  sem_destroy(&shared_data->barrier_work);
  sem_destroy(&shared_data->barrier_exchange);
  free(private_data);
  free(threads);
}

void* make_heat(void* data) {
  // casteo de las estructuras para un manejo mas sencillo
  private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = private_data->shared_data;
  data_job_t* data_job = shared_data->data_job;

  // auxiliar para ver comprobar el balance
  double auxiliar = 0;

  // numero propio de hilo y cantidad totales de hilos
  size_t thread_number = private_data->thread_number;
  size_t thread_count = shared_data->thread_count;

  // filas que le tocan a cada hilo
  uint64_t rows_per_thread = data_job->R / thread_count;
  // filas sobrantes por si no es divisible exacto
  uint64_t remainder = data_job->R % thread_count;

  // inicio de cada hilo en la matriz
  uint64_t start = thread_number * rows_per_thread +
    (thread_number < remainder ? thread_number : remainder);
  // fin de cada hilo en la matriz
  uint64_t end = start + rows_per_thread + (thread_number < remainder ? 1 : 0);

  // caso especial primer hilo
  if (start == 0) start = 1;
  // caso especial ultimo hilo
  if (end >= data_job->R) end = data_job->R - 1;

  // Iterar sobre las celdas de la matriz, excepto las fronteras
  while (shared_data->balance != 1) {
    sem_wait(&shared_data->can_acces_count);
      shared_data->count = shared_data->count + 1;
      if (shared_data->count == shared_data->thread_count) {
        shared_data->balance = 1;
        shared_data->count = 0;
        for (u_int64_t i = 0; i < shared_data->thread_count; i++) {
          sem_post(&shared_data->barrier_work);
        }
      }
    sem_post(&shared_data->can_acces_count);
    sem_wait(&shared_data->barrier_work);

    // Iterar sobre las celdas de la matriz, excepto las fronteras
    for (uint64_t i = start; i < end; i++) {
      for (uint64_t j = 1; j < data_job->C - 1; j++) {
        uint64_t idx = i * data_job->C + j;
        uint64_t up = (i - 1) * data_job->C + j;
        uint64_t down = (i + 1) * data_job->C + j;
        uint64_t left = i * data_job->C + (j - 1);
        uint64_t right = i * data_job->C + (j + 1);

        auxiliar = data_job->burn * (
          data_job->past_warm[up] +
          data_job->past_warm[left] +
          data_job->past_warm[right] +
          data_job->past_warm[down] -
          4 * data_job->past_warm[idx]);

        data_job->current_warm[idx] = data_job->past_warm[idx] + auxiliar;

        double rest_heat = data_job->current_warm[idx]
          - data_job->past_warm[idx];

        if (fabs(rest_heat) > data_job->epsilon) {
          sem_wait(&shared_data->mutex_balance);
            if (shared_data->balance != 0) {
              shared_data->balance = 0;
            }
          sem_post(&shared_data->mutex_balance);
        }
      }
    }

    sem_wait(&shared_data->can_acces_count2);
      shared_data->count2 = shared_data->count2 + 1;
      if (shared_data->count2 == shared_data->thread_count) {
        shared_data->count2 = 0;
        // Intercambiar buffers
        double *ptr_auxiliar = data_job->past_warm;
        data_job->past_warm = data_job->current_warm;
        data_job->current_warm = ptr_auxiliar;

        data_job->report = data_job->report + 1;
        for (u_int64_t i = 0; i < shared_data->thread_count; i++) {
          sem_post(&shared_data->barrier_exchange);
        }
      }
    sem_post(&shared_data->can_acces_count2);
    sem_wait(&shared_data->barrier_exchange);
  }
  return NULL;
}

void make_free(data_job_t* data_job, data_array_t* data_array,
  shared_data_t* shared_data) {
  // liberar la memoria de los estructs principales
  free(shared_data);
  free(data_job);
  free(data_array);
}

void matrix(int filas, int columnas, data_job_t* data_job) {
  // array
  double* past_warm = calloc(filas*columnas, sizeof(double));
  double* current_warm = calloc(filas*columnas, sizeof(double));
  data_job->current_warm = current_warm;
  data_job->past_warm = past_warm;
}

void free_matrix(data_job_t* data_job) {
  free(data_job->past_warm);
  free(data_job->current_warm);
}

int analyze_arguments(char* filename, size_t filename_size, char* job,
    size_t job_size, uint64_t* thread_count) {
  // variables para extrae el nombre base
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
