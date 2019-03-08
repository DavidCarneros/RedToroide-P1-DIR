/*************************************************************************
 * 
 * Project          : Practica 1 de Diseño de infraestructuras de Red
 * 
 * Program name     : DataGenerator.c 
 * 
 * Author           : David Carneros Prado
 * 
 * Date created     : 08/03/2019
 * 
 * Purpose          : Programa encargado de generar NUM numeros reales aleatorios
 *                    y guardarlos en el archivo datos.dat
 * 
 * 
 * **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME "datos.dat"
#define MAX 1000
#define MIN -1000


int main(int argc, char* argv[]){

    char buffer[10];
    float num;
    float rdn;
    int max;
    int i;
    int file;

    if(argc!=2){
        fprintf(stderr,"Error en el uso de argumentos: ./DataGenetator <NUM>\n");
    }

    max=atoi(argv[1]);

    if((file=open(FILENAME,O_WRONLY | O_CREAT | O_TRUNC,0644))==-1){
        fprintf(stderr,"Error al crear el archivo %s\n",FILENAME);
    }

    srand(getpid());
    srand48(getpid());
    
    rdn = drand48() + 1.0;

    for(i=0;i<max-1;i++){
        num = (rand() % (MIN - MAX + 1) + MIN) * rdn;
        sprintf(buffer,"%.2f,",num);
        write(file,buffer,sizeof(char)*strlen(buffer));
    }
    num = (rand() % (MIN - MAX + 1) + MIN) * rdn ;
    sprintf(buffer,"%.2f",num);
    write(file,buffer,sizeof(char)*strlen(buffer));

    return EXIT_SUCCESS;
}   