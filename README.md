# RedToroide-P1-DIR
Red_Toroide::Practica1::DIR


COMO EJECUTAR LA PRACTICA: 

Dentro del directorio Practica: 

    Para compilar: 
        1. $ all

    Para ejecutar:

        Primeros generamos los datos. Hay dos formas para ello
            1) make generateData16        # Genera 8 datos 
            2) ./DataGenerator 16         # Genera 8 datos

        Despues ejecutamos el programa. Hay dos formas: 
            1) make toroide_16
            2) mpirun -n 16 ./RedToroide
