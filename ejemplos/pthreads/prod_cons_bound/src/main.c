// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// Simulates a producer and a consumer that share a bounded buffer

// @see `man feature_test_macros`
#define _DEFAULT_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <unistd.h>

enum {
  ERR_NOMEM_SHARED = EXIT_FAILURE + 1,
  ERR_NOMEM_BUFFER,
  ERR_NO_ARGS,
  ERR_BUFFER_CAPACITY,
  ERR_ROUND_COUNT,
  ERR_MIN_PROD_DELAY,
  ERR_MAX_PROD_DELAY,
  ERR_MIN_CONS_DELAY,
  ERR_MAX_CONS_DELAY,
  ERR_CREATE_THREAD,
};

// datos que los hilos van a ocupar
typedef struct {
  // cantidad de hilos
  size_t thread_count;
  // capacidad maxima del buffer
  size_t buffer_capacity;
  // puntero del buffer
  double* buffer;
  // cantidad de productos a generar
  size_t rounds;
  // tiempos de produccion
  useconds_t producer_min_delay;
  useconds_t producer_max_delay;
  useconds_t consumer_min_delay;
  useconds_t consumer_max_delay;

  // semaforos que se ocupan para controla prod_cons
  sem_t can_produce;
  sem_t can_consume;
} shared_data_t;

typedef struct {
  // numero de hilo
  size_t thread_number;
  // puntero a los datos compartidos
  shared_data_t* shared_data;
} private_data_t;

/**
 * @brief Metodo de analisis de argumentos dados en la linea de terminal
 * 
 * @param argc cantidad de argumentos dados
 * @param argv los argumentos brindados
 * @param shared_data estructura donde se van a guardar
 * @return int retorno para control de errores
 */
int analyze_arguments(int argc, char* argv[], shared_data_t* shared_data);

/**
 * @brief Creacion de equipos de hilos a usar
 * 
 * @param shared_data estructura de datos compartida que ocupa cada hilo
 * @return int retorno para control de errores 
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Metodo para la simulacion de produccion de objetos 
 * 
 * @param data puntero con los datos compartidos
 */
void* produce(void* data);

/**
 * @brief Metodo que simula la simulacion de consumir un objeto
 * 
 * @param data puntero con los datos compartidos
 */
void* consume(void* data);

/**
 * @brief Metodo para asignar un numero random para simulacion
 * 
 * @param min numero minimo que puede durar la simulacion
 * @param max numero maximo que puede durar la simulacion
 * @return useconds_t numero aleatorio entre min y max
 */
useconds_t random_between(useconds_t min, useconds_t max);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  // asigancion de memoria para el struct de shared_data
  shared_data_t* shared_data = (shared_data_t*)
    calloc(1, sizeof(shared_data_t));

  if (shared_data) {
    // analisis de argumentos
    error = analyze_arguments(argc, argv, shared_data);
    if (error == EXIT_SUCCESS) {
      // asignacion de memoria para el buffer
      shared_data->buffer = (double*)
        calloc(shared_data->buffer_capacity, sizeof(double));
      if (shared_data->buffer) {
        // se inicia el semaforo de produccion con el numero de buffer_capacity
        sem_init(&shared_data->can_produce, /*pshared*/ 0,
          shared_data->buffer_capacity);
        // se incia el semaforo de consumir en 0
        sem_init(&shared_data->can_consume, /*pshared*/ 0, /*value*/ 0);

        // semilla para los numero aleatorios de delay
        unsigned int seed = 0u;
        getrandom(&seed, sizeof(seed), GRND_NONBLOCK);
        srandom(seed);

        // forma de mostar el tiempo de ejecucion del programa
        struct timespec start_time;
        clock_gettime(/*clk_id*/CLOCK_MONOTONIC, &start_time);

        // creacion de los hilos
        error = create_threads(shared_data);

        // se detiene lo que duro el programa
        struct timespec finish_time;
        clock_gettime(/*clk_id*/CLOCK_MONOTONIC, &finish_time);

        // formato de tiempo y mostrarlo al usuario
        double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
          (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
        printf("execution time: %.9lfs\n", elapsed);

        // destruir ambos semaforos despues de su uso
        sem_destroy(&shared_data->can_consume);
        sem_destroy(&shared_data->can_produce);
        // liberar la memoria del buffer
        free(shared_data->buffer);
      } else {
        fprintf(stderr, "error: could not create buffer\n");
        error = ERR_NOMEM_BUFFER;
      }
    }

    // liber la memoria compartida
    free(shared_data);
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    error = ERR_NOMEM_SHARED;
  }

  return error;
}

int analyze_arguments(int argc, char* argv[], shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  if (argc == 7) {
    if (sscanf(argv[1], "%zu", &shared_data->buffer_capacity) != 1
      || shared_data->buffer_capacity == 0) {
        fprintf(stderr, "error: invalid buffer capacity\n");
        error = ERR_BUFFER_CAPACITY;
    } else if (sscanf(argv[2], "%zu", &shared_data->rounds) != 1
      || shared_data->rounds == 0) {
        fprintf(stderr, "error: invalid round count\n");
        error = ERR_ROUND_COUNT;
    } else if (sscanf(argv[3], "%u", &shared_data->producer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min producer delay\n");
        error = ERR_MIN_PROD_DELAY;
    } else if (sscanf(argv[4], "%u", &shared_data->producer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max producer delay\n");
        error = ERR_MAX_PROD_DELAY;
    } else if (sscanf(argv[5], "%u", &shared_data->consumer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min consumer delay\n");
        error = ERR_MIN_CONS_DELAY;
    } else if (sscanf(argv[6], "%u", &shared_data->consumer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max consumer delay\n");
        error = ERR_MAX_CONS_DELAY;
    }
  } else {
    fprintf(stderr, "usage: prod_cons_bound buffer_capacity rounds"
      " producer_min_delay producer_max_delay"
      " consumer_min_delay consumer_max_delay\n");
      error = ERR_NO_ARGS;
  }
  return error;
}

int create_threads(shared_data_t* shared_data) {
  assert(shared_data);
  int error = EXIT_SUCCESS;

  // creacion de los hilos que van a ser productor y consumidor
  pthread_t producer, consumer;
  error = pthread_create(&producer, /*attr*/ NULL, produce, shared_data);
  if (error == EXIT_SUCCESS) {
    error = pthread_create(&consumer, /*attr*/ NULL, consume, shared_data);
    if (error != EXIT_SUCCESS) {
      fprintf(stderr, "error: could not create consumer\n");
      error = ERR_CREATE_THREAD;
    }
  } else {
    fprintf(stderr, "error: could not create producer\n");
    error = ERR_CREATE_THREAD;
  }

  if (error == EXIT_SUCCESS) {
    // join de los hilos creados
    pthread_join(producer, /*value_ptr*/ NULL);
    pthread_join(consumer, /*value_ptr*/ NULL);
  }

  return error;
}

void* produce(void* data) {
  // const private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = (shared_data_t*)data;
  size_t count = 0;
  for (size_t round = 0; round < shared_data->rounds; ++round) {
    for (size_t index = 0; index < shared_data->buffer_capacity; ++index) {
      // wait(can_produce)
      // semaforo que permite producir
      sem_wait(&shared_data->can_produce);

      // tiempo que simula la produccion del producto
      usleep(1000 * random_between(shared_data->producer_min_delay
        , shared_data->producer_max_delay));
      shared_data->buffer[index] = ++count;
      // indica la produccion de un producto
      printf("Produced %lg\n", shared_data->buffer[index]);

      // signal(can_consume)
      // avisa que hay algo para consumir
      sem_post(&shared_data->can_consume);
    }
  }

  return NULL;
}

void* consume(void* data) {
  // const private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = (shared_data_t*)data;
  for (size_t round = 0; round < shared_data->rounds; ++round) {
    for (size_t index = 0; index < shared_data->buffer_capacity; ++index) {
      // wait(can_consume)
      // semaforo que permite consumir
      sem_wait(&shared_data->can_consume);

      // buffer donde se encuentra el producto a consumir
      double value = shared_data->buffer[index];
      // tiempo que simula lo que se dura en consumir
      usleep(1000 * random_between(shared_data->consumer_min_delay
        , shared_data->consumer_max_delay));
      // indica el consumo de un producto
      printf("\tConsumed %lg\n", value);

      // signal(can_produce)
      // avisa que hay espacio para producir
      sem_post(&shared_data->can_produce);
    }
  }

  return NULL;
}

useconds_t random_between(useconds_t min, useconds_t max) {
  return min + (max > min ? (random() % (max - min)) : 0);
}
