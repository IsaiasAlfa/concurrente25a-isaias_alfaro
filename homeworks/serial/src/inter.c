// Copyright 2024 Isaias Alfaro Ugalde

#include <inter.h>

void jobs_find_file(const char *filename, struct data_array *dat,
    int *cont_array) {
  assert(filename);
  // Intenta abrir el archivo en modo lectura ("r")
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    jobs_charge_data(file, dat, cont_array);
    jobs_close_file(file);  // Cierra el archivo después de cargar los datos
  } else {
    printf("No se pudo abrir el archivo.  \n");
  }
}

void jobs_charge_data(FILE *file, struct data_array *dat, int *cont) {
  int numEntries = 0;  // Contador de entradas leídas
  char buffer[255];  // Buffer para almacenar cada línea leída del archivo
  while (fgets(buffer, sizeof(buffer), file) != NULL && numEntries < 40) {
    sscanf(buffer, "%s %lf %lf %lf %lf",
        dat[numEntries].plate, &dat[numEntries].value_time,
        &dat[numEntries].value_material, &dat[numEntries].value_area,
        &dat[numEntries].value_epsilon);
    numEntries++;
  }
  *cont = numEntries;  // Actualiza el número de entradas leídas
}

void plate_charge_RC(struct data_job *plate_data, int position,
    struct data_array *dat) {
  double time = dat[position].value_time;
  double material = dat[position].value_material;
  double area = dat[position].value_area;
  double epsilon = dat[position].value_epsilon;

  uint64_t RB = 0;  // Número de filas
  uint64_t CB = 0;  // Número de columnas

  char file_path[100];
  snprintf(file_path, sizeof(file_path), "tests/%s", dat[position].plate);
  FILE *plate = fopen(file_path, "rb");  // Abre el archivo en modo binario
  if (plate == NULL) {
    printf("Error al abrir el archivo\n");
    return;
  }

  // Leer el número de filas (R) del archivo
  if (fread(&RB, sizeof(uint64_t), 1, plate) != 1) {
    printf("Error al leer el número de filas (R)\n");
    fclose(plate);
    return;
  }

  // Leer el número de columnas (C) del archivo
  if (fread(&CB, sizeof(uint64_t), 1, plate) != 1) {
    printf("Error al leer el número de columnas (C)\n");
    fclose(plate);
    return;
    }

    // Almacena los datos leídos en la estructura `heat`
    plate_data->R = RB;
    plate_data->C = CB;
    plate_data->time = time;
    plate_data->material = material;
    plate_data->area = area;
    plate_data->epsilon = epsilon;

  fclose(plate);  // Cierra el archivo
}

void plate_charge_heat(int position, struct data_array *dat,
    struct heat **heat_data) {
  double auxiliar = 0;
  char file_path[100];
  snprintf(file_path, sizeof(file_path), "tests/%s", dat[position].plate);
  FILE *plate = fopen(file_path, "rb");  // Abre el archivo en modo binario
  if (plate == NULL) {
      printf("Error al abrir el archivo");
      return;
  }

  uint64_t R, C;
  fread(&R, sizeof(uint64_t), 1, plate);  // Lee el número de filas
  fread(&C, sizeof(uint64_t), 1, plate);  // Lee el número de columnas

  // Itera sobre cada celda de la matriz para leer los datos de calor
  for (uint64_t i = 0; i < R; i++) {
    for (uint64_t j = 0; j < C; j++) {
      fread(&auxiliar, sizeof(double), 1, plate);
      heat_data[i][j].past_warm = auxiliar;
      heat_data[i][j].current_warm = auxiliar;
    }
  }
  fclose(plate);  // Cierra el archivo
}

void print_data_array(const struct data_array *dat, int size) {
  for (int i = 0; i < size; i++) {
    printf("Entry %d:\n", i);
    printf("  Plate: %s\n", dat[i].plate);
    printf("  Value1: %lf\n", dat[i].value_time);
    printf("  Value2: %lf\n", dat[i].value_material);
    printf("  Value3: %lf\n", dat[i].value_area);
    printf("  Value4: %lf\n", dat[i].value_epsilon);
    printf("\n");
  }
}

void jobs_close_file(FILE *file) {
    fclose(file);  // Cierra el archivo
}

void jobs_out_file(struct data_array *dat, struct heat **heat_data,
    struct data_job *plate_data, FILE *job_out, int position) {
  char time_file[20];
  time_t seconds = (plate_data->time * plate_data->report);
  if (format_time(seconds, time_file, 20) != NULL) {
    fprintf(job_out, "%s\t", dat[position].plate);
    fprintf(job_out, "%g\t", plate_data->time);
    fprintf(job_out, "%g\t", plate_data->material);
    fprintf(job_out, "%g\t", plate_data->area);
    fprintf(job_out, "%g\t", plate_data->epsilon);
    fprintf(job_out, "%d\t", plate_data->report);
    fprintf(job_out, "%s\n", time_file);
  }
  char state_plate[100];
  char *dot_position = strchr(dat[position].plate, '.');
  size_t base_length = dot_position - dat[position].plate;
  snprintf(state_plate, sizeof(state_plate), "%.*s-%d%s",
      (int)base_length, dat[position].plate, plate_data->report, dot_position);
  FILE *file_plate = fopen(state_plate, "wb");
  fwrite(&plate_data->R, sizeof(uint64_t), 1, file_plate);
  fwrite(&plate_data->C, sizeof(uint64_t), 1, file_plate);
  for (uint64_t i = 0; i < plate_data->R; i++) {
    for (uint64_t j = 0; j < plate_data->C; j++) {
      double auxiliar = heat_data[i][j].current_warm;
      fwrite(&auxiliar, sizeof(double), 1, file_plate);
    }
  }
  fclose(file_plate);
}

char* format_time(const time_t seconds, char* text, const size_t capacity) {
  const struct tm* gmt = gmtime(&seconds);
  snprintf(text, capacity, "%04d/%02d/%02d\t%02d:%02d:%02d", gmt->tm_year - 70,
    gmt->tm_mon, gmt->tm_mday - 1, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
  return text;
}
