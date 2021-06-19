/**
 *  Desarrollado por Team Undefined
 *  Alan Jomar Flores Rodriguez
 *  Magaly Ciltiali Jimeno Reyes
 *  Mauricio Ramses Macias Espinosa
 *  
 *  Proyecto de procesado de imagenes en C
**/

#include "i_mage.h"

void iniciarProceso(int argv, char **argc)
{
    datosImg datos;
    unsigned char *img = cargarImagen("lena512.bmp", &datos);
    if (img == NULL)
    {
        puts("No se pudo procesar la imagen");
        exit(1);
    }
    guardarArchivo(&datos, img);
}

unsigned char *cargarImagen(char *filename, datosImg *metadatos)
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

    guardarInfo(type, cabImg, metadatos);

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

void guardarArchivo(datosImg *metadatos, unsigned char *pixeles)
{
    FILE *file;
    file = fopen("pixels.txt", "wt");
    int x, y, posX = 0, posY = 0;
    for (y = metadatos->alto; y > 0; y--)
    {
        for (x = 0; x < metadatos->ancho; x++)
        {
            fprintf(file, " %d ", pixeles[x + y * (metadatos->ancho)]);
            //printf("El pixel en la posicion [%d],[%d] es %d\n", posX, posY, pixeles[x+y*(metadatos->ancho)]);
            //posX++;
        }
        fprintf(file, "\n");
        //posY++;
    }
    fclose(file);
}

void guardarInfo(uint16_t type, cabeceraImg cabImg, datosImg *metadatos)
{
    FILE *info;
    info = fopen("info.txt", "wt");

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