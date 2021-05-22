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

void *pindahin(void *);
void *pindahin(void *arg);
void *pindahindf(void *);
void star(int argc, char *argv[]);
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
        // minde(argc, argv);
        bikin_thread = pthread_create(&tid[2], NULL, pindahindf, (void *)argv[2]);
        if(bikin_thread != 0)
            printf("Yah, gagal disimpan :(\n");
        else
            printf("Direktori sukses disimpan!\n");
        pthread_join(tid[2], NULL);
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

void *pindahind(void *arg) {
    char *path = (char *) arg;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(path);
    strcpy(folder, path);
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
        bikin_thread = pthread_create(&tid[i], NULL, pindahin, ep->d_name);
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
    printf("\e[31mTEST\n\n%s\n\n%s\n\n\e[0m", path, akhir);
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
        printf("\e[32m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

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
        printf("\e[33m\n\n%s\n\n%s\n\n\e[0m", curFile->filename, curFile->path);

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