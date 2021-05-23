# Soal Shift SISOP modul 3 A02 2021

# Soal 2

Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:

## 2a

Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka)

## Jawaban

Pada soal ini, kita diminta untuk mengalikan matriks 4x3 dengan matriks 3x6, yang menghasilkan matriks 4x6 berdasarkan operasi matematika matriks. Untuk programnya, kita menginisialisasi dahulu matriks 4x3 (array 2 dimensi), lalu mengisinya dengan angka-angka, lalu menginisialisasi matriks 3x6 (array 2 dimensi), lalu mengisinya dengan angka. \
Setelah itu memanggil fungsi mulMatrix untuk mengalikan kedua matriks tersebut dan disimpan ke dalam variabel matC yang sebelumnya diinisialisasi dengan shared memory, yang shared memory ini digunakan untuk mentransfer output perkalian matriks ini menjadi input pada program soal 2b. Untuk mentransfer hasilnya tersebut, kita memasang flag sebagai penanda agar program soal 2b bisa menerima hasil inputan tersebut.

## Kode Program
```C
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef long long ll;

void mulMatrix(int rowA, int colA, ll matA[rowA][colA],
                int rowB, int colB, ll matB[rowB][colB],
                ll matC[rowA][colB]) {
    int temp = 0;
    for(int i = 0; i < rowA; i++) {
        for(int j = 0; j < colB; j++) {
            for(int k = 0; k < colA; k++) {
                matC[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
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

int main(int argc, char *argv[]) {
    key_t kunci = 6969;
    void *mem;
    ll matA[4][3], matB[3][6];

    int shmid = shmget(kunci, 512, IPC_CREAT | 0666);
    mem = shmat(shmid, NULL, 0);

    ll (*matC)[6] = mem;
    memset(matC, 0, sizeof(int));
    
    printf("Please insert matrix A (4x3):\n");
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            scanf("%lld", &matA[i][j]);
        }
    }

    printMatrix(4, 3, matA);

    printf("Please insert matrix B (3x6):\n");
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 6; j++) {
            scanf("%lld", &matB[i][j]);
        }
    }

    printMatrix(3, 6, matB);

    mulMatrix(4, 3, matA, 3, 6, matB, matC);

    printMatrix(4, 6, matC);

    // sleep(5);

    matC[5][0] = 1;

    shmdt(mem);
    shmctl(shmid, IPC_RMID, NULL);
}
```

## 2b

Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang a	da. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matri(dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel)

**Ketentuan** 
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```

## 2c

Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

**Kendala** \
Waktu pengerjaan berbarengan dengan ETS jadi tidak bisa maksimal. Dan soal no 1 terlalu sulit untuk selesai tepat waktu
