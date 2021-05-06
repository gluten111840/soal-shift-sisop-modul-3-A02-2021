#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define ROW 4
#define COL 6

typedef long long ll;

struct cellArgs {
    ll a;
    ll b;
    ll *res;
};


ll factorial(int x, int lowerBound) {
    if(x == lowerBound + 1) return lowerBound + 1;
    return x * factorial(x-1, lowerBound);
}

void *calculate(void *args) {
    struct cellArgs *cellVar = args;
    if(cellVar->a == 0 || cellVar->b == 0) {
        *cellVar->res = 0;
    } else if(cellVar->a >= cellVar->b) {
        *cellVar->res = factorial(cellVar->a, cellVar->a - cellVar->b);
    }  else {
        *cellVar->res = factorial(cellVar->a, 0);
    }
    free(cellVar);
    pthread_exit(NULL);
    return NULL;
}

void printMatrix(int row, int col, ll matrix[row][col]) {
    printf("\e[36m");
    printf("\n");
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            printf("%lld ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\e[0m");
}

int main() {
    pthread_t celCalculate[ROW*COL];

    key_t kunci = 6969;
    void *mem;
    ll matB[ROW][COL], result[ROW][COL];

    int shmid = shmget(kunci, 512, IPC_CREAT | 0666);
    mem = shmat(shmid, NULL, 0);

    ll (*matA)[COL] = mem;
    
    while(matA[5][0] == 0) {
        printf("\e[31mWaiting...\e[0m\n");
        sleep(1);
    }

    printMatrix(ROW, COL, matA);

    printf("Please insert matrix B (4x6):\n");
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            scanf("%lld", &matB[i][j]);
        }
    }

    struct cellArgs *cellVar;

    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            cellVar = (struct cellArgs *)malloc(sizeof(struct cellArgs));
            cellVar->a = matA[i][j];
            cellVar->b = matB[i][j];
            cellVar->res = &result[i][j];
            
            pthread_create(&celCalculate[i*4+j], NULL, &calculate, (void *)cellVar);
        }
    }

    for(int i = 0; i < ROW*COL; i++) {
        pthread_join(celCalculate[i], NULL);
    }

    printMatrix(ROW, COL, result);

    shmdt(mem);
    shmctl(shmid, IPC_RMID, NULL);
}