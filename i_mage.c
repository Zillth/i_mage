/**
 *  Desarrollado por Team Undefined
 *  Alan Jomar Flores Rodriguez
 *  Magaly Ciltiali Jimeno Reyes
 *  Mauricio Ramses Macias Espinosa
 *  
 *  Proyecto de procesado de imagenes en C
**/

#include "i_mage.h"

void iniciarProceso(int argc, char **argv)
{
    clock_t start = clock();
    int totalSize = 0;

#pragma omp parallel for
    for (int i = 1; i < argc; i++)
    {
        char *archivo, *fileRoute;
        archivo = malloc(20);
        fileRoute = malloc(30);
        char *archivoCargar;
        archivoCargar = malloc(20);
        printf("Procesando imagen en el proceso: %d\n", omp_get_thread_num());
        strcpy(fileRoute, "./files/");
        strcpy(archivo, argv[i]);
        strncat(fileRoute, archivo, strlen(archivo) - 5);
        datosImg datos;
        int nFragmentos = 0;
        if (archivo[strlen(archivo) - 1] == '6')
        {
            nFragmentos = 64;
        }
        else if (archivo[strlen(archivo) - 1] == '3')
        {
            nFragmentos = 32;
        }
        else
        {
            nFragmentos = 0;
        }
        strcpy(archivoCargar, "");
        strncat(archivoCargar, archivo, strlen(archivo) - 1);
        unsigned char *img = cargarImagen(archivoCargar, &datos, fileRoute, &totalSize);
        if (img == NULL)
        {
            puts("No se pudo procesar la imagen");
            exit(1);
        }
        guardarArchivo(&datos, img, fileRoute, nFragmentos);
    }
    clock_t end = clock();
    float transcurrido = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecucion: %.3f segundos.\n", transcurrido);
    printf("Tiempo estimado de transferencia en una VPN de 2.5 kb/s: %.3f segundos.\n", (totalSize / 2500.000) + transcurrido);
}

unsigned char *cargarImagen(char *filename, datosImg *metadatos, char *fileRoute, int *totalSize)
{

    cabeceraImg cabImg;
    unsigned char *imgData;

    FILE *fileImg;
    uint16_t type;
    fileImg = fopen(filename, "rb");
    if (fileImg == NULL)
        return NULL;
    fread(&type, sizeof(uint16_t), 1, fileImg);
    if (type != 19778)
    {
        puts("Imagen no compatible con el programa.");
        fclose(fileImg);
        return NULL;
    }

    fread(&cabImg, sizeof(cabeceraImg), 1, fileImg);
    fread(metadatos, sizeof(datosImg), 1, fileImg);

    printf("Obteniendo longitud de los archivos en el proceso: %d\n", omp_get_thread_num());
    int tempSize = *totalSize;
    tempSize += (int)cabImg.size;
    *totalSize = tempSize;

    printf("Guardando informacion en el proceso: %d\n", omp_get_thread_num());
    guardarInfo(type, cabImg, metadatos, fileRoute);
    //Lectura de la imagen
    imgData = (unsigned char *)malloc((metadatos->ancho) * (metadatos->alto));
    if (imgData == NULL)
    {
        puts("Error: No se pudo reservar memoria");
        fclose(fileImg);
        return NULL;
    }

    fseek(fileImg, cabImg.offset, SEEK_SET);

    fread(imgData, (metadatos->ancho) * (metadatos->alto), 1, fileImg);
    fclose(fileImg);
    return imgData;
}

void guardarArchivo(datosImg *metadatos, unsigned char *pixeles, char *fileRoute, int nFragmentos)
{
    char *route, *route2;
    route = malloc(sizeof(fileRoute) + 15);
    route2 = malloc(sizeof(fileRoute) + 25);
    strcpy(route, fileRoute);
    strcpy(route2, fileRoute);
    strcat(route, "pixeles.txt");
    strcat(route2, "procesado.txt");

    FILE *file;
    int valorMedio = 0, mediana = 0;
    file = fopen(route, "wt");
    int x, y;

    int tablaFrecuencias[15][2];
    for (int i = 0; i < 16; i++)
    {
        tablaFrecuencias[i][0] = (17) * (i + 1);
        tablaFrecuencias[i][1] = 0;
    }

    int index, claseEncontrada, posicionFrecuencia, numSegmentacion = 1;

    for (y = metadatos->alto; y > 0; y--)
    {
        for (x = 0; x < metadatos->ancho; x++)
        {
            claseEncontrada = 0;
            posicionFrecuencia = 0;
            index = x + y * (metadatos->ancho);
            while (!claseEncontrada)
            {
                if (pixeles[index] <= tablaFrecuencias[posicionFrecuencia][0])
                {
                    tablaFrecuencias[posicionFrecuencia][1] = tablaFrecuencias[posicionFrecuencia][1] + 1;
                    claseEncontrada = 1;
                }
                posicionFrecuencia++;
            }

            valorMedio += pixeles[index];
            fprintf(file, " %d ", pixeles[index]);
        }

        fprintf(file, "\n");
    }

    char *routeSegmentacion, *bufferItoa;
    routeSegmentacion = malloc(sizeof(fileRoute) + 15);
    bufferItoa = malloc(2);
    FILE *segmentacion;

    if (nFragmentos != 0)
    {

        unsigned int divisionFragmentos = (unsigned int)((metadatos->ancho) * (metadatos->alto)) / (nFragmentos * nFragmentos);
        //unsigned int divisionFragmento
        while ((metadatos->alto / divisionFragmentos) * numSegmentacion <= metadatos->alto || (metadatos->ancho / divisionFragmentos) * numSegmentacion <= metadatos->ancho)
        {
            itoa(numSegmentacion, bufferItoa, 10);
            strcpy(routeSegmentacion, fileRoute);
            strcat(routeSegmentacion, "_S_");
            strcat(routeSegmentacion, bufferItoa);
            strcat(routeSegmentacion, ".txt");

            segmentacion = fopen(routeSegmentacion, "wt");
            for (y = (metadatos->alto / divisionFragmentos) * numSegmentacion; y > (metadatos->alto / divisionFragmentos) * (numSegmentacion - 1); y--)
            {
                for (x = (metadatos->ancho / divisionFragmentos) * (numSegmentacion - 1); x < (metadatos->ancho / divisionFragmentos) * numSegmentacion; x++)
                {
                    index = x + y * ((metadatos->ancho / divisionFragmentos) * numSegmentacion);
                    fprintf(segmentacion, " %d ", pixeles[index]);
                }
                fprintf(segmentacion, "\n");
            }
            fclose(segmentacion);
            numSegmentacion++;
        }
    }
    int frecuenciaAbsoluta = 0, mitadDatos = (int)((metadatos->alto * metadatos->ancho) / 2);
    posicionFrecuencia = 0;
    for (int k = 0; k < 15; k++)
    {
        frecuenciaAbsoluta += tablaFrecuencias[k][1];
        if (frecuenciaAbsoluta >= mitadDatos)
        {
            posicionFrecuencia = k;
            break;
        }
    }
    printf("Calculando y guardando media y mediana en el proceso: %d\n", omp_get_thread_num());
    mediana = tablaFrecuencias[posicionFrecuencia - 1][0] + (((mitadDatos - (frecuenciaAbsoluta - tablaFrecuencias[posicionFrecuencia][1])) / tablaFrecuencias[posicionFrecuencia][1]) * 17);

    FILE *procesado;
    procesado = fopen(route2, "wt");
    valorMedio = valorMedio / (metadatos->alto * metadatos->ancho);
    fprintf(procesado, "El valor medio es : %d\n", valorMedio);
    fprintf(procesado, "La mediana es : %d\n", mediana);
    fclose(procesado);
    fclose(file);
}

void guardarInfo(uint16_t type, cabeceraImg cabImg, datosImg *metadatos, char *fileRoute)
{
    char *route;
    route = malloc(sizeof(fileRoute) + 15);
    strcpy(route, fileRoute);
    strcat(route, "info.txt");

    FILE *info;
    info = fopen(route, "wt");

    fprintf(info, "Tipo de imagen es: %d\n", type);
    fprintf(info, "El tamaño del archivo es: %d\n", cabImg.size);
    fprintf(info, "El offset (ubicacion de los datos): %d\n", cabImg.offset);
    fprintf(info, "Los bpp son: %d\n", metadatos->bpp);
    fprintf(info, "El ancho es :%d\n", metadatos->ancho);
    fprintf(info, "El alto es :%d\n", metadatos->alto);
    fprintf(info, "La resX es :%d\n", metadatos->resX);
    fprintf(info, "El resY es :%d\n", metadatos->resY);

    fclose(info);
}