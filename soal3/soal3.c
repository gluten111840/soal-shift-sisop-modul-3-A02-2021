#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<dirent.h>
#include<sys/stat.h>

#define SIZE 1000

pthread_t tid[3];

char pwd[SIZE];
char destination[SIZE] = "/home/bayuekap/izone/";

void* pindahin(void *);

int main(int argc, char **argv)
{
    char format_file[SIZE];
    int i = 0;
    int j = 0;
    int index = 0;

    if(strcmp(argv[1], "-f") == 0)
    {
        for(i=2;i<argc;i++)
        {
            pthread_create(&(tid[j]), NULL, pindahin, argv[i]);
            pthread_join(tid[j], NULL);
            j++;
        }
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        pthread_create(&(tid[index]), NULL, pindahin, argv[2]);
        pthread_join(tid[index], NULL);
        index++;
                
    }
    else if(strcmp(argv[1], "*") == 0)
    {
        int index = 0;
        DIR *dp;
        dp = opendir(pwd);
        struct dirent *ep;
        char source[SIZE];
        // if(dp != NULL)
        // {
        while(ep = readdir(dp))
        {
            if (!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, ".."));
            // printf("%s\n", ep->d_name);
            strcpy(source, pwd);
            strcat(source, "/");
            strcat(source, ep->d_name);
            // printf("%s\n", source);
            // Under Unix, value 8 is a regular file and 4 is a directory
            if(ep->d_type == 8)
            {
                pthread_create(&(tid[index]), NULL, pindahin, source);
                pthread_join(tid[index], NULL);
                index++;
            }
        }
    }
}

void* pindahin(void *arg)
{
    DIR *dp;
    struct dirent *ep;
    dp = opendir(pwd);
    int i = 0;
    int j = 0;
    char *nama, *ext;
    char *ekstensi[10], *nama_file[100];
    char temp_format[SIZE], temp_nama[SIZE];
    int flag = 0;
    char source[SIZE], dest[SIZE];
    nama = strtok(arg, "/");
    while(nama != NULL)
    {
        nama_file[i] = nama;
        nama = strtok(NULL, "/");
        i++;
    }
    strcpy(temp_nama, nama_file[i-1]);
    ext = strtok(nama_file[i-1], ".");
    while(ext != NULL)
    {
        ekstensi[j] = ext;
        ext = strtok(NULL, ".");
        j++;
    }
    strcpy(temp_format, ekstensi[j-1]);
    for(int a=0;a<sizeof(temp_format);a++)
    {
        temp_format[a] = tolower(temp_format[a]);
    }

    char temp[SIZE];
    if(i>1)
    {
        if(dp == NULL || !dp)
            printf("Sad, gagal :(\n");
        while(ep = readdir(dp))
        {
            if(strcmp(ep->d_name, temp_format) == 0 && ep->d_type == 4)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            strcpy(temp, destination);
            strcat(temp, "/");
            strcat(temp, temp_format);
            mkdir(temp, 0777);
        }
    }
    else if(i<1)
    {
        strcpy(temp, destination);
        strcat(temp, "/");
        strcat(temp, "Unknown");
        mkdir(temp, 0777);
    }
    else
    {
        strcpy(temp, destination);
        strcat(temp, "/");
        strcat(temp, "Hidden");
        mkdir(temp, 0777);
    }
    strcpy(source, arg);
    strcpy(dest, destination);
    if(i == 1)
        strcat(dest, "Unknown");
    else
        strcat(dest, temp_format);
    if(nama_file[0] == ".")
        strcat(dest, "Hidden");
    rename(source, dest);
    printf("Berhasil dikategorikan\n");
    i = 0, j = 0;
    return NULL;
}