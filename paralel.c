#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){

    int nproc = 2, i=0, j=0,k=0, active=1, current = 0, status=0;
    int FilasA = 3, ColumnasA = 4;
    int FilasB = 4, ColumnasB = 3;
    int pipefd[2],nbytes;
    pid_t pid=1,wpid;
    srand(time(NULL));
    //el primer elemento de la matriz va a ser el que indica en qué fila está
    //Alocacion e inicializacion
    int **matriz2 = (int **) malloc(FilasA*sizeof(int *));
    for(i =0; i<FilasA;i++ ){
        matriz2[i] = (int *) malloc(ColumnasA*sizeof(int));
    }
    int * pids = (int *) malloc(nproc * sizeof(pid_t));
    for(i = 0; i<FilasA; i++){
        matriz2[i][0] = i;
        for(j=1; j<ColumnasA; j++){
            int nntmp= rand()%20;
            matriz2[i][j]=nntmp;
        }
    }
    int **matriz1 = (int **) malloc(FilasB*sizeof(int *));
    for(i =0; i<FilasB;i++ ){
        matriz1[i] = (int *) malloc(ColumnasB*sizeof(int));
    }
    for(j = 0; j<ColumnasB; j++){
        matriz1[0][j] = j;
        for(i=1; i<FilasB; i++){
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
    //primer ciclo para numero de procesos **en caso de que sean igual al numero de filas
    for(i =0; i<FilasA; i++){
        int *temp = malloc(ColumnasA * sizeof(int));
        for(j=0; j<ColumnasA; j++){
            temp[j]=0;
        }
        if(active <= nproc&& pid>0){
            pipe(pipefd);
            pid = fork();
            pids[active-1] = pid;
            current = active;
            active++;
        }
        if(pid==0){//codigo hijo
        /**************************************************************/
            printf("\t%d %% %d = %d igual %d %% %d = %d desde %d\n",(i),nproc,(i)%nproc,(current),nproc,(current)%nproc,getpid());
            if(((i)%nproc)!=((current)%nproc)){//asignación//
        /**************************************************************/
                temp[0]= i;
                for(j=1; j<ColumnasA; j++){
                    for(k=1; k<FilasB; k++){
                        temp[j]+=matriz2[k-1][i+1]*matriz1[j][k-1];
                    }
                }
                close(pipefd[0]);
                write(pipefd[1],temp,4*sizeof(int));
                printf("\tavienta un pipe de i=%d desde %d\n",temp[0],getpid());
            }
            //exit(0);
        }else if(pid>0){//codigo padre
            //while((wpid = wait(&status)) >0);            
            close(pipefd[1]);
            nbytes = read(pipefd[0],temp,4*sizeof(int));
            third[temp[0]][0] = temp[0];
            for(j=1; j<ColumnasA; j++){
                third[j-1][temp[0]+1] = temp[j];
            }
            if(i==(FilasA-1)){
                int o = i;
                printf("_________________________________________________\n");                
                for(i = 1; i<FilasB; i++){
                    for(j=0; j<ColumnasB; j++){
                        printf("|\t%d\t",matriz1[i][j]);
                    }
                    printf("|\n");
                }
                printf("_________________________________________________\n");
                for(i = 0; i<FilasA; i++){
                    for(j=1; j<ColumnasA; j++){
                        printf("|\t%d\t",matriz2[i][j]);
                    }
                    printf("|\n");
                }
                printf("_________________________________________________\n");
                for(i = 0; i<FilasA; i++){
                    for(j=1; j<ColumnasA; j++){
                        printf("|\t%d\t",third[i][j]);
                    }
                    printf("|\n");
                }
                printf("_________________________________________________\n");
                i = o;
            for(j=0 ; j< nproc; j++){
                waitpid(pids[j],NULL,0);
            }
            return 0;
            }
        }else{
            printf("Error");
            exit(1);
        }
    }
    exit(0);    
}