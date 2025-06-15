// Copyright 2025 Isaias Alfaro Ugalde
#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

/**
* @brief Estructura que contiene los datos de la placa y parámetros para la simulación de calor.
*/
typedef struct data_job {
  char plate[20];          // < Nombre de la placa o etiqueta.
  double time;        // < Tiempo de simulación.
  double material;    // < Propiedad del material.
  double area;        // < Área de la placa.
  double epsilon;     // < Tolerancia para determinar el equilibrio térmico.
  uint64_t R;         // < Número de filas de la matriz de la placa.
  uint64_t C;         // < Número de columnas de la matriz de la placa.
  int balance;        // < Bool Indicador de equilibrio térmico.
  int report;         // < Contador de iteraciones o reportes en la simulación.
  double burn;        // < Constante de calor en la simulacion
  double* past_warm;  // < Array del calor pasado
  double* current_warm;  // < Array del calor futuro
  uint64_t thread_count;  // < Cantidad de hilos.
} data_job_t;

/**
* @brief Estructura que almacena los datos leídos desde un archivo para la simulación.
*/
typedef struct data_array {
  char plate[20];          // < Nombre de la placa o etiqueta.
  double value_time;       // Valor de tiempo leído del archivo.
  double value_material;   // Valor del material leído del archivo.
  double value_area;  //< Valor del área leído del archivo.
  double value_epsilon;  // < Valor de la tolerancia epsilon.
} data_array_t;

/**
 * @brief Imprime el contenido de un array de datos.
 *
 * @param dat Puntero al array de estructuras `data_array` a imprimir.
 * @param size Tamaño del array de datos a imprimir.
 */
void print_data_array(const struct data_array *dat, int size);

/**
 * @brief Busca y procesa un archivo para la simulación de datos.
 *
 * @param filename Nombre del archivo que contiene los datos.
 * @param dat Array de estructuras `data_array` donde se almacenarán los datos leídos.
 * @param cont Puntero a un entero que cuenta el número de datos procesados.
 */
void jobs_find_file(const char *filename, struct data_array *dat, int *cont);

/**
 * @brief Carga los datos de un archivo a una estructura `data_array`.
 *
 * @param file Puntero al archivo abierto para leer los datos.
 * @param dat Array de estructuras `data_array` donde se almacenarán los datos leídos.
 * @param cont Puntero a un entero que cuenta el número de datos procesados.
 */
void jobs_charge_data(FILE *file, struct data_array *dat, int *cont);

/**
 * @brief Cierra un archivo abierto.
 *
 * @param file Puntero al archivo que se va a cerrar.
 */
void jobs_close_file(FILE *file);

/**
 * @brief Carga las dimensiones de la matriz (filas y columnas) desde un `data_array` a la estructura `M`.
 *
 * @param plate_data Puntero a la estructura `M` que contiene la información de la placa.
 * @param position Posición en el array `data_array` de donde se cargarán los datos.
 * @param dat Array de estructuras `data_array` desde donde se cargarán los datos.
 */
void plate_charge_RC(struct data_job *plate_data, int position,
    struct data_array *dat);

/**
 * @brief Carga los datos de calor desde un `data_array` a la estructura `heat`.
 *
 * @param position Posición en el array `data_array` de donde se cargarán los datos de calor.
 * @param dat Array de estructuras `data_array` desde donde se cargarán los datos.
 * @param plate_data Esctructura con los datos del job y el plate.
 */
void plate_charge_heat(int position, struct data_array *dat,
    struct data_job *plate_data);

/**
 * @brief Escribe los resultados en el archvio job y su respectivo plate.
 *
 * @param dat Array de estructuras `data_array` desde donde se cargarán los datos.
 * @param plate_data Esctructura con los datos del job y el plate.
 * @param job Archivo al cual se le va a escribir la nueva informacion.
 * @param position Int para saber cual es la informacion del job correspondiente.
 */
void jobs_out_file(struct data_array *dat, struct data_job *plate_data,
    int position);

void jobs_final_file(const char *filename, char job[]);

/**
 * @brief Escribe los resultados en el archvio job y su respectivo plate.
 *
 * @param seconds segundos que duro la simulacion.
 * @param text Puntero donde se va a guardar la fecha y hora.
 * @param capacity Cantidad maxima de caracteres excritos.
 */
char* format_time(const time_t seconds, char* text, const size_t capacity);

#endif  // SRC_FILE_H_ // Fin del guardia de inclusión
