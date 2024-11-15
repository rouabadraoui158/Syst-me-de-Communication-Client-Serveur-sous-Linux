#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO1 "fifo1"  // Tube pour envoyer la requête
#define FIFO2 "fifo2"  // Tube pour recevoir la réponse

// Structure pour la question envoyée au serveur
typedef struct {
    pid_t client_pid;  // PID du client
    int nombres_demande;  // Nombre de nombres aléatoires demandés
} Question;

// Structure pour la réponse envoyée par le serveur
typedef struct {
    pid_t client_pid;  // PID du client
    int nombres[100];  // Tableau pour les nombres générés
    int count;  // Nombre de nombres générés
} Reponse;

// Variable globale pour détecter quand le client a reçu une réponse
int ready_to_read = 0;

// Fonction de gestion du signal SIGUSR1
void signal_handler(int sig) {
    ready_to_read = 1;  // Indiquer que le client peut lire la réponse
}

// Fonction pour générer un nombre aléatoire
int generate_random_request() {
    return (rand() % 100) + 1;  // Génère un nombre aléatoire entre 1 et 100
}

// Fonction pour lire la réponse du serveur
void read_response(int fifo2_fd) {
    Reponse reponse;
    if (read(fifo2_fd, &reponse, sizeof(Reponse)) == -1) {
        perror("Erreur lors de la lecture de FIFO2");
        exit(1);
    }

    printf("Réponse reçue du serveur : ");
    for (int i = 0; i < reponse.count; i++) {
        printf("%d ", reponse.nombres[i]);
    }
    printf("\n");
}

// Fonction pour envoyer un signal SIGUSR1 au serveur
void send_acknowledgment(pid_t server_pid) {
    if (kill(server_pid, SIGUSR1) == -1) {
        perror("Erreur lors de l'envoi du signal SIGUSR1 au serveur");
    } else {
        printf("Signal SIGUSR1 envoyé au serveur.\n");
    }
}

int main() {
    srand(time(NULL));  // Initialisation du générateur de nombres aléatoires

    // Installer le gestionnaire de signal pour SIGUSR1
    signal(SIGUSR1, signal_handler);

    pid_t server_pid = getpid();  // Le client utilise son propre PID pour l'exemple

    // Créer la question à envoyer au serveur
    Question question;
    question.client_pid = getpid();  // Utiliser le PID du client
    question.nombres_demande = generate_random_request();  // Nombre aléatoire de nombres demandés

    // Ouvrir FIFO1 en écriture
    int fifo1_fd = open(FIFO1, O_WRONLY);
    if (fifo1_fd == -1) {
        perror("Erreur lors de l'ouverture de FIFO1");
        exit(1);
    }

    // Envoi de la question au serveur via FIFO1
    if (write(fifo1_fd, &question, sizeof(Question)) == -1) {
        perror("Erreur lors de l'écriture dans FIFO1");
        exit(1);
    }
    close(fifo1_fd);  // Fermer FIFO1 après l'envoi de la question

    // Attente de la réponse du serveur dans FIFO2
    int fifo2_fd = open(FIFO2, O_RDONLY);
    if (fifo2_fd == -1) {
        perror("Erreur lors de l'ouverture de FIFO2");
        exit(1);
    }

    printf("En attente de la réponse du serveur...\n");

    // Attendre que le serveur envoie un signal SIGUSR1 pour indiquer que la réponse est prête
    while (!ready_to_read) {
        pause();  // Mettre le processus en pause jusqu'à ce qu'il reçoive un signal
    }

    // Lire la réponse du serveur
    read_response(fifo2_fd);  // Lire la réponse du serveur
    close(fifo2_fd);  // Fermer FIFO2 après la lecture

    // Envoi d'un signal au serveur pour confirmer la réception
    send_acknowledgment(server_pid);

    printf("Réponse reçue. Appuyez sur Entrée pour quitter...\n");
    getchar();  // Attente que l'utilisateur appuie sur Entrée avant de quitter

    return 0;
}

