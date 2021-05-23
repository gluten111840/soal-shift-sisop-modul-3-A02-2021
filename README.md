# Soal Shift SISOP modul 3 A02 2021

# Soal 2

Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:

## 2a

Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka)

## Jawaban

Pada soal ini, kita diminta untuk mengalikan matriks 4x3 dengan matriks 3x6, yang menghasilkan matriks 4x6 berdasarkan operasi matematika matriks. Untuk programnya, kita menginisialisasi dahulu matriks 4x3 (array 2 dimensi), lalu mengisinya dengan angka-angka, lalu menginisialisasi matriks 3x6 (array 2 dimensi), lalu mengisinya dengan angka. 

```C
int main()
{
    ...
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
    ...
}
```
Setelah itu memanggil fungsi mulMatrix untuk mengalikan kedua matriks tersebut dan disimpan ke dalam variabel matC yang sebelumnya diinisialisasi dengan shared memory, yang shared memory ini digunakan untuk mentransfer output perkalian matriks ini menjadi input pada program soal 2b.
```C
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

int main()
{
    key_t kunci = 6969;
    void *mem;
    ll matA[4][3], matB[3][6];

    int shmid = shmget(kunci, 512, IPC_CREAT | 0666);
    mem = shmat(shmid, NULL, 0);

    ll (*matC)[6] = mem;
    memset(matC, 0, sizeof(int));
}
```
Untuk mentransfer hasilnya tersebut, kita memasang flag sebagai penanda agar program soal 2b bisa menerima hasil inputan tersebut.
```C
int main()
{
    ...
    matC[5][0] = 1;
    ...
}
```

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

Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matri(dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel)

**Ketentuan** 
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```

## Jawaban

Pada soal 2b ini, kita menginput hasil output pada soal 2a. Untuk melakukan hal tersebut, kita menggunakan shared memory. Lalu untuk mendapatkan input tersebut, maka program ini harus dijalankan bersamaan dengan program pada soal 2a. Jika program 2a masih belum mengeluarkan output, maka program soal 2b akan menunggu hingga variabel matriks yang diassign dengan shared memory memiliki flag = 1. Setelah itu, kita baru bisa mendapatkan inputnya dan menyimpannya ke dalam variabel matA.
```C
int main()
{
    ...
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
    ...
}
```
Setelah kita mendapatkan inputnya, kita menginputkan lagi matriks (matB) sebagai batas maksimal faktorialnya matriks matA. 
```C
int main()
{
    ...
    struct cellArgs *cellVar;

    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            cellVar = (struct cellArgs *) malloc(sizeof(struct cellArgs));
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
    ...
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
```
Misalkan pada cell (1,1) pada matriks matA adalah 5, dan cell (1,1) pada matriks matB adalah 3, maka hasil dari cell (1,1) program pada soal 2b ini adalah **5x4x3 = 60**. Tetapi, jika dimisalkan pada matriks matA pada cell (1,1) bernilai lebih besar atau sama dengan cell (1,1) pada matriks matB, maka hasil faktorialnya adalah (a!/(a-b)!), lalu jika pada matriks matA pada cell (1,1) bernilai lebih kecil daripada cell (1,1) pada matriks matB, maka hasil faktorialnya adalah a!, serta jika bernilai 0, maka hasilnya juga 0. 
***note***
- a = cell pada matA
- b = cell pada matB
```C
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
```
Semua progress di atas dijalankan dengan thread untuk setiap cell, sehingga tidak memperberat kinerja processor. Lalu haislnya disimpan pada variabel matC.

## Kode Program
```C
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
            cellVar = (struct cellArgs *) malloc(sizeof(struct cellArgs));
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
```

## 2c

Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

## Jawaban

## Kode Program
```C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define ROW 4
#define COL 6

int fd1[2];
int fd2[2];

int main() {
    pid_t pid;
    // create pipe1
    if (pipe(fd1) == -1) {
        perror("bad pipe1");
        exit(1);
    }

    // fork (ps aux)
    if ((pid = fork()) == -1) {
    perror("bad fork1");
    exit(1);
    } else if (pid == 0) {
        // stdin --> ps --> pipe1
        dup2(fd1[1], 1);
        // close fds
        close(fd1[0]);
        close(fd1[1]);
        // exec
        execlp("ps", "ps", "aux", NULL);
        // exec didn't work, exit
        perror("bad exec ps");
        exit(1);
    }
    // parent

    // create pipe2
    if (pipe(fd2) == -1) {
        perror("bad pipe2");
        exit(1);
    }

    // fork (grep root)
    if ((pid = fork()) == -1) {
        perror("bad fork2");
        exit(1);
    } else if (pid == 0) {
        // input from pipe1
        dup2(fd1[0], 0);
        // output to pipe2
        dup2(fd2[1], 1);
        // close fds
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        // exec
        execlp("sort", "sort", "-nrk", "3,3", NULL);
        // exec didn't work, exit
        perror("bad exec sort root");
        exit(1);
    }
    // parent

    // close unused fds// fflush(stdout);
    close(fd1[0]);
    close(fd1[1]);

    // fork (grep sbin)
    if ((pid = fork()) == -1) {
        perror("bad fork3");
        exit(1);
    } else if (pid == 0) {
        dup2(fd2[0], 0);
        // output to stdout (already done)
        // close fds
        close(fd2[0]);
        close(fd2[1]);
        // exec
        execlp("head", "head", "-5", NULL);
        // exec didn't work, exit
        perror("bad exec grep sbin");
        exit(1);
    }
    exit(1);
}
```

# Soal 3

Seorang mahasiswa bernama Alex sedang mengalami masa gabut. Di saat masa gabutnya, ia memikirkan untuk merapikan sejumlah file yang ada di laptopnya. Karena jumlah filenya terlalu banyak, Alex meminta saran ke Ayub. Ayub menyarankan untuk membuat sebuah program C agar file-file dapat dikategorikan. Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

## Catatan
- Kategori folder tidak dibuat secara manual, harus melalui program C
- Program ini tidak case sensitive. Contoh: JPG dan jpg adalah sama
- Jika ekstensi lebih dari satu (contoh “.tar.gz”) maka akan masuk ke folder dengan titik terdepan (contoh “tar.gz”)
- Dilarang juga menggunakan fork-exec dan system()
- Bagian b dan c berlaku rekursif

## 3a
Program menerima opsi -f seperti contoh di atas, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna. \
Output yang dikeluarkan adalah seperti ini :
```
File 1 : Berhasil Dikategorikan (jika berhasil)
File 2 : Sad, gagal :( (jika gagal)
File 3 : Berhasil Dikategorikan
```

## Jawaban
Pada soal ini, kita diminta untuk membuat sebuah program dengan input pada saat me-run file .exe (argv) dengan kode *-f*, lalu dilanjutkan dengan lokasi file yang ingin dikategorikan sebanyak mungkin. Untuk membuat program ini, kita menggunakan thread untuk memindahkan masing-masing file, sehingga memperringan pekerjaan. Pada fungsi main, kita mengcompare argv[1] dengan string "-f", lalu kita menge-loop proses create thread sebanyak berapa jumlah lokasi yang diinputkan.
```C
int main(int argc, char *argv[])
{
    ...
    if(strcmp(argv[1], "-f") == 0)
    {
        for(int i=2;i<argc;i++)
        {
            bikin_thread = pthread_create(&tid[i], NULL, pindahindf, (void *)argv[i]);
            if(bikin_thread != 0)
                printf("File %d : Sad, gagal :(\n", i-1);
            else
                printf("File %d : Berhasil Dikategorikan\n", i-1);
        }
        for(int i=2;i<argc;i++)
            pthread_join(tid[i], NULL);
    }
    ...
}
```
Lalu di setiap thread, kita membuat folder hasil pengategorian berdasarkan nama ekstensi file yang akan dikategorikan. Jika filenya itu terhidden, maka dengan cara berikut, akan ditemukan dan masuk ke kategori "Hidden". Kemudian jika filenya tidak memiliki format, akan masuk ke kategori "Unknown". Selain itu, untuk yang memiliki 2 format seperti *.tar.gz*, maka akan diambil yang terdepan hingga ke akhir ekstensi. Lalu dipindahkan ke tujuan dengan fungsi rename.
```C
void *pindahindf(void *arg)
{
    char *path = (char *) arg;
    char *ekstensi = NULL;
    char dot = '.';
    ekstensi = strchr(path, dot);
    char ext[10000];
    memset(ext, '\0', sizeof(ext));
    if((ekstensi-path-strlen(pwd)+1)==2 || 
        (ekstensi-path+1)==1)
        strcpy(ext, "Hidden");
    else if(ekstensi)
    {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");

    char nama_file[10000];
    
    getFileName(path, nama_file);

    char tujuan[10000];
    char akhir[10000];
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);

    strcat(path, nama_file);
    strcat(akhir, "/");
    strcat(akhir, nama_file);
    // printf("\e[31mTEST\n\n%s\n\n%s\n\n\e[0m", path, akhir);
    rename(path, akhir);
    
    return NULL;
}
```

## 3b
Program juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin. \
Contoh adalah seperti ini:
```
$ ./soal3 -d /path/to/directory/
```
Perintah di atas akan mengkategorikan file di /path/to/directory, lalu hasilnya akan disimpan di working directory dimana program C tersebut berjalan (hasil kategori filenya bukan di /path/to/directory).
Output yang dikeluarkan adalah seperti ini :
```
Jika berhasil, print “Direktori sukses disimpan!”
Jika gagal, print “Yah, gagal disimpan :(“
```

## Jawaban
Pada soal 3b ini, kita diminta untuk mengategorikan semua isi dari suatu folder yang diinputkan, namun kita hanya bisa menginputkan 1 folder saja. Kita meng-compare inputan dengan string "-d", lalu kita membuka directory pada inputan setelah -d tersebut dengan menggunakan DIR dan struct dirent, ketika sudah ketemu, maka kita mencari satu persatu semua file meskipun yang ada di dalam folder lagi. Jika kita sudah menemukannya, maka kita membuat folder untuk setiap ekstensi dan kita memindahkan semuanya ke dalam folder-folder yang telah dibuat dengan menggunakan thread.
```C
void de(int argc, char *argv[]) {
    struct fileType *curFile;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(argv[2]);
    strcpy(folder, argv[2]);
    int i = 0;
    while((dp != NULL) && (ep = readdir(dp)))
    {
        if(ep->d_type == 4 && 
            strcmp(ep->d_name, ".") != 0 &&
            strcmp(ep->d_name, "..") != 0)
        {
            strcpy(another, folder);
            strcat(another, "/");
            strcat(another, ep->d_name);
            strcat(another, "/");
            findanother();
        }
        if(strcmp(ep->d_name, ".") == 0 || 
            strcmp(ep->d_name, "..") == 0 ||
            strcmp(ep->d_name, "soal3.c") == 0 ||
            strcmp(ep->d_name, "soal3") == 0 ||
            ep->d_type == 4)
        {
            continue;
        }
        
        curFile = (struct fileType *) malloc(sizeof(struct fileType));
        strcpy(curFile->filename, ep->d_name);
        strcpy(curFile->path, folder);
        // printf("\e[32m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *) curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0;j<i;j++)
        pthread_join(tid[j], NULL);
    closedir(dp);
    return;
}

void *pindahin(void *arg)
{
    struct fileType *curFile = arg;
    char *ekstensi = NULL;
    char dot = '.';
    ekstensi = strchr(curFile->filename, dot);
    char ext[10000];
    memset(ext, '\0', sizeof(ext));

    if((ekstensi-curFile->filename-strlen(pwd)+1)==2 || (ekstensi-curFile->filename+1)==1) {
        strcpy(ext, "Hidden");
    }
    else if(ekstensi) {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");
    
    // getFileName(curFile->filename, curFile->filename);

    char tujuan[10000];
    char akhir[10000];
    // tujuan[0] = '\"';
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);
    
    if(strlen(curFile->path) > 1) {
        char file_nama[10000];
        strcpy(file_nama, curFile->path);
        strcat(file_nama, "/");
        strcat(file_nama, curFile->filename);
        strcat(tujuan, "/");
        strcat(tujuan, curFile->filename);
        rename(file_nama, tujuan);
    }
    else
    {
        strcat(akhir, "/");
        strcat(akhir, curFile->filename);
        rename(curFile->filename, akhir);
    }
    return NULL;
}
```

## 3c
Selain menerima opsi-opsi di atas, program ini menerima opsi *, contohnya ada di bawah ini:
```
$ ./soal3 \*
```
Opsi ini akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut.

## Jawaban
Pada soal 3c ini, kita mengategorikan semua file yang ada di current working directory selain file soal3c.c dan soal3c. Sama seperti soal 3b, kita hanya tinggal mengubah lokasi yang awalnya diambil dari inputan menjadi lokasi current working directory.
```C
void star(int argc, char *argv[])
{
    struct fileType *curFile;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(pwd);
    strcpy(folder, pwd);
    int i = 0;
    while((dp != NULL) && (ep = readdir(dp)))
    {
        if(ep->d_type == 4 && 
            strcmp(ep->d_name, ".") != 0 &&
            strcmp(ep->d_name, "..") != 0)
        {
            strcpy(another, folder);
            strcat(another, "/");
            strcat(another, ep->d_name);
            strcat(another, "/");
            findanother();
        }
        if(strcmp(ep->d_name, ".") == 0 || 
            strcmp(ep->d_name, "..") == 0 ||
            strcmp(ep->d_name, "soal3.c") == 0 ||
            strcmp(ep->d_name, "soal3") == 0 ||
            ep->d_type == 4)
        {
            continue;
        }
        
        curFile = (struct fileType *) malloc(sizeof(struct fileType));
        strcpy(curFile->filename, ep->d_name);
        strcpy(curFile->path, folder);
        // printf("\e[32m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *) curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0;j<i;j++)
        pthread_join(tid[j], NULL);
    closedir(dp);
    return;
}

void *pindahin(void *arg)
{
    struct fileType *curFile = arg;
    char *ekstensi = NULL;
    char dot = '.';
    ekstensi = strchr(curFile->filename, dot);
    char ext[10000];
    memset(ext, '\0', sizeof(ext));

    if((ekstensi-curFile->filename-strlen(pwd)+1)==2 || (ekstensi-curFile->filename+1)==1) {
        strcpy(ext, "Hidden");
    }
    else if(ekstensi) {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");
    
    // getFileName(curFile->filename, curFile->filename);

    char tujuan[10000];
    char akhir[10000];
    // tujuan[0] = '\"';
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);
    
    if(strlen(curFile->path) > 1) {
        char file_nama[10000];
        strcpy(file_nama, curFile->path);
        strcat(file_nama, "/");
        strcat(file_nama, curFile->filename);
        strcat(tujuan, "/");
        strcat(tujuan, curFile->filename);
        rename(file_nama, tujuan);
    }
    else
    {
        strcat(akhir, "/");
        strcat(akhir, curFile->filename);
        rename(curFile->filename, akhir);
    }
    return NULL;
}
```

## 3d
Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder “Hidden”.

```C
void *pindahin(void *arg)
{
    ...
    if((ekstensi-curFile->filename-strlen(pwd)+1)==2 || (ekstensi-curFile->filename+1)==1) {
        strcpy(ext, "Hidden");
    }
    else if(ekstensi) {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");
    ...
}

void *pindahindf(void *arg)
{
    ...
    if((ekstensi-curFile->filename-strlen(pwd)+1)==2 || (ekstensi-curFile->filename+1)==1) {
        strcpy(ext, "Hidden");
    }
    else if(ekstensi) {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");
    ...
}
```

## 3e
Setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat.

```C
void de(int argc, char *argv[]) {
    ...
    while((dp != NULL) && (ep = readdir(dp)))
    {
        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *) curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
}

void star(int argc, char *argv[]) {
    ...
    while((dp != NULL) && (ep = readdir(dp)))
    {
        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *) curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
}

int main(int argc, char *argv[])
{
    ...
    if(strcmp(argv[1], "-f") == 0)
    {
        for(int i=2;i<argc;i++)
        {
            bikin_thread = pthread_create(&tid[i], NULL, pindahindf, (void *)argv[i]);
            if(bikin_thread != 0)
                printf("File %d : Sad, gagal :(\n", i-1);
            else
                printf("File %d : Berhasil Dikategorikan\n", i-1);
        }
        for(int i=2;i<argc;i++)
            pthread_join(tid[i], NULL);
    }
    ...
}
```
## Kode Program
```C
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>

pthread_t tid[99999999];
int bikin_thread;
pid_t child;
char pwd[10000];
char folder[10000];
char another[10000];

void *pindahin(void *arg);
void *pindahindf(void *);
void star(int argc, char *argv[]);
void de(int argc, char *argv[]);
void findanother();

struct fileType {
    char filename[256];
    char path[1000];
};

char *strrev(char *str) {
    char *p1, *p2;

    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

void getFileName(char path[], char filename[]) {
    int i = strlen(path) - 1;
    int j = 0;
    // int len = strlen(path) - 1;
    // char temp[strlen(path)];
    // printf("\e[36m%s\n\n\e[0", path);
    while(i) {
        path[i+1] = '\0';
        if(path[i] == '/')
            break;
        filename[j] = path[i];
        i--;
        j++;
    }
    filename[j] = '\0';

    strrev(filename);
}

int main(int argc, char *argv[])
{
    getcwd(pwd, sizeof(pwd));
    memset(folder, '\0', sizeof(folder));
    memset(another, '\0', sizeof(another));
    if(strcmp(argv[1], "-f") == 0)
    {
        for(int i=2;i<argc;i++)
        {
            bikin_thread = pthread_create(&tid[i], NULL, pindahindf, (void *)argv[i]);
            if(bikin_thread != 0)
                printf("File %d : Sad, gagal :(\n", i-1);
            else
                printf("File %d : Berhasil Dikategorikan\n", i-1);
        }
        for(int i=2;i<argc;i++)
            pthread_join(tid[i], NULL);
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        de(argc, argv);
    }
    else if((argv[1][0] == '*') && (strlen(argv[1]) == 1))
        star(argc, argv);
}

void *pindahin(void *arg)
{
    struct fileType *curFile = arg;
    char *ekstensi = NULL;
    char dot = '.';
    ekstensi = strchr(curFile->filename, dot);
    char ext[10000];
    memset(ext, '\0', sizeof(ext));

    if((ekstensi-curFile->filename-strlen(pwd)+1)==2 || (ekstensi-curFile->filename+1)==1) {
        strcpy(ext, "Hidden");
    }
    else if(ekstensi) {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");
    
    // getFileName(curFile->filename, curFile->filename);

    char tujuan[10000];
    char akhir[10000];
    // tujuan[0] = '\"';
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);
    
    if(strlen(curFile->path) > 1) {
        char file_nama[10000];
        strcpy(file_nama, curFile->path);
        strcat(file_nama, "/");
        strcat(file_nama, curFile->filename);
        strcat(tujuan, "/");
        strcat(tujuan, curFile->filename);
        rename(file_nama, tujuan);
    }
    else
    {
        strcat(akhir, "/");
        strcat(akhir, curFile->filename);
        rename(curFile->filename, akhir);
    }
    return NULL;
}

void de(int argc, char *argv[]) {
    struct fileType *curFile;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(argv[2]);
    strcpy(folder, argv[2]);
    int i = 0;
    while((dp != NULL) && (ep = readdir(dp)))
    {
        if(ep->d_type == 4 && 
            strcmp(ep->d_name, ".") != 0 &&
            strcmp(ep->d_name, "..") != 0)
        {
            strcpy(another, folder);
            strcat(another, "/");
            strcat(another, ep->d_name);
            strcat(another, "/");
            findanother();
        }
        if(strcmp(ep->d_name, ".") == 0 || 
            strcmp(ep->d_name, "..") == 0 ||
            strcmp(ep->d_name, "soal3.c") == 0 ||
            strcmp(ep->d_name, "soal3") == 0 ||
            ep->d_type == 4)
        {
            continue;
        }
        
        curFile = (struct fileType *) malloc(sizeof(struct fileType));
        strcpy(curFile->filename, ep->d_name);
        strcpy(curFile->path, folder);
        // printf("\e[32m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *) curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0;j<i;j++)
        pthread_join(tid[j], NULL);
    closedir(dp);
    return;
}

void *pindahindf(void *arg)
{
    char *path = (char *) arg;
    char *ekstensi = NULL;
    char dot = '.';
    ekstensi = strchr(path, dot);
    char ext[10000];
    memset(ext, '\0', sizeof(ext));
    if((ekstensi-path-strlen(pwd)+1)==2 || 
        (ekstensi-path+1)==1)
        strcpy(ext, "Hidden");
    else if(ekstensi)
    {
        ekstensi++;
        for(int i=0;i<strlen(ekstensi);i++)
            ext[i] = tolower(ekstensi[i]);
    }
    else
        strcpy(ext, "Unknown");

    char nama_file[10000];
    
    getFileName(path, nama_file);

    char tujuan[10000];
    char akhir[10000];
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);

    strcat(path, nama_file);
    strcat(akhir, "/");
    strcat(akhir, nama_file);
    // printf("\e[31mTEST\n\n%s\n\n%s\n\n\e[0m", path, akhir);
    rename(path, akhir);
    
    return NULL;
}


void star(int argc, char *argv[])
{
    struct fileType *curFile;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(pwd);
    strcpy(folder, pwd);
    int i = 0;
    while((dp != NULL) && (ep = readdir(dp)))
    {
        if(ep->d_type == 4 && 
            strcmp(ep->d_name, ".") != 0 &&
            strcmp(ep->d_name, "..") != 0)
        {
            strcpy(another, folder);
            strcat(another, "/");
            strcat(another, ep->d_name);
            strcat(another, "/");
            findanother();
        }
        if(strcmp(ep->d_name, ".") == 0 || 
            strcmp(ep->d_name, "..") == 0 ||
            strcmp(ep->d_name, "soal3.c") == 0 ||
            strcmp(ep->d_name, "soal3") == 0 ||
            ep->d_type == 4)
        {
            continue;
        }
        
        curFile = (struct fileType *) malloc(sizeof(struct fileType));
        strcpy(curFile->filename, ep->d_name);
        strcpy(curFile->path, folder);
        // printf("\e[32m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *) curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0;j<i;j++)
        pthread_join(tid[j], NULL);
    closedir(dp);
    return;
}

void findanother()
{
    struct fileType *curFile;
    DIR *dp;
    struct dirent *ep;
    int i  = 0;
    strcpy(curFile->path, another);
    dp = opendir(curFile->path);
    while((dp != NULL) && (ep = readdir(dp)))
    {
        if(ep->d_type == 4 && strcmp(ep->d_name, ".") != 0 &&
            strcmp(ep->d_name, "..") != 0)
        {
            strcat(curFile->path, ep->d_name);
            strcat(curFile->path, "/");
            findanother();
        }

        if(strcmp(ep->d_name, ".") == 0 || 
            strcmp(ep->d_name, "..") == 0 ||
            strcmp(ep->d_name, "soal3.c") == 0 ||
            strcmp(ep->d_name, "soal3") == 0 ||
            ep->d_type == 4)
        {
            continue;
        }

        curFile = (struct fileType *) malloc(sizeof(struct fileType));
        strcpy(curFile->filename, ep->d_name);
        strcpy(curFile->path, folder);
        // printf("\e[33m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

        bikin_thread = pthread_create(&tid[i], NULL, pindahin, (void *)curFile);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0;j<i;j++)
        pthread_join(tid[j], NULL);
    closedir(dp);
    return;
}
```
**Kendala** \
Waktu pengerjaan berbarengan dengan ETS jadi tidak bisa maksimal. Dan soal no 1 terlalu sulit untuk selesai tepat waktu
