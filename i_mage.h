/**
 *  Desarrollado por Team Undefined
 *  Alan Jomar Flores Rodriguez
 *  Magaly Ciltiali Jimeno Reyes
 *  Mauricio Ramses Macias Espinosa
 *  
 *  Proyecto de procesado de imagenes en C
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//BMP
typedef struct
{
    unsigned int size;
    uint16_t reservadoCopia;
    uint16_t caracteristicasAdicionales;
    unsigned int offset;
} cabeceraImg;

// Encabezado del mapa de bits
typedef struct
{
    unsigned int tamCabecera;
    unsigned int ancho;
    unsigned int alto;
    uint16_t planos; //Numero de planos
    uint16_t bpp; //bits por pixel
    unsigned int metodoCompresion;
    unsigned int imgSize;
    unsigned int resX;
    unsigned int resY;
    unsigned int rangoColores; //2^bpp rangos de colores
    unsigned int coloresImportantes;

} datosImg;

void iniciarProceso(int, char**);

unsigned char* cargarImagen(char*, datosImg*, char*);

void guardarArchivo(datosImg*, unsigned char*, char*);

void guardarInfo(uint16_t, cabeceraImg, datosImg*, char*);

