// Copyright 2025 Isaias Alfaro Ugalde
#ifndef SRC_PLATE_H_
#define SRC_PLATE_H_

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>
#include <math.h>

#include "file.h"

/**
 * @struct shared_data
 * @brief Estructura que almacena los datos compartidos de los hilos.
 */
typedef struct shared_data {
    sem_t mutex_work_available;  // < Semaforo para el trabajo disponible
    sem_t mutex_balance;  // < Semaforo para el balance
    sem_t can_acces_count;  // < Semaforo para el acceso a count
    sem_t can_acces_count2;  // < Semaforo para el acceso a count2
    sem_t barrier_exchange;  // < Semaforo para la barrera de intercambio
    sem_t barrier_work;  // < Semaforo para la barrera de trabajo
    uint64_t work_available;  // < Variable para el trabajo disponible
    uint64_t balance;  // < Variable para el balance
    uint64_t count;  // < Contador de iteraciones
    uint64_t count2;  // < Contador de iteraciones
    data_job_t* data_job;  // < Puntero a los datos importantes de la simulacion
    uint64_t thread_count;  // < Cantidad de hilos
}shared_data_t;

/**
 * @struct private_data
 * @brief Estructura que almacena los datos privados de cada hilo.
 */
typedef struct private_data {
    uint64_t thread_number;  // < numero de hilo
    shared_data_t* shared_data;  // < puntero a la informacion compartida
}private_data_t;


/**
 * @brief Crea y gestiona los datos para la simulación de calor.
 *
 * Asigna memoria para las estructuras necesarias, llama a las funciones para
 * encontrar archivos, cargar datos y crear matrices. La función maneja errores
 * de asignación de memoria y libera los recursos al finalizar.
 *
 * @param filename Nombre del archivo que contiene los datos para la simulación.
 * @param job Nombre del trabajo que se utilizará para generar el archivo de salida.
 * @param thread_count Cantidad de hilos a usar en la ejecución. 
 */
void make_data(const char *filename, char job[], uint64_t thread_count);

/**
 * @brief Crea la matriz de datos de calor y gestiona el proceso de simulación.
 *
 * Llama a las funciones para cargar datos y crear matrices de calor. Asigna y
 * libera memoria para la matriz de calor durante el proceso.
 *
 * @param shared_data Estructura que contiene la información de la placa.
 * @param dat Array de datos que se utilizarán en la simulación.
 * @param cont_array Puntero a un entero que cuenta el número de datos procesados.
 * @param job Nombre del trabajo que se utilizará para generar el archivo de salida.
 */
void make_matrix(shared_data_t* shared_data, data_array_t *dat,
    int *cont_array, char job[]);

/**
 * @brief Metodo que se llama cuando la matriz es muy pequeña y es mejor trabajar con un solo hilo.
 * 
 * @param data_job Struct con la informacion necesaria para la simulacion.
 */
void heat_serial(data_job_t* data_job);

/**
 * @brief Metodo de creacion de equipos para la simulacion.
 * 
 * Crea los equipos de hilos que van a simular la tranferencia de calor en la placa
 * tambien los elimina y vuelve a crear para otra iteracion.
 * 
 * @param shared_data Puntero a la estructura que contiene los datos compartidos de los hilos.
 */
void heat_team(shared_data_t* shared_data);

/**
 * @brief Calcula y actualiza la distribución de calor en la placa.
 *
 * Utiliza el método de difusión para calcular la distribución del calor en la
 * placa.
 *
 * @param data Puntero con la información privada de cada hilo para su correcta ejecución.
 */
void* make_heat(void* data);

/**
 * @brief Libera la memoria asignada para las estructuras de datos.
 *
 * Libera la memoria asignada para las estructuras `data_job` y `data_array` para
 * evitar fugas de memoria.
 *
 * @param data_job Estructura que contiene la información de la placa.
 * @param data_array Array de datos que se utilizarán en la simulación.
 * @param shared_data Estructura de datos compartidos.
 */
void make_free(data_job_t* data_job, data_array_t* data_array,
    shared_data_t* shared_data);

/**
 * @brief Crea la matriz para la simulacion
 *
 * Creamos la matriz, la cual es un array de arrays y le asignamos la 
 * memoria para poder llenarla.
 *
 * @param filas Int que me dice las filas de la matriz
 * @param columnas Int que me cuenta las columnas de la matriz.
 * @param plate_data Struct donde guardar los arrays.
 */
void matrix(int filas, int columnas, struct data_job *plate_data);

/**
 * @brief Liberar la memoria de la matriz
 *
 * Libera la memoria de la matriz, con un for para liberar correctamente 
 * cada array.
 *
 * @param plate_data Struct donde estan los arrays.
 */
void free_matrix(struct data_job *plate_data);

/**
 * @brief Metodo de analisis de argumentos.
 * 
 * Pide los argumentos al usuario, analiza en caso de errores y los guarda
 * en sus variables respectivas para la ejecucion del programa.
 * 
 * @param filename Puntero donde se guarda el nombre del archivo de trabajo
 * @param filename_size Tamano del archvio de trabajo
 * @param job Nombre base del archivo de trabajo
 * @param job_size Tamano del nombre base
 * @param thread_count Cantidad de hilos para la ejecucion
 * @return int Retorno para ver si hay algun error de entrada
 */
int analyze_arguments(char* filename, size_t filename_size, char* job,
    size_t job_size, uint64_t* thread_count);

#endif  // SRC_PLATE_H_ // Fin del guardia de inclusión
