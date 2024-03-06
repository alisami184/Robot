#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

struct mesg {
    int label;
    float position[6];

};

int error;

#define ERROR(-1)
#define LABELS 200

int main(int nba, char * arg[]) {
    printf("main retqrd");
    struct mesg messageclient, messageServeur, msg_clientStock;
    struct mesg msg_client[LABELS];
    int result, nsend;
    struct sockaddr_in sockAddrClient, sockAddrServeur, sock;
    int serveurduClient, clientduServeur, client, nConnect, longaddrduClient, longaddrduServeur;
    int n, i, resultsClient, resultrClient, resultsServeur, resultrServeur;
    long int Te = 10000; // Te=10ms
    double Un, Sn, Snp, K;
    double tau, dt, A, B;
    int Rt;
    bool first = true;

    //int msg_client[LABELS,LABELS];
    double retard[LABELS];
    int index = 0, kk = 0;
    int j = 0;

    struct timeb taux;
    double temps_avant, temps_apres;
    //creation de la socket pour le client 
    serveurduClient = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockAddrClient.sin_family = PF_INET;
    sockAddrClient.sin_port = htons(4000);
    sockAddrClient.sin_addr.s_addr = 0;
    longaddrduClient = sizeof(sockAddrClient);

    //creation de la socket POUR le server
    clientduServeur = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockAddrServeur.sin_family = PF_INET;
    sockAddrServeur.sin_port = htons(2000);
    sockAddrServeur.sin_addr.s_addr = inet_addr(arg[1]);
    longaddrduServeur = sizeof(sockAddrServeur);

    error = bind(serveurduClient, (struct sockaddr * ) & sockAddrClient, longaddrduClient);
    if (error == ERROR) {
        printf("\n erreur de bind du serveur du Client UDP!! \n");
        exit(1);
    }
    resultsClient = ERROR;
    resultrClient = ERROR;
    resultsServeur = ERROR;
    resultrServeur = 10;

    fcntl(clientduServeur, F_SETFL, fcntl(clientduServeur, F_GETFL) | O_NONBLOCK);
    fcntl(serveurduClient, F_SETFL, fcntl(serveurduClient, F_GETFL) | O_NONBLOCK);
    printf("la taille c'est %d\n ", sizeof(messageclient));

    do {

        usleep(Te);

        //recevoir du client les positions desirées
        resultrClient = recvfrom(serveurduClient, & messageclient, sizeof(messageclient), 0, (struct sockaddr * ) & sockAddrClient, (socklen_t * ) & longaddrduClient);
        //si on recoit les consignes du client 
        if (resultrClient != -1) {
            //lancer le chrono
            ftime( & taux);
            temps_avant = (1000. * taux.time + taux.millitm);
            retard[j] = temps_avant;

            //Stocker les msgs du client
            msg_client[j].label = messageclient.label;
            for (kk = 0; kk < 6; kk++)
                msg_client[j].position[kk] = messageclient.position[kk];
            j++;
            printf("\n jai recu du client : label=%d rr=%d rs=%d \n", messageclient.label, resultrClient, resultsClient);

            if (first) {

                printf("je rentre iciiii");
                resultsServeur = sendto(clientduServeur, & messageclient, sizeof(messageclient), 0, (struct sockaddr * ) & sockAddrServeur, sizeof(sockAddrServeur));
                printf("\n jai envoyer au serveur: : label=%d rr=%d rs=%d \n", messageclient.label, resultrServeur, resultsServeur);
                first = false;
                index++;
            }

        }
        // printf("dans label on a : %d, et dans msg_client on a : %d ",messageServeur.label, msg_client[index] );
        //recevoir du serveur les positions actuelles
        resultrServeur = recvfrom(clientduServeur, & messageServeur, sizeof(messageServeur), 0, (struct sockaddr * ) & sockAddrServeur, (socklen_t * ) & longaddrduServeur);

        //si on recoit les consignes du server 
        if (resultrServeur != -1) {
            printf("\n jai recu du serveur: : label=%d rr=%d rs=%d \n", messageServeur.label, resultrServeur, resultsServeur);

            ftime( & taux);
            temps_apres = 1000. * taux.time + taux.millitm;
            printf("\n\n\t\t\t\t\t\tregarde label: %d et mess: %d RETARD entre R et S: %.f\n", messageServeur.label, msg_client[index], temps_apres - retard[index]);
            //rempli le stock
            msg_clientStock.label = msg_client[index].label;
            for (kk = 0; kk < 6; kk++)
                msg_clientStock.position[kk] = msg_client[index].position[kk];
            //envoyer au client les postions
            resultsServeur = sendto(serveurduClient, & messageServeur, sizeof(messageServeur), 0, (struct sockaddr * ) & sockAddrClient, sizeof(sockAddrClient));
            printf("\n jai envoyé au client les postions : : label=%d rr=%d rs=%d \n", messageServeur.label, resultrClient, resultsClient);
            //envoier pour le serveur les position commander 
            resultsServeur = sendto(clientduServeur, & msg_clientStock, sizeof(msg_clientStock), 0, (struct sockaddr * ) & sockAddrServeur, sizeof(sockAddrServeur));
            printf("\n jai envoyer au serveur: : label=%d rr=%d rs=%d  index = %d\n", msg_clientStock.label, resultrServeur, resultsServeur, index);
            index++;
        }
        printf("\n\n\nfin dq boucle message enregistre: %d\n\n\n", msg_client[j].label);

    }
    while (messageclient.label < 100);
    close(serveurduClient);
    close(clientduServeur);

    return 0;

}