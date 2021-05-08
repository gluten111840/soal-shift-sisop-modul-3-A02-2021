#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define SIZE_BUF 100
#define FILE_SEND_BUF 1024
#define CLIENTPATH "/home/ananda/Documents/soal-shift-sisop-modul-3-A02-2021/soal1/Client/"

int handleRegLog(int fd, char cmd[]);
void recieveInput(const char *title, char str[]);
void printWarn(const char *msg);
void downloadBook(int fd);
void deleteBook(int fd);
void sendFile(int fd);
void seeBook(int fd);

int main () {
    struct sockaddr_in saddr;
    int fd, ret_val;
    struct hostent *local_host; /* need netdb.h for this */
    char message[SIZE_BUF],  cmd[SIZE_BUF];

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", hstrerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);
    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "Connect failed [%s]\n", hstrerror(errno));
        close(fd);
        return -1;
    }

    //TERIMA MSG SERVE / WAIT
    ret_val = recv(fd, message, SIZE_BUF, 0);
    // puts(message);
    while(strcmp(message, "wait") == 0) {
        printf("\e[31mServer is full!\e[0m\n");
        ret_val = recv(fd, message, SIZE_BUF, 0);
    }
    printf("\e[33mServer is now responding to you.\e[0m\n");
    int commandTrue = 0;
    while(1)
    {
        // sign up user
        int b=0;
        while(!commandTrue) {
            recieveInput("Insert Register/Login", cmd);
            for(b = 0; b < strlen(cmd); b++){
                cmd[b] = tolower(cmd[b]);
            }
            ret_val = send(fd, cmd, sizeof(cmd), 0);
            if(!strcmp(cmd, "register") || !strcmp(cmd, "login")) {
                if(handleRegLog(fd, cmd)) {
                    commandTrue = 1;
                    break;
                }
            } else {
                ret_val = recv(fd, message, SIZE_BUF, 0);
                if(!strcmp(message, "notlogin")) {
                    printf("\e[31mAccess restricted!\nPlease register/login.\e[0m\n");
                } else {
                    commandTrue = 1;
                }
            }
        }
        commandTrue = 0;
        while(!commandTrue){
            recieveInput("Insert Command\n(add/delete/see/find)", cmd);
            for(b = 0; b < strlen(cmd); b++){
                cmd[b] = tolower(cmd[b]);
            }
            ret_val = send(fd, cmd, sizeof(cmd), 0);
            if(!strcmp(cmd, "register") || !strcmp(cmd, "login")) {
                printf("\e[32mYou already Logged in.\e[0m\n");
            }

            if(!strcmp(cmd, "add")){
                sendFile(fd);
            }
            if(!strcmp(cmd, "download")){
                downloadBook(fd);
            }
            if(!strcmp(cmd, "delete")){
                deleteBook(fd);
            }
            if(!strcmp(cmd, "see")){
                seeBook(fd);
            }
        }

        sleep(2);
        if(commandTrue) break;
    }
    printf("\e[31mDisconnected from server.\e[0m\n\n");

    /* Last step: close the socket */
    close(fd);
    return 0;
}

int handleRegLog(int fd, char cmd[]){
    int ret_val, isFound = 0;
	char id[SIZE_BUF], password[SIZE_BUF], message[SIZE_BUF];
	
    recieveInput("Enter Username", id);
    recieveInput("Enter Password", password);

    ret_val = send(fd, id, sizeof(id), 0);
    ret_val = send(fd, password, sizeof(id), 0);

    ret_val = recv(fd, message, SIZE_BUF, 0);
    puts(message);

    //check if its terminate condition
    if(!strcmp(message, "regloginsuccess")){
        return 1;
    }
    else if(!strcmp(message, "userfound")) {
        printWarn("Username or ID already exist!");
        return 0;
    }
    else if(!strcmp(message, "wrongpass")) {
        printWarn("ID or Password doesn't match!");
        return 0;
    }
	
}

void sendFile(int fd){
    int ret_val;
    char publisher[SIZE_BUF], tahun[SIZE_BUF], filepath[SIZE_BUF];
    
    recieveInput("Publisher", publisher);
    recieveInput("Tahun Publikasi", tahun);
    recieveInput("Filepath", filepath);

    ret_val = send(fd, publisher, sizeof(publisher), 0);
    ret_val = send(fd, tahun, sizeof(tahun), 0);
    ret_val = send(fd, filepath, sizeof(filepath), 0);
    
    FILE *book = fopen(filepath, "r");
    char data[FILE_SEND_BUF];

    while(fgets(data, FILE_SEND_BUF, book) != NULL) {
        // printf("\e[35m[Sending]\e[33m %s\e[0m", data);
        if(send(fd, data, sizeof(data), 0) != -1) {
            bzero(data, FILE_SEND_BUF);
        }
    }
    fclose(book);
    printf("\e[32mFile sent!\e[0m\n");
    send(fd, "done", FILE_SEND_BUF, 0);
}

void recvFile(int server, char filename[]) {
    int ret_rec;
    char fullPath[100] = {0};
    char data[FILE_SEND_BUF];

    sprintf(fullPath, "%s%s", CLIENTPATH, filename);
    FILE *file = fopen(fullPath, "w");
    fclose(file);
    while(1) {
        file = fopen(fullPath, "a");
        if(recv(server, data, sizeof(data), 0) != -1) {
            if(!strcmp(data, "err404")) {
                printf("\e[31mFile not found!\e[0m\n");
                return;
            }
            if(!strcmp(data, "done")) {
                printf("\e[32mFile downloaded!\e[0m\n");
                return;
            }
            
            // printf("\e[35m[Recieved]\e[33m %s\e[0m", data);
            fprintf(file, "%s", data);
            bzero(data, FILE_SEND_BUF);
        }
        fclose(file);
    }
}

void downloadBook(int fd) {
    int ret_val;
    char filename[SIZE_BUF];

    recieveInput("Masukkan nama file", filename);

    ret_val = send(fd, filename, sizeof(filename), 0);

    recvFile(fd, filename);
}

void deleteBook(int fd) {
    int ret_val;
    char filename[SIZE_BUF], resp[SIZE_BUF];

    recieveInput("Masukkan nama file", filename);

    ret_val = send(fd, filename, sizeof(filename), 0);

    ret_val = recv(fd, resp, SIZE_BUF, 0);
    if(!strcmp(resp, "done")) {
        printf("\e[32mFile Deleted!\e[0m\n");
    }

    if(!strcmp(resp, "notfound")) {
        printf("\e[31mFile Not Found!\e[0m\n");
    }
}

void seeBook(int fd) {
    int ret_val, done = 0;

    // ret_val = send(fd, "test", sizeof("test"), 0);
    char filename[FILE_SEND_BUF], pub[FILE_SEND_BUF],
         tahun[FILE_SEND_BUF], eks[FILE_SEND_BUF], message[FILE_SEND_BUF], filepath[FILE_SEND_BUF];

    // ret_val = recv(fd, pub, FILE_SEND_BUF, 0);
    // printf("%s\n\n", pub);
    printf("Yihi");
    while(1){
        // ret_val = recv(fd, message, FILE_SEND_BUF, 0);
        ret_val = recv(fd, filename, FILE_SEND_BUF, 0);
        printf("Nama: %s", filename);
        ret_val = recv(fd, pub, FILE_SEND_BUF, 0);
        printf("Publikasi: %s", pub);
        ret_val = recv(fd, tahun, FILE_SEND_BUF, 0);
        printf("Tahun Publishing: %s", tahun);
        ret_val = recv(fd, eks, FILE_SEND_BUF, 0);
        printf("Ekstensi File: %s", eks);
        ret_val = recv(fd, filepath, FILE_SEND_BUF, 0);
        printf("Filepath: %s", filepath);
        // printf("Filepath: %s", message);
    }
}

void recieveInput(const char *title, char str[]) {
    printf("\e[0m%s\n> \e[36m", title);
    scanf("%s", str);
    str[strlen(str)] = '\0';
    printf("\e[0m");
}

void printWarn(const char *msg) {
    printf("\e[31m%s\e[0m\n", msg);
}
