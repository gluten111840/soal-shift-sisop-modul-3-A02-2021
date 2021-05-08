#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<ctype.h>
#include<dirent.h>
#include<sys/stat.h>

#define SIZE 255

pthread_t tid[3];

char pwd[SIZE];
char slashh[1] = "/";
char dot[1] = ".";

void* pindahin(void *);
void* buatfolder(void *);

int main(int argc, char *argv[])
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
            printf("%s\n", ep->d_name);
            strcpy(source, pwd);
            strcat(source, slashh);
            strcat(source, ep->d_name);
            printf("%s\n", source);
            // Under Unix, value 8 is a regular file and 4 is a directory
            if(ep->d_type == 8)
            {
                pthread_create(&(tid[index]), NULL, pindahin, (void*)source);
                pthread_join(tid[index], NULL);
                index++;
            }
        }
        closedir(dp);
    }
    for(int i=2;i<argc;i++)
    {
        char filename[SIZE];
        strcpy(filename, argv[i]);
        char *format;
        format = strchr(filename, '.');
        pthread_create(&tid[i], NULL, buatfolder, (void*)format+1);
        pthread_join(tid[i], NULL);
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
    for(int i=0;i<sizeof(temp_format);i++)
    {
        temp_format[i] = tolower(temp_format[i]);
    }

    char temp[SIZE];
    if(i>1)
    {
        if(dp == NULL || !dp)
            printf("Berhasil Dikategorikan\n");
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
            strcpy(temp, pwd);
            strcat(temp, "/");
            strcat(temp, temp_format);
            mkdir(temp, 0777);
            strcpy(dest, temp);
            strcpy(source, arg);
            rename(source, dest);
        }
    }
    else
    {
        strcpy(temp, pwd);
        strcat(temp, "/");
        strcat(temp, "Unknown");
        mkdir(temp, 0777);
        strcpy(dest, temp);
        strcpy(source, arg);
        rename(source, dest);
    }
    if(nama_file[0] == ".")
    {
        strcpy(temp, pwd);
        strcat(temp, "/");
        strcat(temp, "Hidden");
        mkdir(temp, 0777);
        strcpy(dest, temp);
        strcpy(source, arg);
        rename(source, dest);
    }
    
    return NULL;
}

void* buatfolder(void *format)
{
    char dest[SIZE] = "home/bayuekap/izone/";
    strcat(dest, format);
    int result = mkdir(dest, 0777);
}