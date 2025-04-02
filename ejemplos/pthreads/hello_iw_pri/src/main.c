// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// se parece a una mini clase de C++
// thread_private_data_t
// typedef remplaza simbolo "variable" por la definicion
typedef struct private_data {
  uint64_t thread_number;  // rank, numero del hilo
  uint64_t thread_count;  // cantidad de hilos
  struct private_data* next;  // siguiente nodo de cola
} private_data_t;


/**
 * @brief Función que ejecuta un saludo en un hilo.
 * @param data Puntero a los datos que se pasarán al hilo.
 * @return Puntero a los datos resultantes de la ejecución.
 */
void* greet(void* data);

/**
 * @brief Crea un número específico de hilos.
 * @param thread_count Cantidad de hilos a crear.
 * @return 0 si la creación fue exitosa, otro valor en caso de error.
 */
int create_threads(uint64_t thread_count);

// procedure main(argc, argv[])
// error return numbers in specific to catch. "No se ven bien"
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  // create thread_count as result of converting argv[1] to integer
  // thread_count := integer(argv[1])
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) == 1) {
    } else {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;  // primer error rutina 1
    }
  }

  error = create_threads(thread_count);
  return error;
}  // end procedure

// subrutina que crea los hilos, mudilizar el main
int create_threads(uint64_t thread_count) {
  int error = EXIT_SUCCESS;
  // for thread_number := 0 to thread_count do
  // malloc solo reserva la memoria "deja la basura"
  pthread_t* threads = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
  // arreglo de la cantidad de hilos y tamaño del private data
  // calloc inicializa la memoria en 0, mas lento
  private_data_t* private_data = (private_data_t*)
    calloc(thread_count, sizeof(private_data_t));
  if (threads && private_data) {
    // paralelismo de datos incremento de desempeño
    for (uint64_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].thread_count = thread_count;
      // create_thread(greet, thread_number)
      // Ahora se envia la memoria privada del hilo
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, greet
        , /*arg*/ &private_data[thread_number]);
      if (error == EXIT_SUCCESS) {
      } else {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;  // primer error rutina 2
        break;
      }
    }

    // print "Hello from main thread"
    printf("Hello from main thread\n");

    for (uint64_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n"
      , thread_count);
    error = 22;  // segundo error rutina 2
  }

  return error;
}

// procedure greet:
void* greet(void* data) {
  // assert(data);
  // direccion de memoria privada
  private_data_t* private_data = (private_data_t*) data;
  // print "Hello from secondary thread"
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n"
    , (*private_data).thread_number, private_data->thread_count);
  return NULL;
}  // end procedure
