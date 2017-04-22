#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){
    int nproc = 2;//numero de procesos
    int i,j,k;//indices for

    int active = 1;//numero de procesos activos
    int current = 0;//proceso (hijo) en ejecucion
    int status = 0;//variable para el wait

    //tamaño de matrices
    int FilasA = 3, ColumnasA = 3;
    int FilasB = 3, ColumnasB = 3;

    //pipe
    int nbytes;

    pid_t pid = 1, wpid;
    srand(time(NULL));

    int **pipes = (int **)malloc(nproc * sizeof(int *));
    for(i = 0; i<nproc;i++){
        pipes[i] = (int *) malloc(2*sizeof(int));
    }

    //Alocacion e inicializacion
    int **matriz2 = (int **) malloc(FilasA*sizeof(int *));
    for(i =0; i<FilasA;i++ ){
        matriz2[i] = (int *) malloc(ColumnasA*sizeof(int));
    }
    for(i = 0; i<FilasA; i++){
        for(j=0; j<ColumnasA; j++){
            int nntmp= rand()%20;
            matriz2[i][j]=nntmp;
        }
    }
    int **matriz1 = (int **) malloc(FilasB*sizeof(int *));
    for(i =0; i<FilasB;i++ ){
        matriz1[i] = (int *) malloc(ColumnasB*sizeof(int));
    }
    for(j = 0; j<ColumnasB; j++){
        for(i=0; i<FilasB; i++){
            int ntmp= rand()%20;
            matriz1[i][j]=ntmp;
        }
    }
    int **third = (int **) malloc(FilasA*sizeof(int *));
    for(i =0; i<FilasA;i++ ){
        third[i] = (int *) malloc(ColumnasA*sizeof(int));
    }
    for(i = 0; i<FilasA; i++){
        for(j=0; j<ColumnasA; j++){
            third[i][j]=0;
        }
    }

    for(i=0; i<FilasA; i++){
        int *temp = malloc((ColumnasA +1) * sizeof(int));
        for(j=0; j<=ColumnasA; j++){
            temp[j]=0;
        }
        if(active <= nproc&& pid>0){
            pipe(pipes[active-1]);
            pid = fork();
            //pids[active-1] = pid;
            current = active;
            active++;
        }

        if(pid==0){
            printf("\tcurrent = %d, i=%d, nproc=%d, active=%d, i-nproc+1=%d, pid=%d\n",current,i,nproc,active,i-nproc+1, getpid());
            if((current-1)== (i) || (i-nproc+1)==current){//debe o no debe
                temp[0]=i;
                for(j=0; j<ColumnasA; j++){
                    for(k=0; k<FilasB; k++){
                        temp[j+1]+=matriz2[k][i]*matriz1[j][k];
                    }
                }
                close(pipes[active-1][0]);
                write(pipes[active-1][1],temp,4*sizeof(int));

                printf("Resultado de la fila %d desde el hijo %d:\n",i,getpid());
                for(j=0; j<=ColumnasA; j++){
                    printf("|%d",temp[j]);
                }
                printf("|\n");
            }

        }else if(pid >0){
            //printf("El padre %d ejecuta en el indice %d\n",getpid(),i);
            for(j =0; j<nproc;j++){
                //while((wpid = wait(&status)) >0);
                close(pipes[j][1]);
                nbytes = read(pipes[j][0],temp,4*sizeof(int));
                printf("Received pipes:\n");
                for(j=0; j<ColumnasA; j++){
                    printf("%d,",temp[0]);
                    third[j][temp[0]] = temp[j+1];
                }
                printf("\n");
            }

            if(i==(FilasA-1)){
                printf("_________________________________________________\n");                
                for(j = 0; j<FilasB; j++){
                    for(k=0; k<ColumnasB; k++){
                        printf("|\t%d\t",matriz1[j][k]);
                    }
                    printf("|\n");
                }
                printf("_________________________________________________\n");
                for(j = 0; j<FilasA; j++){
                    for(k=0; k<ColumnasA; k++){
                        printf("|\t%d\t",matriz2[j][k]);
                    }
                    printf("|\n");
                }
                printf("_________________________________________________\n");
                for(j = 0; j<FilasA; j++){
                    for(k=0; k<ColumnasA; k++){
                        printf("|\t%d\t",third[j][k]);
                    }
                    printf("|\n");
                }
                printf("_________________________________________________\n");
                return 0;
            }
            while((wpid = wait(&status)) >0);
        }else{
            printf("Error en fork");
            exit(1);
        }
    }
    return 0;
}