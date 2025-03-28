// Copyright 2024 Isaias Alfaro Ugalde

#include <matrix.h>

/**
 * @brief mutex para acceder sin condicion de carrera a los datos.
 */
pthread_mutex_t balance_mutex;
/**
 * @brief condicion para igual los hilos.
 */
pthread_cond_t balance_cond;

void make_data(const char *filename, char job[], int hilos) {
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
  make_matrix(plate_data, dat, &cont_array, job, hilos);
}

void make_matrix(struct data_job *plate_data, struct data_array *dat,
    int *cont_array, char job[], int hilos) {
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
    make_heat(plate_data, heat_data, hilos);
    /*for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        printf("%f " , heat_data[i][j].current_warm);
      }
      printf("\n");
    }*/
    jobs_out_file(dat, heat_data, plate_data, job_file, i);
    liberar_matrix(heat_data, rows);
  }
  jobs_close_file(job_file);
  make_free(plate_data, dat);
}


void make_heat(struct data_job *plate_data, struct heat **heat_dat, int hilos) {
    if (plate_data->R <= 1 || plate_data->C <= 1) {
        printf("Error: dimensiones de la matriz no válidas.\n");
        return;
    }
    if (hilos <= 0) {
        printf("Error: el número de hilos debe ser mayor que 0.\n");
        return;
    }

    // Inicializar mutex y condición
    pthread_mutex_init(&balance_mutex, NULL);
    pthread_cond_init(&balance_cond, NULL);

    // Asignar memoria para los hilos
    pthread_t* threads = malloc(hilos * sizeof(pthread_t));
    if (threads == NULL) {
        perror("Error al asignar memoria para threads");
        pthread_mutex_destroy(&balance_mutex);
        pthread_cond_destroy(&balance_cond);
        return;
    }

    // Asignar memoria para los argumentos de los hilos
    struct hilo_data *hilo_args = malloc(hilos * sizeof(struct hilo_data));
    if (hilo_args == NULL) {
        perror("Error al asignar memoria para hilo_args");
        free(threads);
        pthread_mutex_destroy(&balance_mutex);
        pthread_cond_destroy(&balance_cond);
        return;
    }

    double burn = (plate_data->time * plate_data->material) /
        (plate_data->area * plate_data->area);

    // Crear los hilos
    for (int i = 0; i < hilos; i++) {
        hilo_args[i].plate_data = plate_data;
        hilo_args[i].heat_dat = heat_dat;
        hilo_args[i].hilo_id = i;
        hilo_args[i].total_hilos = hilos;
        hilo_args[i].burn = burn;
        int err = pthread_create(&threads[i], NULL,
            make_heat_hilos, (void*)&hilo_args[i]);
        if (err != 0) {
            printf("Error al crear el hilo %d: %s\n", i, strerror(err));
            free(hilo_args);
            free(threads);
            pthread_mutex_destroy(&balance_mutex);
            pthread_cond_destroy(&balance_cond);
            return;
        }
    }

    // Unir los hilos
    for (int i = 0; i < hilos; i++) {
        int err = pthread_join(threads[i], NULL);
        if (err != 0) {
            printf("Error al unir el hilo %d: %s\n", i, strerror(err));
        }
    }

    // Destruir mutex y condición
    pthread_cond_destroy(&balance_cond);
    pthread_mutex_destroy(&balance_mutex);

    // Liberar memoria
    free(hilo_args);
    free(threads);
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

void* make_heat_hilos(void* arg) {
    struct hilo_data* data = (struct hilo_data*) arg;
    uint64_t cols_por_hilo = (data->plate_data->C -2) / data->total_hilos;
    uint64_t inicio_columna = (data->hilo_id * data->plate_data->C -1)
        / (data->plate_data->C -2);
    uint64_t inicio_fila = data->hilo_id;
    if (data->hilo_id == 0) {
      inicio_columna = 1;
      inicio_fila = 1;
    } else if (data->hilo_id == data->total_hilos - 1) {
      inicio_columna = data->plate_data->C -2 % data->total_hilos;
      inicio_fila = data->plate_data->R - 1;
      cols_por_hilo = data->plate_data->C -2 % data->total_hilos;
    }
    double auxiliar, rest_heat;
    uint64_t auxiliar_columnas = inicio_columna;
    uint64_t auxiliar_filas = inicio_fila;
    while (data->plate_data->balance != 1) {
      pthread_mutex_lock(&balance_mutex);
      data->plate_data->balance = 1;
      pthread_cond_broadcast(&balance_cond);
      pthread_mutex_unlock(&balance_mutex);

      // Actualizar current heat para el rango de columnas asignado
      for (uint64_t i = 0; i < cols_por_hilo; i++) {
        if (auxiliar_filas > data->plate_data->C - 1) {
          auxiliar_columnas = 1;
          auxiliar_filas = auxiliar_filas +1;
        }
        auxiliar = data->burn * (data->heat_dat[auxiliar_filas]
            [auxiliar_columnas +1 ].past_warm + data->heat_dat[auxiliar_filas]
            [auxiliar_columnas - 1].past_warm + data->heat_dat
            [auxiliar_filas + data->plate_data->C]
            [auxiliar_columnas].past_warm + data->heat_dat[auxiliar_filas -
            data->plate_data->C][auxiliar_columnas].past_warm - (4 *
            data->heat_dat[auxiliar_filas][auxiliar_columnas].past_warm));
        data->heat_dat[auxiliar_filas][auxiliar_columnas].current_warm
            += auxiliar;
        rest_heat =
            data->heat_dat[auxiliar_filas][auxiliar_columnas].current_warm -
            data->heat_dat[auxiliar_filas][auxiliar_columnas].past_warm;
        if (data->plate_data->epsilon < rest_heat) {
          pthread_mutex_lock(&balance_mutex);
          data->plate_data->balance = 0;
          pthread_cond_broadcast(&balance_cond);
          pthread_mutex_unlock(&balance_mutex);
        }
        auxiliar_columnas = auxiliar_columnas + 1;
      }
      pthread_mutex_lock(&balance_mutex);
      while (data->plate_data->balance == 0) {
        pthread_cond_wait(&balance_cond, &balance_mutex);
      }
      pthread_mutex_unlock(&balance_mutex);
      // printf("Columnas: %ld, Hilo ID: %d\n", auxiliar_columnas, data->hilo_id);
      // printf("Filas %ld\n, Hilo ID: %d\n", auxiliar_filas, data->hilo_id);
      // Actualizar past heat si balance es 0
      if (data->plate_data->balance == 0) {
        for (uint64_t i = 0; i < cols_por_hilo; i++) {
          if (auxiliar_filas > data->plate_data->C - 1) {
            auxiliar_columnas = 1;
            auxiliar_filas = auxiliar_filas +1;
          }
          data->heat_dat[auxiliar_filas][auxiliar_columnas].past_warm =
              data->heat_dat[auxiliar_filas][auxiliar_columnas].current_warm;
        }
        auxiliar_columnas = auxiliar_columnas + 1;
      }
      // Incrementar el ciclo
      if (data->hilo_id == 0) {
        data->plate_data->report += 1;
      }
    }
  return NULL;
}
