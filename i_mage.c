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
    char *archivo, *fileRoute;
    archivo = malloc(20);
    fileRoute = malloc(30);
    strcpy(fileRoute, "./files/");
    for (int i = 1; i < argc; i++)
    {
        strcpy(archivo, argv[i]);
        strncat(fileRoute, archivo, strlen(archivo) - 4);
        datosImg datos;
        unsigned char *img = cargarImagen(archivo, &datos, fileRoute);
        if (img == NULL)
        {
            puts("No se pudo procesar la imagen");
            exit(1);
        }
        guardarArchivo(&datos, img, fileRoute);
        strcpy(fileRoute, "./files/");
    }
    clock_t end = clock();
    float transcurrido = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecucion: %.3f segundos.\n", transcurrido);
}

unsigned char *cargarImagen(char *filename, datosImg *metadatos, char *fileRoute)
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

void guardarArchivo(datosImg *metadatos, unsigned char *pixeles, char *fileRoute)
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
    for (y = metadatos->alto; y > 0; y--)
    {
        for (x = 0; x < metadatos->ancho; x++)
        {
            valorMedio += pixeles[x + y * (metadatos->ancho)];
            fprintf(file, " %d ", pixeles[x + y * (metadatos->ancho)]);
        }

        fprintf(file, "\n");
    }
    for (int i = 1; i < (metadatos->ancho * metadatos->alto); i++)
    {
        for (int j = 0; j < (metadatos->ancho * metadatos->alto) - 1; j++)
        {
            if (pixeles[j] > pixeles[j + 1])
            {
                int temp = pixeles[j];
                pixeles[j] = pixeles[j + 1];
                pixeles[j + 1] = temp;
            }
        }
    }
    FILE *procesado;
    procesado = fopen(route2, "wt");
    valorMedio = valorMedio / (metadatos->alto * metadatos->ancho);
    fprintf(procesado, "El valor medio es : %d\n", valorMedio);
    mediana = pixeles[(metadatos->alto * metadatos->ancho) / 2];
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