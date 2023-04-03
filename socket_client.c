#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>

#define MAX_SIZE 1000
#define NAME_SIZE 32
#define TABLE_SIZE 32

void *func (void *arg)
{
    while (1)
    {
        int sock_fd = *((int *) arg);
        char mes[MAX_SIZE]      = {};
        char name[NAME_SIZE]    = {};
        recv (sock_fd, mes, MAX_SIZE, 0);
        if (!strcmp (mes, "No user found"))
            puts ("No user found");
        else
        {
            sscanf (mes, "%s", name);
            printf ("%s ", name);
            printf (mes + NAME_SIZE + 1);
            putchar ('\n');
        }
    }
}

int main (int argc, char *argv[])
{
    if (argc != 2)
        exit (EXIT_FAILURE);
    int sock_fd  = socket (AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;    
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_port          = htons (3000);
    serv_addr.sin_addr.s_addr   = htonl (INADDR_LOOPBACK);

    if (connect (sock_fd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) == -1)
        perror ("Error1");
    send (sock_fd, (const void *) argv[1], sizeof (argv[1]), 0);

    pthread_t thr   = 0;
    pthread_create (&thr, (const pthread_attr_t *) NULL, func, (void *) &sock_fd);

    char format[32] = {};
    sprintf (format, "%%%ds", NAME_SIZE);

    while (1)
    {
        char mes[MAX_SIZE]      = {};
        char name[NAME_SIZE]    = {};
        char letter[MAX_SIZE]   = {};

        fgets (mes, MAX_SIZE, stdin);
        if (!strcmp (mes, "Disconnect"))
        {
            send (sock_fd, (const void *) "Disconnect", 11, 0);
            break;
        }

        sscanf (mes, "%s", name);
        sprintf (letter, format, name);
        strcat (letter, mes + strlen (name) + 1);
        send (sock_fd, (const void *) letter, sizeof (letter), 0);
    }

    shutdown (sock_fd, SHUT_RDWR);
    close (sock_fd);

    return 0;
}