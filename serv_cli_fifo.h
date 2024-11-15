#ifndef SERV_CLI_FIFO_H
#define SERV_CLI_FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO1 "fifo1"        // Tube nommé pour recevoir les questions
#define FIFO2 "fifo2"        // Tube nommé pour envoyer les réponses
#define NMAX 100             // Nombre maximum de nombres aléatoires

// Structure représentant une question du client
typedef struct {
    int client_num;          // Numéro du client (PID)
    int nombres_demande;     // Nombre de nombres demandés
} Question;

// Structure représentant une réponse du serveur
typedef struct {
    int client_num;          // Numéro du client (PID)
    int count;               // Nombre de nombres générés
    int numbers[NMAX];       // Les nombres générés
} Reponse;

#endif

