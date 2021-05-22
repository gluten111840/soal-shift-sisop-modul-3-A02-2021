#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_CONNECTIONS 10
#define SIZE_BUF 100
#define FILE_SEND_BUF 1024
#define SERVERPATH "/home/ananda/Documents/soal-shift-sisop-modul-3-A02-2021/soal1/Server/FILES/"

// Parameterless Functions

int create_tcp_server_socket();
void checkFile();

// Functions With Parameter(s)

void addCommand(int client, char idpass[128]);
void downloadCommand(int client);
void deleteCommand(int client, char idpass[128]);
void seeCommand(int client);
void findCommand(int client);
void appendAkun(const char *id, const char *password);
int checkIdentity(int mode, char id[], char password[]);
int findFile(char filename[]);
int findLine(int *found, char filename[]);

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

void recvFile(int client, char filename[]) {
    int ret_rec;
    char fullpath[256];
    char data[FILE_SEND_BUF];

    sprintf(fullpath, "%s%s", SERVERPATH, filename);
    FILE *file = fopen(fullpath, "w+");
    fclose(file);
    while(1) {
        if(recv(client, data, sizeof(data), 0) != -1) {
            if(!strcmp(data, "done")) return;
            
            file = fopen(fullpath, "a");
            fprintf(file, "%s", data);
            fclose(file);
        }
        printf("\e[33%s\n\e[0m", data);
        fflush(stdout);
        bzero(data, FILE_SEND_BUF);
    }
}

// void commandHandler(int all_connections[], int i, int *userLoggedIn, char id[SIZE_BUF], char pass[SIZE_BUF], char cmd[SIZE_BUF]);

int main () {
    fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, serving = 1;
    int ret_val, ret_val1, ret_val2, ret_val3, status_val;
    char message[SIZE_BUF], id[SIZE_BUF], password[SIZE_BUF], cmd[SIZE_BUF];
    char idpass[256];
    socklen_t addrlen;
    int all_connections[MAX_CONNECTIONS];

    // Make needed files if not found.
    checkFile();

    /* Get the socket server fd */
    server_fd = create_tcp_server_socket(); 
    if (server_fd == -1) {
        fprintf(stderr, "[!] Failed to create a server\n");
        return -1; 
    }   

    /* Initialize all_connections and set the first entry to server fd */
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

	printf("\e[32m[!] Server is running....\n\n");
    int userLoggedIn = 0;
    while (1) {
        FD_ZERO(&read_fd_set);
        /* Set the fd_set before passing it to the select call */
        for (i=0;i < MAX_CONNECTIONS;i++) {
            if (all_connections[i] >= 0) {
                FD_SET(all_connections[i], &read_fd_set);
            }
        }

        /* Invoke select() and then wait! */
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events */
        if (ret_val >= 0 ) {
            /* Check if the fd with event is the server fd */
            if (FD_ISSET(server_fd, &read_fd_set)) { 
                /* accept the new connection */
                new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    printf("\e[32m[!] New connection incoming:\e[0m\n");
                    for (i=0;i < MAX_CONNECTIONS;i++) {
                        if (all_connections[i] < 0) {
                            all_connections[i] = new_fd;
                            printf("\e[32m    On Index: %d\e[0m\n\n", i);

                            // If the server is still serving a client, make other connections wait.
                            if(i != serving) {
                                ret_val1 = send(all_connections[i], "wait",  SIZE_BUF, 0);
                            } else {
                                ret_val1 = send(all_connections[i], "serve",  SIZE_BUF, 0);
                            }
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "\e[31m[!] Accept failed [%s]\e[0m\n", strerror(errno));
                }
                ret_val--;
                if (!ret_val) continue;
            } 

            /* Check if the fd with event is a non-server fd */
            // step2
            for (i=1;i < MAX_CONNECTIONS;i++) {
                if ((all_connections[i] > 0) &&
                    (FD_ISSET(all_connections[i], &read_fd_set))) {
                    // Receieve/read command from client.
                    ret_val1 = recv(all_connections[i], cmd, sizeof(cmd), 0);
                    printf("\e[1;37mServing client FD %d [index %d]\e[0m\n", all_connections[i], i);
                    printf("Command: \e[1;37m%s\e[0m\n", cmd);

                    // Check if client terminante
                    if (ret_val1 == 0) {
                        printf("\e[31mClosing connection for FD: %d\e[0m\n", all_connections[i]);
                        printf("ID:Password = %s:%s\n\n", id, password);
                        id[0] = 0;
                        password[0] = 0;
                        close(all_connections[i]);
                        all_connections[i] = -1;

                        // After a client terminates, find the next open connection.
                        // and serve for that client.
                        while(1) {
                            //If it reaches the end of max connection.
                            if(serving == MAX_CONNECTIONS - 1) {
                                serving = 1;
                                printf("Serving for index [%d]\n", serving);
                                break;
                            }

                            // If the next connection is open
                            if(all_connections[serving + 1] != -1) {
                                serving++;
                                printf("Serving for index [%d]\n", serving);
                                break;
                            }
                            serving++;
                        }
                        userLoggedIn = 0;
                        if(all_connections[serving] != -1)
                            status_val = send(all_connections[serving], "serve",  100, 0);
                    } 
                    if (ret_val1 > 0) {
                        
                        //Prioritize register and login.
                        if(!strcmp(cmd, "register")) {
                            if(userLoggedIn) {
                                continue;
                            }

                            // Waits for incoming message from client.
                            ret_val2 = recv(all_connections[i], id, sizeof(id), 0);
                            ret_val3 = recv(all_connections[i], password, sizeof(password), 0);
                            if(checkIdentity(0, id, password)) {
                                status_val = send(all_connections[serving],
                                        "userfound", SIZE_BUF, 0);
                            } else {
                                userLoggedIn = 1;
                                appendAkun(id, password);
                                status_val = send(all_connections[serving],
                                        "regloginsuccess", SIZE_BUF, 0);
                                sprintf(idpass, "%s:%s", id, password);
                            }
                        } else if(!strcmp(cmd, "login")) {
                            if(userLoggedIn) {
                                // printf("\e[32mYou already logged in!\n");
                                continue;
                            }

                            // Waits for incoming message from client.
                            ret_val2 = recv(all_connections[i], id, sizeof(id), 0);
                            ret_val3 = recv(all_connections[i], password, sizeof(password), 0);
                            if(!checkIdentity(1, id, password))
                                status_val = send(all_connections[serving],
                                        "wrongpass", SIZE_BUF, 0);
							else {
                                userLoggedIn = 1;
                                status_val = send(all_connections[serving],
                                        "regloginsuccess", SIZE_BUF, 0);
                                sprintf(idpass, "%s:%s", id, password);
                            }
                        } else {
                            if(userLoggedIn) {
                                if(!strcmp(cmd, "add")){
                                    addCommand(all_connections[serving], idpass);
                                }
                                if(!strcmp(cmd, "download")){
                                    downloadCommand(all_connections[serving]);
                                }
                                if(!strcmp(cmd, "delete")) {
                                    deleteCommand(all_connections[serving], idpass);
                                }
                                if(!strcmp(cmd, "see")) {
                                    seeCommand(all_connections[serving]);
                                }
                                if(!strcmp(cmd, "find")) {
                                    findCommand(all_connections[serving]);
                                }

                            } else {
                                status_val = send(all_connections[serving],
                                        "notlogin", SIZE_BUF, 0);
                                continue;
                                // ret_val3 = recv(all_connections[i], cmd, sizeof(cmd), 0);
                            }
                        }
                        
                        printf("ID of the user now: %s\n", id);
                        printf("Password of the user now: %s\n\n", password);
                    }
                    if (ret_val1 == -1 || ret_val2 == -1 || ret_val3 == -1) {
                        printf("recv() failed for fd: %d [%s]\n",
                            all_connections[i], strerror(errno));
                        break;
                    }
                }
                ret_val1--;
                if (!ret_val1) continue;
                
            }
		}
    }

    /* Last step: Close all the sockets */
    for(i=0;i < MAX_CONNECTIONS;i++) {
        if (all_connections[i] > 0) {
            close(all_connections[i]);
        }
    }

    // ;
    return 0;
}

/*
Handler for the add command.
*/
void addCommand(int client, char idpass[128]) {
    char publisher[SIZE_BUF], tahun[SIZE_BUF],
         filepath[SIZE_BUF], filename[SIZE_BUF],
         message[SIZE_BUF];
    int ret_pub, ret_year, ret_fp, ret_stat;
    ret_pub = recv(client, publisher, sizeof(publisher), 0);
    ret_year = recv(client, tahun, sizeof(tahun), 0);
    ret_fp = recv(client, filepath, sizeof(filepath), 0);
    int ret_rec;
    char fullpath[256];
    char data[FILE_SEND_BUF];

    getFileName(filepath, filename);
    sprintf(fullpath, "%s%s", SERVERPATH, filename);

    FILE *tsv = fopen("files.tsv", "a");
    fprintf(tsv, "%s\t%s\t%s\n", fullpath, publisher, tahun);
    fclose(tsv);

    // recvFile(client, filename);
    FILE *file = fopen(fullpath, "w+");
    while(1) {
        ret_rec = recv(client, data, FILE_SEND_BUF, 0);
        printf("Data Begin -%s- Data End\n", data);
        fflush(stdout);
        if(ret_rec != -1) {
            if(!strcmp(data, "done")) {
                break;
            }
        }
        fprintf(file, "%s", data);
        bzero(data, FILE_SEND_BUF);
    }
    fclose(file);

    FILE *log = fopen("running.log", "a");
    fprintf(log, "Tambah : %s (%s)\n", filename, idpass);
    fclose(log);
}

void downloadCommand(int client) {
    char fullpath[256], filename[128];

    int ret_fp;
    ret_fp = recv(client, filename, sizeof(filename), 0);

    sprintf(fullpath, "%s%s", SERVERPATH, filename);
    // printf("%s\n", fullpath);

    if(findFile(filename)) {
        FILE *book = fopen(fullpath, "r");
        char data[FILE_SEND_BUF];

        while(fgets(data, FILE_SEND_BUF, book) != NULL) {
            // printf("\e[35m[Sending]\e[33m %s\e[0m", data);
            if(send(client, data, sizeof(data), 0) != -1) {
                bzero(data, FILE_SEND_BUF);
            }
        }
        fclose(book);
        printf("\e[32mFile sent!\e[0m\n");
        send(client, "done", FILE_SEND_BUF, 0);
    } else {
        send(client, "err404", FILE_SEND_BUF, 0);
    }
}

void deleteCommand(int client, char idpass[128]) {
    int ret_client, found = 0;
    char filename[128], newPath[256], oldpath[256];

    ret_client = recv(client, filename, sizeof(filename), 0);

    findLine(&found, filename);
    if(found) {
        ret_client = send(client, "done", SIZE_BUF, 0);
        sprintf(newPath, "%sold-%s", SERVERPATH, filename);
        sprintf(oldpath, "%s%s", SERVERPATH, filename);
        rename(oldpath, newPath);
    } else {
        ret_client = send(client, "notfound", SIZE_BUF, 0);
    }

    FILE *log = fopen("running.log", "a");
    fprintf(log, "Hapus : %s (%s)\n", filename, idpass);
    fclose(log);
}

void seeCommand(int client) {
    FILE *book = fopen("files.tsv", "r");
    char data[FILE_SEND_BUF], file[64], filename[64], pub[64],
         tahun[64], eks[64], filepath[256];
    char temp[FILE_SEND_BUF];

    int i = 0;
    char *p;
    while(fgets(data, FILE_SEND_BUF, book) != NULL) {
        if(i != 0) {
            strcpy(filepath, strtok_r(data, "\t", &p));
            strcpy(pub, strtok_r(NULL, "\t", &p));
            strcpy(tahun, strtok_r(NULL, "\t", &p));
            tahun[strlen(tahun)-1] = '\0';

            getFileName(filepath, file);

            strcpy(filename, strtok_r(file, ".", &p));
            strcpy(eks, strtok_r(NULL, ".", &p));

            bzero(data, FILE_SEND_BUF);

            sprintf(temp, "Nama: %s\n", filename);
            strcat(data, temp);
            sprintf(temp, "Publisher: %s\n", pub);
            strcat(data, temp);
            sprintf(temp, "Tahun Publishing: %s\n", tahun);
            strcat(data, temp);
            sprintf(temp, "Ekstensi File: %s\n", eks);
            strcat(data, temp);
            sprintf(temp, "Filepath: %s%s.%s\n", filepath, file, eks);
            strcat(data, temp);
            
            send(client, data, sizeof(data), 0);
            
            int j;
        }
        i++;
        bzero(data, sizeof(data));
    }
    send(client, "done", sizeof("done"), 0);

    fclose(book);
}

void findCommand(int client) {
    FILE *book = fopen("files.tsv", "r");
    char data[FILE_SEND_BUF], file[64], filename[64], pub[64],
         tahun[64], eks[64], filepath[256];

    char temp[FILE_SEND_BUF], fileToFind[FILE_SEND_BUF];

    int client_val;

    client_val = recv(client, fileToFind, FILE_SEND_BUF, 0);

    int i = 0;
    char *p;
    while(fgets(data, FILE_SEND_BUF, book) != NULL) {
        if(i != 0) {
            strcpy(filepath, strtok_r(data, "\t", &p));
            strcpy(pub, strtok_r(NULL, "\t", &p));
            strcpy(tahun, strtok_r(NULL, "\t", &p));
            tahun[strlen(tahun)-1] = '\0';

            getFileName(filepath, file);

            strcpy(filename, strtok_r(file, ".", &p));
            strcpy(eks, strtok_r(NULL, ".", &p));

            bzero(data, FILE_SEND_BUF);

            if(strstr(filename, fileToFind) != NULL) {
                sprintf(temp, "Nama: %s\n", filename);
                strcat(data, temp);
                sprintf(temp, "Publisher: %s\n", pub);
                strcat(data, temp);
                sprintf(temp, "Tahun Publishing: %s\n", tahun);
                strcat(data, temp);
                sprintf(temp, "Ekstensi File: %s\n", eks);
                strcat(data, temp);
                sprintf(temp, "Filepath: %s%s.%s\n", filepath, file, eks);
                strcat(data, temp);
                
                send(client, data, sizeof(data), 0);
            }
            
            int j;
        }
        i++;
        bzero(data, sizeof(data));
    }
    send(client, "done", sizeof("done"), 0);

    fclose(book);
}

/*
Check if id and password exists in akun.txt
Mode 0 for register, 1 for login.
Returns 1 if it does, else returns 0.
*/
int checkIdentity(int mode, char id[], char password[]){
    char akun[100], temp[100];
    FILE *fp = fopen("akun.txt", "r");
    if(mode)
        sprintf(akun, "%s:%s", id, password);
    else
        sprintf(akun, "%s:", id);

    // Loop per line
    while(fscanf(fp, "%s", temp) == 1){
        // Cek apakah id:password sudah ada.
        if(strstr(temp, akun)!=0) {
                fclose(fp);
                return 1;
        }
    }
    fclose(fp);
    return 0;
}

int findFile(char filename[]) {
    FILE *tsv = fopen("files.tsv", "r");
    char temp[256];
    
    while(fscanf(tsv,"%s", temp) == 1){
        // Cek apakah id:password sudah ada.
        if(strstr(temp, filename)!=0) {
            fclose(tsv);
            return 1;
        }
    }
    fclose(tsv);
    return 0;
}

int findLine(int *found, char filename[]) {
    FILE *tsv = fopen("files.tsv", "r+");
    FILE *tmp = fopen("temp.tsv", "w+");
    char temp[256], line[256], tsvPath[256], tempPath[256];
    
    while(fgets(line, 256, tsv) != 0){
        // Cek apakah id:password sudah ada.
        if(sscanf(line, "%255[^\n]", temp) != 1) break;
        if(strstr(temp, filename) != 0) {
            *found = 1;
        } else {
            fprintf(tmp, "%s\n", temp);
        }
    }
    remove("files.tsv");
    rename("temp.tsv", "files.tsv");

    fclose(tmp);
    fclose(tsv);
    return 0;
}

/*
Add id and password to akun.txt with format "id:password".
*/
void appendAkun(const char *id, const char *password) {
    FILE *app = fopen("akun.txt", "a+");
    fprintf(app, "%s:%s\n", id, password);
    fclose(app);
}

int create_tcp_server_socket() {
    struct sockaddr_in saddr;
    int fd, ret_val;

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "Socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("\e[32mCreated a socket with fd: %d\e[0m\n", fd);

    /* Initialize the socket address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    saddr.sin_addr.s_addr = INADDR_ANY; 

    /* Step2: Bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "Bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: Listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "Listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

/*
Check if file is exists, and make it if it doesn't.
*/
void checkFile() {
    if(access("akun.txt", F_OK )) {
		FILE *fp = fopen("akun.txt", "w+");
		fclose(fp);
	} 
    if(access("files.tsv", F_OK )) {
		FILE *fp = fopen("files.tsv", "w+");
        fprintf(fp, "FilePath\tPublisher\tTahun Publikasi\n");
		fclose(fp);
	}
    if(access("running.log", F_OK )) {
		FILE *fp = fopen("running.log", "w+");
		fclose(fp);
	}

    struct stat stats;
    stat("./FILES", &stats);

    if(!S_ISDIR(stats.st_mode))
        mkdir("./FILES", 0777);
}