// Copyright 2025 Isaias Alfaro Ugalde
#ifndef SRC_PLATE_H_
#define SRC_PLATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

#include "file.h"
#include "ethread.h"

/**
 * @brief Estructura que almacena los datos compartidos de los hilos.
 * 
 */
typedef struct shared_data {
    heat_t** heat;
    data_job_t* data_job;
    uint64_t thread_count;
}shared_data_t;

/**
 * @brief Estructura que almacena los datos privados de cada hilo.
 * 
 */
typedef struct private_data {
    uint64_t thread_number;
    shared_data_t* shared_data;
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
 * Libera la memoria asignada para las estructuras `plate_data` y `dat` para
 * evitar fugas de memoria.
 *
 * @param plate_data Estructura que contiene la información de la placa.
 * @param dat Array de datos que se utilizarán en la simulación.
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
 */
heat_t** matrix(int filas, int columnas);

/**
 * @brief Liberar la memoria de la matriz
 *
 * Libera la memoria de la matriz, con un for para liberar correctamente 
 * cada array.
 *
 * @param filas Int que me dice las filas de la matriz
 * @param matrix Puntero que me da donde inicia la matriz.
 */
void free_matrix(heat_t** heat, int filas);

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
