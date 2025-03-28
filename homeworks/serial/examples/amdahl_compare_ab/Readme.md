Para satisfacer un requerimiento de procesamiento de sonido, los ingenieros de una empresa de desarrollo diseñaron dos algoritmos A y B. El algoritmo A es de fuerza bruta, y se estima que el 78% de su tiempo de ejecución se puede paralelizar. El algoritmo B es mucho más eficiente porque tarda un tercio de lo que tarda el algoritmo A en conseguir los mismos resultados, pero se estima que sólo el 29% de su ejecución es paralelizable.

El cliente necesita que el software pueda procesar un archivo de 10 minutos de audio en 20 minutos o menos. Los ingenieros implementaron el algoritmo A por ser más fácil, y lo probaron con el archivo de audio en una las computadoras con 8 núcleos que tiene cada usuario en la compañía cliente. Como resultado el algoritmo A en su versión serial tardó 1 hora y 9 minutos.

El equipo de desarrollo necesita saber cuál algoritmo paralelizar para satisfacer el requerimiento del cliente, si el algoritmo A, o el B, o ninguno de los dos. Ayude al equipo, el cual necesita los siguientes datos para poder tomar la decisión. Suponga que el tiempo paralelizable de los algoritmos se puede distribuir perfectamente entre los hilos de ejecución. Use precisión de tres decimales.

1. ¿Cuánto tiempo tardaría el algoritmo A paralelizado en procesar el archivo en la computadora de 8 núcleos? Respuesta {21.91} minutos.

2. ¿Cuánto tiempo tardaría el algoritmo B paralelizado en procesar el archivo en la computadora de 8 núcleos? Respuesta {17.16} minutos.

3. El algoritmo A paralelizado alcanzaría una eficiencia de Respuesta {0.39}.

4. El algoritmo B paralelizado alcanzaría una eficiencia de Respuesta {0.17}.

5. El cliente expresó estar interesado en lograr que el procesamiento del audio sea en tiempo real. Es decir, que procesar un archivo de 10 minutos tarde 10 minutos. El cliente está dispuesto a adquirir equipo de cómputo más rápido. Esta meta se puede lograr:

Con el algoritmo A paralelizado en una máquina con 16 núcleos. {}

Con el algoritmo B paralelizado en una máquina con 16 núcleos. {}

Con el algoritmo A paralelizado en una máquina con 32 núcleos. {}

Con el algoritmo B paralelizado en una máquina con 32 núcleos. {}

Con ninguna de las anteriores. {X}