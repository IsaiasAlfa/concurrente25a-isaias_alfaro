// Copyright 2025 Isaias Alfaro Ugalde
#ifndef SRC_MATRIX_H_
#define SRC_MATRIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inter.h>


/**
 * @brief Crea y gestiona los datos para la simulación de calor.
 *
 * Asigna memoria para las estructuras necesarias, llama a las funciones para
 * encontrar archivos, cargar datos y crear matrices. La función maneja errores
 * de asignación de memoria y libera los recursos al finalizar.
 *
 * @param filename Nombre del archivo que contiene los datos para la simulación.
 * @param job Nombre del trabajo que se utilizará para generar el archivo de salida.
 */
void make_data(const char *filename, char job[]);

/**
 * @brief Crea la matriz de datos de calor y gestiona el proceso de simulación.
 *
 * Llama a las funciones para cargar datos y crear matrices de calor. Asigna y
 * libera memoria para la matriz de calor durante el proceso.
 *
 * @param plate_data Estructura que contiene la información de la placa.
 * @param dat Array de datos que se utilizarán en la simulación.
 * @param cont_array Puntero a un entero que cuenta el número de datos procesados.
 * @param job Nombre del trabajo que se utilizará para generar el archivo de salida.
 */
void make_matrix(struct data_job *plate_data, struct data_array *dat,
    int *cont_array, char job[]);

/**
 * @brief Calcula y actualiza la distribución de calor en la placa.
 *
 * Utiliza el método de difusión para calcular la distribución del calor en la
 * placa hasta que se alcanza el equilibrio.
 *
 * @param plate_data Estructura que contiene la información de la placa.
 * @param heat_data Matriz que contiene los datos de calor actuales y pasados.
 */
void make_heat(struct data_job *plate_data, struct heat **heat_data);

/**
 * @brief Libera la memoria asignada para las estructuras de datos.
 *
 * Libera la memoria asignada para las estructuras `plate_data` y `dat` para
 * evitar fugas de memoria.
 *
 * @param plate_data Estructura que contiene la información de la placa.
 * @param dat Array de datos que se utilizarán en la simulación.
 */
void make_free(struct data_job *plate_data, struct data_array *dat);

/**
 * @brief Crea la matriz para la simulacion
 *
 * Creamos la matriz, la cual es un array de arrays y le asignamos la 
 * memoria para poder llenarla.
 *
 * @param filas Int que me dice las filas de la matriz
 * @param columnas Int que me cuenta las columnas de la matriz.
 */
struct heat **matrix(int filas, int columnas);

/**
 * @brief Liberar la memoria de la matriz
 *
 * Libera la memoria de la matriz, con un for para liberar correctamente 
 * cada array.
 *
 * @param filas Int que me dice las filas de la matriz
 * @param matrix Puntero que me da donde inicia la matriz.
 */
void free_matrix(struct heat **matrix, int filas);

#endif  // SRC_MATRIX_H_ // Fin del guardia de inclusión
