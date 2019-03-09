#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define L       3
#define MAX_ITEMS  1024
#define FILENAME   "datos.dat"
#define SALIR      0
#define CONTINUAR  1

#define NORTE 0
#define SUR 1
#define ESTE 2
#define OESTE 3

void obtenerDatos(double* datos,int *finalizar, int *cantidadNumeros);
int* vecinosToroide(int node);
void calcularMinimo(int rank, double numero);
double minimo(double a, double b);

int main(int argc, char* argv[]){

    double *datos;
    double buffNumero;
    int cantidadNumeros=0;
    int salir=CONTINUAR;
    MPI_Status status;
    int i;
    int size,rank;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(rank==0){

        datos = malloc(MAX_ITEMS*sizeof(double));
        obtenerDatos(datos,&salir,&cantidadNumeros);
        
        if(cantidadNumeros!=(L*L)){
            fprintf(stderr,"Error con el numero de datos\n");
            salir=SALIR;
        }
        if(size!=(L*L)){
            fprintf(stderr,"Error con el numero de nodos\n");
            salir=SALIR;
        }

        MPI_Bcast(&salir,1,MPI_INT,0,MPI_COMM_WORLD);
        
        if(salir==CONTINUAR){
            for(i=0;i<(L*L);i++){
                buffNumero = datos[i];
                MPI_Send(&buffNumero,1,MPI_DOUBLE,i,1,MPI_COMM_WORLD);
            }
        }

    }    

    MPI_Bcast(&salir,1,MPI_INT,0,MPI_COMM_WORLD);
    
    
    if(salir!=SALIR){
        MPI_Recv(&buffNumero,1,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        calcularMinimo(rank,buffNumero);
    }
    
    MPI_Finalize();

    return EXIT_SUCCESS;
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

int* vecinosToroide(int node){
    int fil,col;
    static int vecinos[L];

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

void calcularMinimo(int rank, double numero){
    int i;
    int *vecinos;
    MPI_Status status;
    vecinos = vecinosToroide(rank);
    double suNumero;

    for(i=1;i<L;i++){
        MPI_Send(&numero,1,MPI_DOUBLE,vecinos[SUR],1,MPI_COMM_WORLD);
        MPI_Recv(&suNumero,1,MPI_DOUBLE,vecinos[NORTE],1,MPI_COMM_WORLD,&status);
        numero = minimo(numero,suNumero);
    }


    MPI_Send(&numero,1,MPI_DOUBLE,vecinos[ESTE],1,MPI_COMM_WORLD);
    MPI_Recv(&suNumero,1,MPI_DOUBLE,vecinos[OESTE],1,MPI_COMM_WORLD,&status);
    numero = minimo(numero,suNumero); 
    

    if(rank==0){
        printf("El numero minimo es: %.2f\n",numero);
    }

}

double minimo(double a, double b){
    if(a<b){
        return a;
    }else{
        return b;
    }
}