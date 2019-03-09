#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <mpi.h>

#define LADO       4
#define MAX_ITEMS  1024
#define FILENAME   "datos.dat"
#define SALIR      0
#define CONTINUAR  1

void obtenerDatos(double* datos,int *finalizar, int *cantidadNumeros);

int main(int argc, char* argv[]){

    double *datos;
    int cantidadNumeros=0;
    int salir;
    int i;

    datos = malloc(MAX_ITEMS*sizeof(double));
    obtenerDatos(datos,&salir,&cantidadNumeros);
    
    if(cantidadNumeros!=(LADO*LADO)){
        fprintf(stderr,"Error con el numero de datos\n");
        salir=SALIR;
    }
    for(i=0;i<cantidadNumeros;i++){
        printf("%.2f\n",datos[i]);
    }
    

}

void obtenerDatos(double* datos,int *finalizar,int *cantidadNumeros){
    char *linea;
    char *token;
    
    FILE *file;

    linea = malloc(MAX_ITEMS*sizeof(char));

    if((file = fopen(FILENAME,"r"))==NULL){
        fprintf(stderr,"Error al abrir el archivo %s\n",FILENAME);
        finalizar = SALIR;

    } 
    else{
        fgets(linea,MAX_ITEMS*sizeof(char),file);
        token = strtok(linea,",");
        
        while(token!=NULL){
            datos[(*cantidadNumeros)++]=atof(token);
            token = strtok(NULL,",");
        }
        
        
    }

   free(linea);

}