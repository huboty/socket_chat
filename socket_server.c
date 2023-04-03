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

typedef struct
{
    char name[32];
    int client_fd;
} client_data;

typedef struct 
{
    client_data *table_data;
    int n_clients;
    int table_size;
} table_t;


void *end (void *arg)
{
    int sock_fd = *((int *) arg);
    while (1)
    {
        char command[MAX_SIZE]  = {};
        fgets (command, MAX_SIZE, stdin);
        if (!strcmp (command, "Quit"))
        {
            shutdown (sock_fd, SHUT_RDWR);
            close (sock_fd);
            exit (EXIT_SUCCESS);
        }
    }
};

void *func (void *arg)
{
    table_t *table  = (table_t *) arg;
    int n_self      = table->n_clients;
    int self_id     = table->table_data[n_self].client_fd;
    int mes_size    = 0;

    while (1)
    {
        char mes[MAX_SIZE]      = {};
        char name[NAME_SIZE]    = {};
        char format[32]         = {};
        sprintf (format, "%%%ds", NAME_SIZE);
        mes_size                = recv (self_id, (void *) mes, MAX_SIZE, 0);
        sscanf (mes, "%s", name);

        if (!strcmp (name, "Disconnect"))
        {
            table->table_data[n_self].name[0] = '\0';
            close (table->table_data[table->n_clients].client_fd);
            return;
        }
        else if (!strcmp (name, "All"))
        {
            int i = 0;
            int n = table->n_clients;
            for (int number = 0; number < n; i++)
            {
                if (table->table_data[i].name[0] != '\0')
                {
                    number++;
                    sprintf (mes, format, name);
                    send (table->table_data[i].client_fd, (void *) mes, strlen (mes), 0);
                }
            }
        }
        else
        {
            int i = 0;
            int number = 0;
            int n = table->n_clients;
            for (number = 0; number < n; i++)
            {
                if (table->table_data[i].name[0] != '\0')
                {
                    number++;
                    if (!strcmp (table->table_data[i].name, name))
                    {
                        sprintf (mes, format, name);
                        send (table->table_data[i].client_fd, (void *) mes, strlen (mes), 0);
                        break;
                    }
                }
            }
            if (number == n)
            {
                send (self_id, (void *) "No user found", 14, 0);
            }
        }
    }
};

int main ()
{
    int sock_fd  = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
        perror ("Error1");
    struct sockaddr_in serv_addr, cl_addr;
    
    table_t table       = {};
    table.n_clients     = 0;
    table.table_size    = TABLE_SIZE; 
    table.table_data    = (client_data *) calloc (table.table_size, sizeof (client_data));

    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_port          = htons (3000);
    serv_addr.sin_addr.s_addr   = htonl (INADDR_LOOPBACK);
    if (bind (sock_fd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) == -1)
        perror ("Error2");

    int struct_len  = 0;
    int client_fd   = 0;
    pthread_t thr   = 0;
    
    pthread_create (&thr, (const pthread_attr_t *) NULL, end, (void *) &sock_fd);
    while (1)
    {
        if (listen (sock_fd, 1) == -1)
            perror ("Error3");
        struct_len          = sizeof (struct sockaddr_in);
        client_fd           = accept (sock_fd, (struct sockaddr *) &cl_addr, &struct_len);

        char name[NAME_SIZE] = {};
        recv (client_fd, (void *) table.table_data[table.n_clients].name, NAME_SIZE, 0);
        table.table_data[table.n_clients].client_fd = client_fd;
        table.n_clients++;

        pthread_create (&thr, (const pthread_attr_t *) NULL, func, (void *) &table);

        if (table.table_size <= table.n_clients)
        {
            table.table_data    = (client_data *) realloc (table.table_data, table.table_size + TABLE_SIZE);
            table.table_size    += TABLE_SIZE;
        }

        send (table.table_data[j].client_fd, (void *) "Active clients", 15, 0);
        for (int i = 0, m = 0; m < table.n_clients; i++)
        {
            if (table.table_data[i].name[0])
            {
                m++;
                int n = table.n_clients;
                for (int number = 0, j = 0; number < n; j++)
                {
                    if (table.table_data[j].name[0])
                    {
                        char mes[MAX_SIZE]      = {};
                        number++;
                        sprintf (mes, "%s\n", table.table_data[i].name);
                        send (table.table_data[j].client_fd, (void *) mes, strlen (mes), 0);
                    }
                }
            }
        }
    }

    return 0;
}