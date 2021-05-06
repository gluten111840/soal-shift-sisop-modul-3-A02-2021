#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_CONNECTIONS 10
#define SIZE_BUF 100
#define FILE_SEND_BUF 1024

// Parameterless Functions

int create_tcp_server_socket();
void checkFile();

// Functions With Parameter(s)

int checkIdentity(int mode, char id[], char password[]);
void appendAkun(const char *id, const char *password);
void addCommand(int all_connections[], int serving);
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

void addFile(int client, char filename[]) {
    int ret_rec;
    char serverPath[100] = "/home/ananda/Documents/SoalShiftModul3/soal1/FILES/";
    char data[FILE_SEND_BUF];

    sprintf(serverPath, "%s%s", serverPath, filename);
    FILE *file = fopen(serverPath, "w");
    fclose(file);
    while(1) {
        if(recv(client, data, sizeof(data), 0) != -1) {
            if(!strcmp(data, "done")) return;
            
            file = fopen(serverPath, "a");
            fprintf(file, "%s", data);
            bzero(data, FILE_SEND_BUF);
            fclose(file);
        }
    }
}

// void commandHandler(int all_connections[], int i, int *userLoggedIn, char id[SIZE_BUF], char pass[SIZE_BUF], char cmd[SIZE_BUF]);

int main () {
    fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, serving = 1;
    int ret_val, ret_val1, ret_val2, ret_val3, status_val;
    char message[SIZE_BUF], id[SIZE_BUF], password[SIZE_BUF], cmd[SIZE_BUF];
    socklen_t addrlen;
    int all_connections[MAX_CONNECTIONS];

    // Make needed files if not found.
    checkFile();

    /* Get the socket server fd */
    server_fd = create_tcp_server_socket(); 
    if (server_fd == -1) {
        fprintf(stderr, "Failed to create a server\n");
        return -1; 
    }   

    /* Initialize all_connections and set the first entry to server fd */
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

	printf("\nServer is running....\n\n");
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
                    printf("New connection incoming:\n\n");
                    for (i=0;i < MAX_CONNECTIONS;i++) {
                        if (all_connections[i] < 0) {
                            all_connections[i] = new_fd;

                            // If the server is still serving a client, make other connections wait.
                            if(i != serving) {
                                ret_val1 = send(all_connections[i], "wait",  SIZE_BUF, 0);
                            } else {
                                ret_val1 = send(all_connections[i], "serve",  100, 0);
                            }
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "Accept failed [%s]\n", strerror(errno));
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
                    printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
                    printf("Command : %s\n", cmd);

                    // Check if client terminante
                    if (ret_val1 == 0) {
                        printf("Closing connection for fd : %d\n", all_connections[i]);
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
                                printf("\e[32mYou already logged in!\n");
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
                            }
                        } else if(!strcmp(cmd, "login")) {
                            if(userLoggedIn) {
                                printf("\e[32mYou already logged in!\n");
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
                            }
                        } else {
                            if(userLoggedIn) {
                                printf("Kamu berhak mengakses command\n");
                                if(!strcmp(cmd, "add")){
                                    addCommand(all_connections, serving);
                                }
                            } else {
                                status_val = send(all_connections[serving],
                                        "notlogin", SIZE_BUF, 0);
                                continue;
                                // ret_val3 = recv(all_connections[i], cmd, sizeof(cmd), 0);
                            }
                        }
                        
                        printf("ID of the user now : %s\n", id);
                        printf("Password of the user now : %s\n\n", password);
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

    return 0;
}

/*
Handler for the add command.
*/
void addCommand(int all_connections[], int serving) {
    char publisher[SIZE_BUF], tahun[SIZE_BUF], filepath[SIZE_BUF], filename[SIZE_BUF];
    int ret_pub, ret_year, ret_fp;
    ret_pub = recv(all_connections[serving], publisher, sizeof(publisher), 0);
    ret_year = recv(all_connections[serving], tahun, sizeof(tahun), 0);
    ret_fp = recv(all_connections[serving], filepath, sizeof(filepath), 0);

    getFileName(filepath, filename);
    printf("%s\n", filename);

    FILE *tsv = fopen("files.tsv", "a");
    fprintf(tsv, "%s\t%s\t%s\n", filepath, publisher, tahun);
    fclose(tsv);

    addFile(all_connections[serving], filename);
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
    while(fscanf(fp,"%s", temp) == 1){
        // Cek apakah id:password sudah ada.
        if(strstr(temp, akun)!=0) {
                return 1;
        }
    }
    fclose(fp);
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
    printf("Created a socket with fd: %d\n", fd);

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

    struct stat stats;
    stat("./FILES", &stats);

    if(!S_ISDIR(stats.st_mode))
        mkdir("./FILES", 0777);
}