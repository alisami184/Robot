/* 
 * INTERMEDIARE CODE
 *
 * Autors: Ali, Gabriel, Laura, Louis
 *
 */

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

#define ERROR (-1)
#define LABELS 20
#define FLAG 666




int main(int nba, char * arg[]) {
    printf("main retard");
    struct mesg messageclient, messageServeur, msg_clientStock;
    struct mesg msg_client[LABELS];
    int result, nsend;
    struct sockaddr_in sockAddrClient, sockAddrServeur, sock;
    int serveurduClient, clientduServeur, client, nConnect, longaddrduClient, longaddrduServeur;
    int n, i, resultsClient, resultrClient, resultsServeur, resultrServeur;
    long int Te = 100000; // Te=100ms
    double Un, Sn, Snp, K;
    double tau, dt, A, B;
    int Rt;
    bool first = true;

    //int msg_client[LABELS,LABELS];
    double retard[LABELS];
    int index = 0, kk = 0;
    int j = 1,compteur=0;

    struct timeb taux;
    double temps_avant, temps_apres;
    //creation de la socket pour le client 
    serveurduClient = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockAddrClient.sin_family = PF_INET;
    sockAddrClient.sin_port = htons(4001);
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
    //printf("la taille c'est %d\n ", sizeof(messageclient));

    do {

        //On écoute le client pour les consignes
        resultrClient = recvfrom(serveurduClient, & messageclient, sizeof(messageclient), 0, (struct sockaddr * ) & sockAddrClient, (socklen_t * ) & longaddrduClient);
      
        //on recoit du client une consigne, on le stocke et on lance le chrono
        if (resultrClient != -1) {

            /*/lancer le chrono
            ftime( & taux);
            temps_avant = (1000. * taux.time + taux.millitm);

            
            retard[j] = temps_avant;   //On stock le temps init "0.0"
            */
            if ( msg_client[j-1].label !=messageclient.label )
            {
            msg_client[j].label = messageclient.label;   //Stocker les msgs du client , le label et la position
            for (kk = 0; kk < 6; kk++)
                msg_client[j].position[kk] = messageclient.position[kk];
            }
            j++;

            printf("\n RECEIVED Client : label=%d rr=%d rs=%d \n", messageclient.label, resultrClient, resultsClient);  //console


            //Attend le retard avant d'envoyer au serveur
            printf("\nEN Attente de %d...",Te);
            usleep(Te); 
           //envoier pour le serveur les position commander 
            resultsServeur = sendto(clientduServeur, & msg_client[index], sizeof(messageServeur), 0, (struct sockaddr * ) & sockAddrServeur, sizeof(sockAddrServeur));
            printf("\n\t\t\tSEND Server  :  label=%d rr=%d rs=%d  index = %d\n", msg_client[index].label, resultrServeur, resultsServeur, index);
            index++;



            //Pour la premiere commande client , on l'envoie directement au serveur
            if (first) 
            {
                //Attend le retard avant d'envoyer au serveur
                printf("\nEN Attente de %d...",Te*100);
                usleep(Te); 
                //on envoie au serveur 
                resultsServeur = sendto(clientduServeur, & messageclient, sizeof(messageclient), 0, (struct sockaddr * ) & sockAddrServeur, sizeof(sockAddrServeur));
                printf("\n\t\t\tSEND Serveur :  label=%d rr=%d rs=%d \n", messageclient.label, resultrServeur, resultsServeur);  //console
                first = false;
                index++;
            }
        }
     
        //On ecoute le serveur
        resultrServeur = recvfrom(clientduServeur, & messageServeur, sizeof(messageServeur), 0, (struct sockaddr * ) & sockAddrServeur, (socklen_t * ) & longaddrduServeur);

        //si on recoit les retours du serveur
        if (resultrServeur != -1 ) {

            printf("\n\t\t\tRECEIVED serveur : label=%d rr=%d rs=%d \n", messageServeur.label, resultrServeur, resultsServeur);   //console

            //envoyer au client les postions
            resultsClient = sendto(serveurduClient, & messageServeur, sizeof(messageServeur), 0, (struct sockaddr * ) & sockAddrClient, sizeof(sockAddrClient));
            printf("\n SEND Client  :  label=%d rr=%d rs=%d \n", messageServeur.label, resultrClient, resultsClient); //console
            
            
            /*Pour eviter de passer le meme consigne 2 fois
            while(msg_client[index].label == messageServeur.label) 
            {index++;
            printf("mesma mensagem-");
            }*/
            //Si il y a des command different envoyer les nouveau consignes
          
           
        }

    } //fim do while
   while (messageclient.label != FLAG);
    close(serveurduClient);
    close(clientduServeur);

    return 0;

}

