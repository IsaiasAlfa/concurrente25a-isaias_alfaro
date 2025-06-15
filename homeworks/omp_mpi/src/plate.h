// Copyright 2025 Isaias Alfaro Ugalde
#ifndef SRC_PLATE_H_
#define SRC_PLATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <omp.h>
#include <math.h>
#include <mpi.h>

#include "file.h"

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
 * @brief Calcula el índice de inicio para un proceso en un entorno distribuido.
 *
 * Esta función determina el índice de inicio para un proceso dado su rango,
 * el número total de procesos, y el rango de inicio y fin del trabajo.
 *
 * @param rank Rango del proceso actual.
 * @param end Índice final del trabajo.
 * @param workers Número total de procesos (trabajadores).
 * @param begin Índice inicial del trabajo.
 * @return int Índice de inicio calculado para el proceso.
 */
int calculate_start(int rank, int end, int workers, int begin);

/**
 * @brief Calcula el índice de finalización para un proceso en un entorno distribuido.
 *
 * Esta función determina el índice de finalización para un proceso dado su rango,
 * el número total de procesos, y el rango de inicio y fin del trabajo.
 *
 * @param rank Rango del proceso actual.
 * @param end Índice final del trabajo.
 * @param workers Número total de procesos (trabajadores).
 * @param begin Índice inicial del trabajo.
 * @return int Índice de finalización calculado para el proceso.
 */
int calculate_finish(int rank, int end, int workers, int begin);

/**
 * @brief Crea la matriz de datos de calor y gestiona el proceso de simulación.
 *
 * Llama a las funciones para cargar datos y crear matrices de calor. Asigna y
 * libera memoria para la matriz de calor durante el proceso.
 *
 * @param data_job Estructura que contiene la información de la placa y los parámetros de simulación.
 * @param dat Array de datos que se utilizarán en la simulación.
 * @param cont_array Puntero a un entero que cuenta el número de datos procesados.
 */
void make_matrix(data_job_t* data_job, data_array_t *dat,
    int *cont_array);

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
 * @param data_job Estructura que contiene la información de la placa.
 */
void heat_team(data_job_t* data_job);

/**
 * @brief Calcula y actualiza la distribución de calor en la placa.
 *
 * Utiliza el método de difusión para calcular la distribución del calor en la
 * placa.
 *
 * @param data_job Estructura que contiene la información de la placa y los parámetros de simulación.
 */
void make_heat(data_job_t* data_job);

/**
 * @brief Libera la memoria asignada para las estructuras de datos.
 *
 * Libera la memoria asignada para las estructuras `data_job` y `data_array` para
 * evitar fugas de memoria.
 *
 * @param data_job Estructura que contiene la información de la placa.
 * @param data_array Array de datos que se utilizarán en la simulación.
 */
void make_free(data_job_t* data_job, data_array_t* data_array);

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
 * @param argc Puntero al contador de argumentos
 * @param argv Puntero al array de argumentos
 * @return int Retorno para ver si hay algun error de entrada
 */
int analyze_arguments(char* filename, size_t filename_size, char* job,
    size_t job_size, uint64_t* thread_count, int* argc, char*** argv);

#endif  // SRC_PLATE_H_ // Fin del guardia de inclusión
