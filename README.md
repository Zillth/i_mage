# Bienvenidos a nuestro proyecto de procesamiento de imagenes BMP con C

## Instrucciones para la ejecución

- Ejecuta el comando **gcc  i_mage.h  i_mage.c  main.c  -o  main  -fopenmp** (en el caso que no exista el archivo main.exe)
- Ejecuta el archivo .exe con el nombre de los archivos que desees procesar junto con un 6 si deseas segmentarlos kernels de 64x64, 3 si lo quieres de 32x32 y 0 si no deseas segmentarlo "./main  **nombre archivo**6  **nombre archivo**3  ..."
- Listo, los archivos de texto se generaran en la carpeta "files", (Si no existe la carpeta files agregarla en el directorio principal donde tiene este proyecto)

### Ejemplo de ejecución
./main ejemplo.bmp6 ejemplo2.bmp3 ejemplo3.bmp0