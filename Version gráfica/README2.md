# Versión gráfica exploratoria

Esta carpeta contiene una versión gráfica experimental del programa, desarrollada como complemento de la versión oficial por consola (`mapa.c`).

El archivo principal de esta versión es:

```text
versiongrafica2.c
```

## Importante

Esta versión **no corresponde al entregable oficial principal**. Fue realizada como una prueba adicional para visualizar el mapa y el recorrido de forma gráfica.

Utiliza dependencias propias de Windows, por lo que debe compilarse en Windows con MinGW.

## Compilación

Desde la carpeta donde se encuentra `versiongrafica2.c`, ejecutar:

```cmd
gcc -Wall -Wextra -g3 versiongrafica2.c -o output\versiongrafica2.exe -lgdi32 -luser32 -lm
```

Si no existe la carpeta `output`, crearla antes:

```cmd
mkdir output
```

## Ejecución

```cmd
output\versiongrafica2.exe
```

## Cambio de archivo de entrada

Esta versión no solicita el archivo por consola, ya que fue desarrollada como complemento experimental.

Para cambiar el archivo de entrada, se debe modificar directamente el nombre del archivo en el código, aproximadamente en la **línea 931**:

```c
char *nombre_archivo = "input.txt";
```

Por ejemplo:

```c
char *nombre_archivo = "input2.txt";
```

o:

```c
char *nombre_archivo = "input3.txt";
```

## Consideraciones

- La versión oficial del proyecto es `mapa.c`.
- Esta versión gráfica depende de Windows.
- El cambio de input se realiza desde el código fuente.
- Las instrucciones principales del proyecto están en el README general.

## Autores

- Rodrigo Domínguez Larenas
- Ariel Fernández Fuentealba
- Felipe Grandón Contreras
