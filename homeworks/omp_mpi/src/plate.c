// Copyright 2025 Isaias Alfaro Ugalde

#include "plate.h"

void make_data(const char *filename, char job[], uint64_t thread_count) {
  // rango y mundo de MPI
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int world = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world);

  if (rank == 0) {
    // crear carpeta out
    mkdir("out", 0777);
  }

  // barrera para sincronizar todos los procesos
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    // escuchar respuestas
    jobs_final_file(filename, job);
  } else {
// memoria para las distintas estructuras
  data_job_t* data_job = (data_job_t*) calloc(1, sizeof(data_job_t));
  data_array_t* data_array = calloc(40, sizeof(struct data_array));

  int cont_array = 0;
  // error al asignar memoria
  if (data_job == NULL || data_array == NULL) {
    // Manejar errores de asignación de memoria
    fprintf(stderr, "Error al asignar memoria\n");
    free(data_job);
    free(data_array);
    return;
  }
    data_job->thread_count = thread_count;
    // buscar el archivo inicial y cargar todos sus datos
    jobs_find_file(filename, data_array, &cont_array);
    // creacion de las distintas simulaciones
    make_matrix(data_job, data_array, &cont_array);
  }
}

void make_matrix(data_job_t* data_job, data_array_t* data_array,
    int* cont_array) {

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  rank = rank - 1;  // Ajustar el rango para que comience desde 0

  int world = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world);
  world = world - 1;

  int overall_start = 0;
  int overall_finish = *cont_array;

  // variable para guardar hilos maximos del usuario
  uint64_t threads_max = data_job->thread_count;

  // Calculo para ver el principio y final de cada proceso
  const int process_start = calculate_start(rank,
    overall_finish, world, overall_start);
  const int process_finish = calculate_finish(rank,
    overall_finish, world, overall_start);

  // for para todas las veces que se ocupan hacer distintas simulaciones
  for (int i = process_start; i < process_finish; i++) {
    // cargar datos inciales
    plate_charge_RC(data_job, i, data_array);
    int rows = data_job->R;
    int cols = data_job->C;
    // creacion de la estructura de datos
    matrix(rows, cols, data_job);
    // cargar datos de calor
    plate_charge_heat(i, data_array, data_job);

    // mas hilos que filas
    if (data_job->thread_count > data_job->R) {
      data_job->thread_count = data_job->R;
    }

    if (data_job->C > 500 && data_job->R > 500
      && data_job->thread_count > 1) {
      // equipo de hilos
      heat_team(data_job);
    } else {
      heat_serial(data_job);
    }
    // enviar el resultado al proceso 0
    jobs_out_file(data_array, data_job, i);
    // liberar la matriz
    free_matrix(data_job);
    data_job->thread_count = threads_max;
  }
  // liberar la memoria
  make_free(data_job, data_array);
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

void heat_team(data_job_t* data_job) {
  data_job->report = 0;

  // constante en la ejecucion de calor
  double burn = (data_job->time * data_job->material) /
  (data_job->area * data_job->area);
  data_job->burn = burn;

  if (data_job->R <= 1 || data_job->C <= 1) {
    printf("Error: dimensiones de la matriz no válidas.\n");
    return;
  }

  data_job->balance = 0;
  uint64_t thread_count = data_job->thread_count;

  // Inicio de la zona paralela
  #pragma omp parallel num_threads(thread_count) \
  default(none) shared(data_job) firstprivate(thread_count)
  {  // NOLINT(whitespace/braces)
    make_heat(data_job);  // NOLINT(readability/casting)
  }
}

void make_heat(data_job_t* data_job) {
  // auxiliar para ver comprobar el balance
  double auxiliar = 0;

  // While para el balance
  while (data_job->balance != 1) {
    #pragma omp barrier
    #pragma omp single
    {
      data_job->balance = 1;
    }
    // Iterar sobre las celdas de la matriz, excepto las fronteras
    #pragma omp for schedule(runtime)
    for (uint64_t i = 1; i < data_job->R - 1; i++) {
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

        // Verificar si el calor restante es mayor que epsilon
        if (fabs(rest_heat) > data_job->epsilon) {
          #pragma omp critical
          {
            data_job->balance = 0;  // No está equilibrado
          }
        }
      }
    }

    // Barrera para intercambio de buffers
    #pragma omp barrier
    #pragma omp single
    {
      // Intercambiar buffers
      double *ptr_auxiliar = data_job->past_warm;
      data_job->past_warm = data_job->current_warm;
      data_job->current_warm = ptr_auxiliar;

      data_job->report = data_job->report + 1;
    }
  }
}

void make_free(data_job_t* data_job, data_array_t* data_array) {
  // liberar la memoria de los estructs principales
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
  // liberar la memoria
  free(data_job->past_warm);
  free(data_job->current_warm);
}

int analyze_arguments(char* filename, size_t filename_size, char* job,
    size_t job_size, uint64_t* thread_count, int* argc, char*** argv) {
  // ver si dieron suficientes argumentos
  if (*argc < 3) {
    fprintf(stderr, "Uso: %s <archivo_tests> <cantidad_hilos>\n", (*argv)[0]);
    return -1;
  }

  // Copiar el nombre del archivo
  strncpy(filename, (*argv)[1], filename_size - 1);
  filename[filename_size - 1] = '\0';

  // Extraer el nombre del trabajo sin extensión ni ruta
  const char *slash = strrchr(filename, '/');
  const char *basename = slash ? slash + 1 : filename;
  const char *dot = strrchr(basename, '.');
  size_t len = dot ? (size_t)(dot - basename) : strlen(basename);
  if (len >= job_size) len = job_size - 1;
  strncpy(job, basename, len);
  job[len] = '\0';

  // Leer la cantidad de hilos
  char *endptr = NULL;
  uint64_t threads = strtoull((*argv)[2], &endptr, 10);
  if (endptr == (*argv)[2] || threads == 0) {
    fprintf(stderr, "Cantidad de hilos inválida: %s\n", (*argv)[2]);
    return -1;
  }
  *thread_count = threads;

  return 0;
}

int calculate_start(int rank, int end, int workers, int begin) {
  // Calcular elrango de inicio
  int range = end - begin;
  int min_val = (rank < (range % workers)) ? rank : (range % workers);
  return begin + rank * (range / workers) + min_val;
}

int calculate_finish(int rank, int end, int workers, int begin) {
  // Calcular el rango final
  return calculate_start(rank + 1, end, workers, begin);
}

