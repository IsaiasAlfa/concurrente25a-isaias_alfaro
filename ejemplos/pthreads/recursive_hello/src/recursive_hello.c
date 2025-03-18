// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>

/**
 * @brief Funcion que imprime un mensaje y crea un hilo si es mayor a 2
 * 
 * @param arg Puntero con el numero para ver si se ejecutara correctamente
 * @return Null 
 */
void* greet(void* arg);

// procedure main:
int main(void) {
  // create_thread(greet)
  pthread_t thread;  // creacion del hilo que se va a utilizar
  // ver si se creo o no el hilo
  size_t prueba = 2;
  int error = pthread_create(&thread, NULL, greet, &prueba);
  if (error == EXIT_SUCCESS) {
    // print "Hello from main thread"
    // usleep(1);  // indeterminism
    printf("Hello from main thread\n");
    pthread_join(thread, /*value_ptr*/ NULL);  // borrar el hilo que se creo
  } else {
    fprintf(stderr, "Error: could not create secondary thread\n");
  }
  return error;
}  // end procedure

// procedure greet:
// Metodo que se llama para que el nuevo hilo ejecute
void* greet(void* arg) {
  size_t number = *(size_t*)arg;
  if (number == 0) {
    printf("Bye ");
    printf("%ld\n", number);
  } else if (number > 0) {
    printf("Hello ");
    printf("%ld\n", number);
    number = number-1;
    pthread_t thread;
    int error = pthread_create(&thread, NULL, greet, &number);
    if (error == EXIT_SUCCESS) {
      pthread_join(thread, NULL);
  }
  }
  return NULL;
}  // end procedure
