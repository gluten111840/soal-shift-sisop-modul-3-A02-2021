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
char another2[10000];

void *pindahin(void *);
void *pindahindf(void *);
void star(int argc, char *argv[]);
void findanother();

int main(int argc, char *argv[])
{
    getcwd(pwd, sizeof(pwd));
    memset(folder, '\0', sizeof(folder));
    memset(another, '\0', sizeof(another));
    memset(another2, '\0', sizeof(another2));
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

    char *nama_file = NULL;
    nama_file = strchr(path, '/');
    if(nama_file)
        nama_file++;
    else
        nama_file = path;

    char tujuan[10000];
    char akhir[10000];
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);
    if(strlen(another2) > 1)
    {
        char file_nama[10000];
        strcpy(file_nama, another2);
        strcat(file_nama, "/");
        strcat(file_nama, nama_file);
        strcat(tujuan, "/");
        strcat(tujuan, nama_file);
        rename(file_nama, tujuan);
    }
    else if(strlen(folder) > 1)
    {
        char file_nama[10000];
        strcpy(file_nama, folder);
        strcat(file_nama, "/");
        strcat(file_nama, nama_file);
        strcat(tujuan, "/");
        strcat(tujuan, nama_file);
        rename(file_nama, tujuan);
    }
    // else
    // {
        strcat(akhir, "/");
        strcat(akhir, nama_file);
        rename(path, akhir);
    // }
    return NULL;
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

    char *nama_file = NULL;
    nama_file = strchr(path, '/');
    if(nama_file)
        nama_file++;
    else
        nama_file = path;

    char tujuan[10000];
    char akhir[10000];
    strcpy(tujuan, pwd);
    strcat(tujuan, "/");
    strcat(tujuan, ext);
    strcpy(akhir, tujuan);
    mkdir(tujuan, S_IRWXU);

    strcat(akhir, "/");
    strcat(akhir, nama_file);
    rename(path, akhir);
    
    return NULL;
}


void star(int argc, char *argv[])
{
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

void findanother()
{
    DIR *dp;
    struct dirent *ep;
    int i  = 0;
    memset(another2, '\0', sizeof(another2));
    strcpy(another2, another);
    dp = opendir(another2);
    while((dp != NULL) && (ep = readdir(dp)))
    {
        if(ep->d_type == 4 && strcmp(ep->d_name, ".") != 0 &&
            strcmp(ep->d_name, "..") != 0)
        {
            strcat(another2, ep->d_name);
            strcat(another2, "/");
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