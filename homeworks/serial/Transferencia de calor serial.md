# Problema
El problema se basa en poder simular con cierta exactitud como se comparta el calor en una lamina de un material en especifico. Esto se logra por medio de una simulación en la que se le inyecta a una matriz que simula ser la lamina calor por sus extremos y vemos como con el pasar del tiempo la lamina se va calentado hasta alcanzar un punto de equilibrio. 
***

### Ejemplo de entrada:
tests/job001.txt

### Ejemplo de salida:
tests/job001.tsv

***
# Diseño
El diseño consta de una matriz la cual contiene un struct de dos datos. Esta va a simular el calor con el tiempo actualizando sus datos hasta llegar al punto de equilibrio. 

Para mas información sobre el diseño ver el [[Diseño]] ubicado en la carpeta desing.

***
# Manual de usuario

## Build
Para poder compilar la solución se tiene que realizar lo siguiente:
* Abrir una terminal y ubicarse en la carpeta llamada serial de la tarea.
* En esta terminal se tiene que escribir el siguiente comando "make".
* Este comando va a construir por completo la solución de la tarea.

## Ejecución
Para ejecutar el programa se tiene que realizar lo siguiente:
* Abrir una terminal y ubicarse en la carpeta llamada serial de la tarea. 
* En esta terminal se tiene que escribir el siguiente comando "make run".
* Este comando va a compilar y ejecutar la solución de la tarea. 
* En este paso le va a solicitar una dirección de un documento.
  * Debe darle la ruta del archivo, por favor asegúrese que el programa pueda acceder al mismo.
  * Segundo el programa solo acepta archivos de nombre jobxxx.txt para su correcta ejecución. 

***
# Créditos

### Autor
Isaías Alberto Alfaro Ugalde
### Información de contacto
isaias.alfaro@ucr.ac.cr

### Créditos extra
""
