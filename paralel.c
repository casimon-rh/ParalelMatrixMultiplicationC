#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){

    int nproc = 3, i=0, status=0,j=0,k=0;
    int ffilas = 3, fcolumnas = 4;
    int sfilas = 4, scolumnas = 3;
    int pipefd[2],nbytes;
    char buf;
    srand(time(NULL));
    //el primer elemento de la matriz va a ser el que indica en qué fila está
    int **first = (int **) malloc(ffilas*sizeof(int *));
    for(i =0; i<ffilas;i++ ){
        first[i] = (int *) malloc(fcolumnas*sizeof(int));
    }
    for(i = 0; i<ffilas; i++){
        first[i][0] = i;
        for(j=1; j<fcolumnas; j++){
            first[i][j]=rand()%20;
        }
    }
    int **second = (int **) malloc(sfilas*sizeof(int *));
    for(i =0; i<sfilas;i++ ){
        second[i] = (int *) malloc(scolumnas*sizeof(int));
    }
    for(j = 0; j<scolumnas; j++){
        second[0][j] = j;
        for(i=1; i<sfilas; i++){
            second[i][j]=rand()%20;
        }
    }
    int **third = (int **) malloc(ffilas*sizeof(int *));
    for(i =0; i<ffilas;i++ ){
        third[i] = (int *) malloc(fcolumnas*sizeof(int));
    }
    for(i = 0; i<ffilas; i++){
        for(j=0; j<fcolumnas; j++){
            third[i][j]=0;
        }
    }
    
    for(i =0; i<nproc; i++){
        int *temp = malloc(fcolumnas * sizeof(int));
        for(j=0; j<fcolumnas; j++){
            temp[j]=0;
        }
        pipe(pipefd);
        pid_t pid = fork();
        if(pid==0){//codigo hijo     
            temp[0]= i;
            for(j=1; j<fcolumnas; j++){
                for(k=1; k<sfilas; k++){
                    temp[j]+=first[k-1][i+1]*second[j][k-1];
                }
            }
            close(pipefd[0]);
            write(pipefd[1],temp,4*sizeof(int));
            exit(0);
        }else if(pid>0){//codigo padre
            close(pipefd[1]);
            nbytes = read(pipefd[0],temp,4*sizeof(int));
            wait(NULL); 
            third[temp[0]][0] = temp[0];
            for(j=1; j<fcolumnas; j++){
                third[j-1][temp[0]+1] = temp[j];
            }
            if(i==(nproc-1)){
                for(i = 0; i<ffilas; i++){
                    for(j=0; j<fcolumnas; j++){
                        printf("|%d| ",first[i][j]);
                    }
                    printf("\n");
                }
                printf("\n");
                for(i = 0; i<sfilas; i++){
                    for(j=0; j<scolumnas; j++){
                        printf("|%d| ",second[i][j]);
                    }
                    printf("\n");
                }
                printf("\n");
                for(i = 0; i<ffilas; i++){
                    for(j=0; j<fcolumnas; j++){
                        printf("|\t%d\t|",third[i][j]);
                    }
                    printf("\n");
                }

            return 0;
            }
        }else{
            printf("Error");
            exit(1);
        }
    }    
}