//gcc mutext.c -o mutex -DMUTEX -DFIN -lpthread
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#define nproc 100
#define cuadrado 1000

void *hijo();
void waitS();
void signalS();
void printTodo();

int semaforo;
struct sembuf operacion;

int **matriz2,**matriz1,**matriz3;

//tamaño de matrices
int FilasA = cuadrado, ColumnasA = cuadrado;
int FilasB = cuadrado, ColumnasB = cuadrado;
int main(){
    int i,j,k;//indices for


    //inicializacion
    matriz2 = (int **) malloc(FilasA*sizeof(int *));
    for(i =0; i<FilasA;i++ ){
        matriz2[i] = (int *) malloc(ColumnasA*sizeof(int));
    }
    for(i = 0; i<FilasA; i++){
        for(j=0; j<ColumnasA; j++){
            int nntmp= rand()%20;
            matriz2[i][j]=nntmp;
        }
    }
    matriz1 = (int **) malloc(FilasB*sizeof(int *));
    for(i =0; i<FilasB;i++ ){
        matriz1[i] = (int *) malloc(ColumnasB*sizeof(int));
    }
    for(j = 0; j<ColumnasB; j++){
        for(i=0; i<FilasB; i++){
            int ntmp= rand()%20;
            matriz1[i][j]=ntmp;
        }
    }
    matriz3 = (int **) malloc(FilasA*sizeof(int *));
    for(i =0; i<FilasA;i++ ){
        matriz3[i] = (int *) malloc(ColumnasA*sizeof(int));
    }
    for(i = 0; i<FilasA; i++){
        for(j=0; j<ColumnasA; j++){
            matriz3[i][j]=0;
        }
    }
    
    //Creacion de la llave
    key_t clave = ftok("/bin/ls",33);
    //Creacion del semaforo
    if((semaforo=semget(clave,2,0600 | IPC_CREAT))== -1){
        printf("error semget\n");
        exit(1);
    }
    //Inicializar semaforo
    semctl(semaforo,0,SETVAL,1);

    pthread_t hijos[nproc];
    int aux[nproc];

    #ifdef verbose
    printf("Log:\n");
    #endif
    for(i=0; i<nproc; i++){
        aux[i] = i+1;
        pthread_create(&hijos[i],NULL,hijo, (void *)&aux[i]);
        
    }
    for(i=0; i<nproc; i++){
        pthread_join(hijos[i],NULL);
    }
    printTodo();
    return 0;
}
void waitS(){
    operacion.sem_num =0;
    operacion.sem_op =-1;
    operacion.sem_flg =SEM_UNDO;
    if(semop(semaforo,&operacion,1)==-1){
        printf("error semop\n");
        exit(1);
    }
}
void signalS()
{
    operacion.sem_num =0;
    operacion.sem_op =1;
    operacion.sem_flg =SEM_UNDO;
    if(semop(semaforo,&operacion,1)==-1){
        printf("error semop\n");
        exit(1);
    }
}

void *hijo(void * arg){
    int current = *(int *)arg;
    int i,j,k;//indices for
    int factor =0;
    
    for(i=0; i<FilasA; i++){
        if((nproc*factor)+(current-1)==i){//selector
            #ifdef verbose
                printf("(nproc[%d]*factor[%d])+(current[%d]-1)==i[%d]\n",nproc,factor,current,i);
            #endif
            waitS();
            for(j=0; j<ColumnasA; j++){
                for(k=0; k<FilasB; k++){
                    matriz3[j][i]+=matriz2[k][i]*matriz1[j][k];
                }
            }
            #ifdef verbose
                printf("Resultado de la fila %d desde el hijo %d:\n",i,current);
                for(j=0; j<ColumnasA; j++){
                    printf("|%d",matriz3[j][i]);
                }
                printf("|\n");
            #endif
            signalS();
        }
        factor+=(i+1==nproc*(factor+1));
    }
}
void printTodo(){
    int i,j,k;//indices for

    printf("\n");
    #ifdef verbose
        printf("Matriz 1:\n");
    #endif
    for(j = 0; j<FilasB; j++){
        for(k=0; k<ColumnasB; k++){
            printf("|%d",matriz1[j][k]);
        }
        printf("|\n");
    }
    
    printf("\n");
    printf("\n");
    #ifdef verbose
        printf("Matriz 2:\n");
    #endif
    for(j = 0; j<FilasA; j++){
        for(k=0; k<ColumnasA; k++){
            printf("|%d",matriz2[j][k]);
        }
        printf("|\n");
    }
    
    printf("\n");
    printf("\n");
    #ifdef verbose
        printf("Matriz 3:\n");
    #endif
    for(j = 0; j<FilasA; j++){
        for(k=0; k<ColumnasA; k++){
            printf("|%d",matriz3[j][k]);
        }
        printf("|\n");
    }
}