#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO1 "fifo1"  // Tube pour recevoir les questions
#define FIFO2 "fifo2"  // Tube pour envoyer les réponses

// Structure pour la question envoyée par le client
typedef struct {
    pid_t client_pid;  // PID du client
    int nombres_demande;  // Nombre de nombres aléatoires demandés
} Question;

// Structure pour la réponse envoyée au client
typedef struct {
    pid_t client_pid;  // PID du client
    int nombres[100];  // Tableau pour les nombres générés
    int count;  // Nombre de nombres générés
} Reponse;

// Fonction pour gérer l'arrivée d'un signal SIGUSR1
void hand_reveil(int sig) {
    // Le serveur ne fait rien ici, mais cela permet de réagir au signal
}

// Fonction pour gérer la fin du serveur
void fin_serveur(int sig) {
    printf("Serveur terminé suite à un signal.\n");
    exit(0);  // Arrêter le serveur proprement
}

int main() {
    // Installation des gestionnaires de signaux
    signal(SIGUSR1, hand_reveil);
    signal(SIGINT, fin_serveur);

    // Créer les tubes nommés (FIFO)
    if (mkfifo(FIFO1, 0666) == -1) {
        perror("Erreur lors de la création de FIFO1");
        exit(1);
    }
    if (mkfifo(FIFO2, 0666) == -1) {
        perror("Erreur lors de la création de FIFO2");
        exit(1);
    }

    printf("Serveur en attente de questions...\n");

    while (1) {
        int fifo1_fd = open(FIFO1, O_RDONLY);  // Ouvrir FIFO1 en lecture
        if (fifo1_fd == -1) {
            perror("Erreur lors de l'ouverture de FIFO1");
            continue;
        }

        Question question;
        if (read(fifo1_fd, &question, sizeof(Question)) == -1) {
            perror("Erreur lors de la lecture de FIFO1");
            close(fifo1_fd);
            continue;
        }

        printf("Question reçue du client %d demandant %d nombres aléatoires.\n", 
               question.client_pid, question.nombres_demande);

        // Générer les nombres aléatoires
        Reponse reponse;
        reponse.client_pid = question.client_pid;
        reponse.count = question.nombres_demande;
        for (int i = 0; i < reponse.count; i++) {
            reponse.nombres[i] = rand() % 100;  // Nombre aléatoire entre 0 et 99
        }

        // Ouvrir FIFO2 en écriture et envoyer la réponse
        int fifo2_fd = open(FIFO2, O_WRONLY);
        if (fifo2_fd == -1) {
            perror("Erreur lors de l'ouverture de FIFO2");
            close(fifo1_fd);
            continue;
        }

        if (write(fifo2_fd, &reponse, sizeof(Reponse)) == -1) {
            perror("Erreur lors de l'écriture dans FIFO2");
        }

        close(fifo1_fd);
        close(fifo2_fd);

        // Envoyer un signal SIGUSR1 au client pour signaler la fin du traitement
        kill(question.client_pid, SIGUSR1);
        printf("Réponse envoyée au client %d.\n", question.client_pid);
    }

    return 0;
}

