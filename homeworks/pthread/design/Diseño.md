vamos a profundizar un poco en el diseño de esta tarea, con lo cual vamos a dar una explicación sobre el UML que se encuentra en esta misma carpeta.
[[UML pthreads.pdf]]
[[UML pthreads.png]]


***
### Explicación
En el se ven los structs representados con un color verde para identificarlos de una mejor manera.
Los procesos grandes o significativos tienen un color naranja
Los procesos rojos son procesos críticos del programa.

***

* Primero del main sale un proceso del cual se encarga de cargar los datos en dos de las structs.
* Segundo con los datos generales ya cargados. El programa se encarga de empezar a rellenar las matrices en orden con los datos de calor.
* Tercero con la matriz ya cargada nos dirigimos al proceso llamado make_heat, este se va a encargar de crear uno de los structs este no esta relacionado con cargar información por eso esta separado en su creación.
* Cuarto creamos los hilos de ejecución para las matrices, con los hilos creados pasamos a la función make_heat_hilos.
* Quinto esta funcion se va a encargar de hacer la simulación de calor con los hilos definidos anteriormente. Cada hilo se mueve por una cantidad definida de casillas, asi logramos que todos los hilos trabajen.
* Sexto al terminar todos los hilos regresamos a la función make_matrix la cual se va a encargar de guardar la información en un documento y seguir con  la siguiente matriz. 