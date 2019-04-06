/*************************************************************************
 * 
 * Project          : Practica 1 de Diseño de infraestructuras de Red
 * 
 * Program name     : RedToroide.c 
 * 
 * Author           : David Carneros Prado
 * 
 * Date created     : 10/03/2019
 * 
 * Purpose          : Simulación de una red toroide para calcular el valor minimo
 *                    de un conjunto de datos. El proceso con rank==0 sera el encargado
 *                    de obtener esos datos del archivo "datos.dat" y de enviarlo 
 *                    a los procesos. Tambien enviara el proceso con rank==0 si se 
 *                    debe continuar o no con la ejecución.
 *                    Despues todos realizaran el siguiente algoritmo:
 *                      Desde 1 a L :
 *                         enviar(SUR,MiNumero)
 *                         recibir(NORTE,suNumero)
 *                         MiNumero = min(MiNumero,SuNumero)
 *                      Desde 1 a L :
 *                         enviar(ESTE,MiNumero)
 *                         recibir(OESTE,suNumero)
 *                         MiNumero = min(MiNumero,SuNumero)
 * 
 *                      El algoritmo tendrá una complejidad O(raiz(n)) o lo 
 *                      que es lo mismo O(L)
 * 
 * 
 * **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


#define MAX_ITEMS  1024
#define FILENAME   "datos.dat"
#define TRUE   1
#define FALSE  0

#define L     4

#define NORTE 0
#define SUR   1
#define ESTE  2
#define OESTE 3

/* Declaracion de funciones */
int* vecinosToroide(int node);
void obtenerDatos(double* datos,int *continuar, int *cantidadNumeros);
void calcularMinimo(int rank, double numero);
double minimo(double a, double b);

/* MAIN*/
int main(int argc, char* argv[]){

    double *datos;
    double buffNumero;
    int cantidadNumeros=0,continuar=TRUE;
    MPI_Status status;
    int size,rank,i;

    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    /* Para el proceso con rank 0 */
    if(rank==0){
        /* Inicializamos el array de datos y obtenemos los datos del fichero*/
        datos = malloc(MAX_ITEMS*sizeof(double));
        obtenerDatos(datos,&continuar,&cantidadNumeros);
        /* Si el numero de datos no es igual al numero de nodos no podra ejecutarse */
        if(cantidadNumeros!=(L*L)){
            fprintf(stderr,"Error con el número de datos\n");
            continuar=FALSE;
        }
        /* Si la cantidad de nodos no es la misma que L*L no podra ejecutarse*/
        if(size!=(L*L)){
            fprintf(stderr,"Error con el número de nodos\n");
            continuar=FALSE;
        }
        /* Multidifusion para saber si continuar con la ejecucion o no*/
        MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
        
        /* Si se continua con la ejecucion mandar el dato a cada uno de los nodos*/
        if(continuar){
            for(i=0;i<(L*L);i++){
                buffNumero = datos[i];
                MPI_Bsend(&buffNumero,1,MPI_DOUBLE,i,1,MPI_COMM_WORLD);
            }
            /* Liberamos el array de datos puesto que no lo vamos a usar ya*/
            free(datos);
        }

    }    
    /* Recibir del nodo 0 si se ha de continuar o no*/
    MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
    
    /* Si continuamos, esperamos recibir el numero y empezamos a calcular el minimo*/
    if(continuar){
        MPI_Recv(&buffNumero,1,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        calcularMinimo(rank,buffNumero);
    }
    
    MPI_Finalize();

    return EXIT_SUCCESS;
}
/*
Funcion para obtener los datos del fichero "datos.dat"
Se le pasara por referencia el array datos, la variable continuar y 
la cantidad de numeros.
*/
void obtenerDatos(double* datos,int *continuar,int *cantidadNumeros){
    char *linea;
    char *token;
    FILE *file;
    linea = malloc(MAX_ITEMS*sizeof(char));
    /* Si no se puede leer el fichero mostramos el error y no podra continuar */
    if((file = fopen(FILENAME,"r"))==NULL){
        fprintf(stderr,"Error al abrir el archivo %s\n",FILENAME);
        continuar = FALSE;
    } 
    else{
        /*Hacemos uso de la funcion strtok para leer la cadena separada por comas*/
        fgets(linea,MAX_ITEMS*sizeof(char),file);
        token = strtok(linea,",");
        while(token!=NULL){
            /* vamos metiendo el dato en el array e incrementando el puntero que hara de indice*/
            datos[(*cantidadNumeros)++]=atof(token);
            token = strtok(NULL,",");
        }  
    }
    fclose(file);
    free(linea);
}
/*
Funcion para obtener los vecinos que tiene un nodo dado su rank.
Se devolverá un array de 4 posiciones, en las que cada posicion indicará:
    vecinos[NORTE] = vecinos[0] <-- El vecino norte
    vecinos[SUR]   = vecinos[1] <-- El vecino sur
    vecinos[ESTE]  = vecinos[4] <-- El vecino este
    vecinos[OESTE] = vecinos[3] <-- El vecino oeste
*/
int* vecinosToroide(int node){
    int fil,col;
    static int vecinos[4];

    fil = node / L;
    col = node % L;

    /* Primero calculamos los vecinos NORTE y SUR*/

    switch(fil){
        /* Caso de fila inferior*/
        case 0:
            vecinos[NORTE] = node + L;
            vecinos[SUR] = (L-1)*L + node;
            break;
        /* Caso de fila superior */
        case L-1:
            vecinos[NORTE] = node % L;
            vecinos[SUR] = node - L; 
            break;
        /* Caso de fila central */
        default:
            vecinos[NORTE] = node + L;
            vecinos[SUR] = node - L; 
            break;
    }


   /* Calculamos los vecinos ESTE y OESTE */

    switch(col){
        /* Caso de columna izquierda*/
        case 0:
            vecinos[OESTE] = (L * (fil+1))-1;
            vecinos[ESTE] = node + 1;
            break;
        /* Caso de columna derecha*/
        case L-1:
            vecinos[OESTE] = node -1;
            vecinos[ESTE] = fil * L;
            break;
        /* Caso de columna central */
        default: 
            vecinos[OESTE] = node -1;
            vecinos[ESTE] = node + 1;  
            break;
    }

    return vecinos;
}

/*
Función que implementa el algoritmo:
       Desde 1 a L:
          enviar(SUR,MiNumero)
          recibir(NORTE,suNumero)
          MiNumero = min(MiNumero,SuNumero)
       Desde 1 a L:
           enviar(ESTE,MiNumero)
           recibir(OESTE,suNumero
           MiNumero = min(MiNumero,SuNumero)

Solo el nodo con rank==0 imprimirá el resultado.
El algoritmo tendrá una complejidad O(raiz(n)) o lo que es lo mismo O(L)
*/
void calcularMinimo(int rank, double numero){
    int i;
    int *vecinos;
    MPI_Status status;
    vecinos = vecinosToroide(rank);
    double suNumero;
    
    for(i=1;i<L;i++){
        MPI_Bsend(&numero,1,MPI_DOUBLE,vecinos[SUR],1,MPI_COMM_WORLD);
        MPI_Recv(&suNumero,1,MPI_DOUBLE,vecinos[NORTE],1,MPI_COMM_WORLD,&status);
        numero = minimo(numero,suNumero);
    }

    for(i=1;i<L;i++){
        MPI_Bsend(&numero,1,MPI_DOUBLE,vecinos[ESTE],1,MPI_COMM_WORLD);
        MPI_Recv(&suNumero,1,MPI_DOUBLE,vecinos[OESTE],1,MPI_COMM_WORLD,&status);
        numero = minimo(numero,suNumero); 
    }

    

    if(rank==0){
        printf("El número minimo es: %f\n",numero);
    }

}

/*
Devuelve el menor numero de los pasados por argumentos
*/
double minimo(double a, double b){
    if(a<b){
        return a;
    }else{
        return b;
    }
}