#ifndef HANDLERS_SERV_H
#define HANDLERS_SERV_H

#include "serv_cli_fifo.h"

void hand_reveil(int sig) {
    printf("Signal SIGUSR1 reçu. Réponse prête à lire...\n");
}

void fin_serveur(int sig) {
    printf("Serveur terminé proprement suite à un signal.\n");
    unlink(FIFO1);
    unlink(FIFO2);
    exit(0);
}

void setup_signal_handlers() {
    signal(SIGUSR1, hand_reveil);  // Handler pour SIGUSR1
    signal(SIGINT, fin_serveur);   // Handler pour SIGINT (Ctrl+C)
}

#endif

