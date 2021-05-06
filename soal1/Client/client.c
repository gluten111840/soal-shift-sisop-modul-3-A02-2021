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

int handleRegLog(int fd, char cmd[]);
void recieveInput(const char *title, char str[]);
void printWarn(const char *msg);
void addBook(int fd);

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
        while(!commandTrue) {
            recieveInput("Insert Register/Login", cmd);
            int b=0;
            for(b=0;b<strlen(cmd);b++){
                cmd[b] = tolower(cmd[b]);
            }
            ret_val = send(fd, cmd, sizeof(cmd), 0);
            if(!strcmp(cmd, "register") || !strcmp(cmd, "login")) {
                if(handleRegLog(fd, cmd))
                    commandTrue = 1;
            } else {
                ret_val = recv(fd, message, SIZE_BUF, 0);
                if(!strcmp(message, "notlogin")) {
                    printf("\e[31mAccess restricted!\nPlease register/login.\n");
                } else {
                    commandTrue = 1;
                }
            }
        }

        // other command
        while(1){
            recieveInput("Insert Command (add/delete/see/find)", cmd);
            ret_val = send(fd, cmd, sizeof(cmd), 0);
            if(!strcmp(cmd, "add")){
                addBook(fd);
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
    ret_val = send(fd, id, sizeof(id), 0);

    recieveInput("Enter Password", password);
    ret_val = send(fd, password, sizeof(id), 0);

    ret_val = recv(fd, message, SIZE_BUF, 0);
    puts(message);

    //check if its terminate condition
    if(!strcmp(message, "regloginsuccess"))

        return 1;
    else if(!strcmp(message, "userfound")) {
        printWarn("Username or ID already exist!");
        return 0;
    }
    else if(!strcmp(message, "wrongpass")) {
        printWarn("ID or Password doesn't match!");
        return 0;
    }
	
}

void addBook(int fd){
    int ret_val;
    // int maxBit = 1024;
    char publisher[SIZE_BUF], tahun[SIZE_BUF], filepath[SIZE_BUF];
    
    recieveInput("Publisher", publisher);
    ret_val = send(fd, publisher, sizeof(publisher), 0);

    recieveInput("Tahun Publikasi", tahun);
    ret_val = send(fd, tahun, sizeof(tahun), 0);

    recieveInput("Filepath", filepath);
    ret_val = send(fd, filepath, sizeof(filepath), 0);

    FILE *book = fopen(filepath, "r");
    char data[FILE_SEND_BUF] = {0};

    while(fgets(data, FILE_SEND_BUF, book) != NULL) {
        if(send(fd, data, sizeof(data), 0) != -1) {
            bzero(data, FILE_SEND_BUF);
        }
    }
    fclose(book);
    send(fd, "done", FILE_SEND_BUF, 0);
}

void recieveInput(const char *title, char str[]) {
    printf("\e[0m%s\n> \e[36m", title);
    scanf("%s", str);
    printf("\e[0m");
}

void printWarn(const char *msg) {
    printf("\e[31m%s\e[0m\n", msg);
}
