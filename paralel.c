#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){
    //el primer elemento de la matriz va a ser el que indica en qué fila está
    int second[3][4] = {{0,10,16,5},{1,2,4,2},{2,14,17,2}};
    int first[4][3] = {{0,1,2},{19,16,18},{11,8,9},{8,9,5}};
    int third[3][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0}};

    int nproc = 3, i=0, status=0,j=0,k=0;
    int ffilas = 3, fcolumnas = 4;
    int sfilas = 4, scolumnas = 3;
    int pipefd[2],nbytes;
    char buf;
    
    for(i =0; i<nproc; i++){
        int temp[4] = {0,0,0,0};
        pipe(pipefd);
        pid_t pid = fork();
        if(pid==0){//codigo hijo     
            temp[0]= i;
            for(j=1; j<fcolumnas; j++){
                printf("-----------------\n");
                for(k=1; k<sfilas; k++){
                    printf("%d = %d*%d\n",temp[j],first[k-1][i+1],second[j][k-1]);
                    temp[j]+=first[k-1][i+1]*second[j][k-1];
                    printf("%d\n",temp[j]);
                    printf("____________\n");
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