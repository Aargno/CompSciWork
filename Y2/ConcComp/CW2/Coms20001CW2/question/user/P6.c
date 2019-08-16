#include "P6.h"

int mod(int n, int m) {
    if (n < m) while (n+m < m) n += m;
    else while (n >= m) n -= m;
    return n;
}

void main_P6() {
    int pipeFD[16][2]; //Not being copied between forked functions
    for (int i = 0; i < 16; i++) {
            int j = open(); //Gets latest free fd
            if (j == -1) exit(EXIT_SUCCESS); //If no free fd exit
            pipeFD[i][0] = j; //Set fd to free fd
            pipeFD[i][1] = j;
            pipe(pipeFD[i]); 
    }
    int index = 0; // (Using Chandy/Misra Solution, adapted from description at: https://en.wikipedia.org/wiki/Dining_philosophers_problem#Chandy/Misra_solution)
    for (int i = 0; i < 15; i++) {
        int r = fork();
        index = i;
        if (r == -1) exit(EXIT_FAILURE);
        if (r != 0) break;
        if (i == 14 && r == 0) index++;
    }
    int indSize = 1;
    if (index > 9) indSize = 2;
    char* m;
    char left_fork;
    char right_fork;
    char sig;
    switch (index) {
        case 0: //First process gets forks to left and right
            left_fork = 'd';
            right_fork = 'd';
            break;
            
        case 15: //Last process gets no forks
            left_fork = 'n';
            right_fork = 'n';
            break;
            
        default: //Other processes get fork to right
            left_fork = 'n';
            right_fork = 'd';
            break;
    }
    int ate = 0;
    int req[2] = {0};
    int x = 0;
    int r = 0;
    while (1) {
        if (left_fork != 'n' && right_fork != 'n' && ate == 0) {
            write(STDOUT_FILENO, "Philosopher: ", 13);
            itoa(m, index);
            write(STDOUT_FILENO, m, 2);
            write(STDOUT_FILENO, " ate\n", 5);
            left_fork = 'd';
            right_fork = 'd';
            ate = 1;
        }
        if (left_fork == 'n' && req[0] == 0) {
            write(pipeFD[mod(index-1, 16)][1], "rr", 2); //Request from right rr
            write(STDOUT_FILENO, "Philosopher: ", 13);
            itoa(m, index);
            write(STDOUT_FILENO, m, 2);
            write(STDOUT_FILENO, " requested left fork\n", 21);
            req[0] = 1;
        }
        if (right_fork == 'n' && req[1] == 0) {
            write(pipeFD[mod(index+1, 16)][1], "rl", 2); //Request from left rl
            write(STDOUT_FILENO, "Philosopher: ", 13);
            itoa(m, index);
            write(STDOUT_FILENO, m, 2);
            write(STDOUT_FILENO, " requested right fork\n", 22);
            req[1] = 1;
        }
        
        x = 0;
        r = 0;
        while (x < 1) {
            r = read(pipeFD[index][0], m, 2);
            if (r == -1) break;
            if (*m == 'r') { //Fork request handler
                *m++;
                if (*m == 'r') { //Request for right fork
                    *m--;
                    if (right_fork == 'd') { //If fork is dirty
                        write(pipeFD[mod(index+1, 16)][1], "gl", 2); //Give fork from left gl
                        write(STDOUT_FILENO, "Philosopher: ", 13);
                        itoa(m, index);
                        write(STDOUT_FILENO, m, 2);
                        write(STDOUT_FILENO, " gave right fork\n", 17);
                        right_fork = 'n';
                        ate = 0;
                        req[1] = 0;
                    } else {
                        write(pipeFD[mod(index+1, 16)][1], "nl", 2); //No from left nl
                        write(STDOUT_FILENO, "Philosopher: ", 13);
                        itoa(m, index);
                        write(STDOUT_FILENO, m, 2);
                        write(STDOUT_FILENO, " rejected request for right fork\n", 33);
                        itoa(m, index);
                    }
                } else if (*m == 'l') { //Request for left fork
                    *m--;
                    if (left_fork == 'd') { //If fork is dirty
                        write(pipeFD[mod(index-1, 16)][1], "gr", 2); //Give fork from right gr
                        write(STDOUT_FILENO, "Philosopher: ", 13);
                        itoa(m, index);
                        write(STDOUT_FILENO, m, 2);
                        write(STDOUT_FILENO, " gave left fork\n", 16);
                        left_fork = 'n';
                        ate = 0;
                        req[0] = 0;
                    } else {
                        write(pipeFD[mod(index-1, 16)][1], "nr", 2); //No from right nr
                        write(STDOUT_FILENO, "Philosopher: ", 13);
                        itoa(m, index);
                        write(STDOUT_FILENO, m, 2);
                        write(STDOUT_FILENO, " rejected request for left fork\n", 32);
                    }
                }
            } else if (*m == 'g') { //If we read a give request
                *m++;
                if(*m == 'r') { //Get right fork
                    *m--;
                    write(STDOUT_FILENO, "Philosopher: ", 13);
                    itoa(m, index);
                    write(STDOUT_FILENO, m, 2);
                    write(STDOUT_FILENO, " got right fork\n", 16);
                    right_fork = 'c';
                } else if (*m == 'l') { //Get left fork
                    *m--;
                    write(STDOUT_FILENO, "Philosopher: ", 13);
                    itoa(m, index);
                    write(STDOUT_FILENO, m, 2);
                    write(STDOUT_FILENO, " got left fork\n", 15);
                    left_fork = 'c';
                }
            } else if (*m == 'n') { //Rejection handler
                *m++;
                if (*m == 'r') { //Rejected right fork
                    *m--;
                    write(STDOUT_FILENO, "Philosopher: ", 13);
                    itoa(m, index);
                    write(STDOUT_FILENO, m, 2);
                    write(STDOUT_FILENO, " rejected from right\n", 21);
                    req[1] = 0;
                } else if (*m == 'l') { //Rejected left fork
                    *m--;
                    write(STDOUT_FILENO, "Philosopher: ", 13);
                    itoa(m, index);
                    write(STDOUT_FILENO, m, 2);
                    write(STDOUT_FILENO, " rejected from left\n", 20);
                    req[0] = 0;
                }
            } else if (*m == 'q') {
                close(pipeFD[index][1]);
                exit(EXIT_SUCCESS);
            }
            x++;
        }
        read(-1, &sig, 1);
        if (index == 0 && sig == 't') {
            for (int i = 0; i < 16; i++) {
                write(pipeFD[i][1], "qq", 2);
            }
            close(pipeFD[index][1]);
            exit(EXIT_SUCCESS);
        }
    }
//         itoa(m, index);
//         write(STDOUT_FILENO, m, 2);
//         write(STDOUT_FILENO, &left_fork, 1);
//         write(STDOUT_FILENO, &right_fork, 1);
//         write(STDOUT_FILENO, "\n", 1);
    exit(EXIT_SUCCESS);
}