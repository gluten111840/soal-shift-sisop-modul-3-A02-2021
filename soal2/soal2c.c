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