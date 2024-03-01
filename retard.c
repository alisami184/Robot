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


struct mesg {
    int label;
    float position[6];
  
};
int error;

#define ERROR (-1)
#define LABELS 200


int main (int nba, char *arg[]) {
printf("main retqrd");
  struct mesg messageclient,messageServeur;
    int result, nsend;
    struct sockaddr_in sockAddrClient,sockAddrServeur, sock;
    int serveurduClient,clientduServeur, client, nConnect, longaddrduClient,longaddrduServeur;
    int n , i, resultsClient, resultrClient ,resultsServeur, resultrServeur ;
    long int  Te=10000; // Te=10ms
    double  Un,  Sn,  Snp , K;
    double tau, dt, A, B;
    int Rt;  


    int tab[LABELS];
    double retard[LABELS];
    int index = 0;
    int j=0;


    //creation de la socket pour le client 
    serveurduClient=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    sockAddrClient.sin_family=PF_INET;
    sockAddrClient.sin_port=htons(4000); 
    sockAddrClient.sin_addr.s_addr=0;
    longaddrduClient=sizeof(sockAddrClient);

    //creation de la socket POUR le server
    clientduServeur=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    sockAddrServeur.sin_family=PF_INET;
    sockAddrServeur.sin_port=htons(2000); 
    sockAddrServeur.sin_addr.s_addr=inet_addr(arg[1]);
    longaddrduServeur=sizeof(sockAddrServeur);

    error=bind(serveurduClient,(struct sockaddr*)&sockAddrClient,longaddrduClient);
    if(error==ERROR) {
        printf("\n erreur de bind du serveur du Client UDP!! \n");
        exit(1);
    }
    resultsClient=ERROR;resultrClient=ERROR;resultsServeur=ERROR;resultrServeur=10;

    fcntl(clientduServeur,F_SETFL,fcntl(clientduServeur,F_GETFL) | O_NONBLOCK); 
    fcntl(serveurduClient,F_SETFL,fcntl(serveurduClient,F_GETFL) | O_NONBLOCK); 
    printf("la taille c'est %d\n ",sizeof(messageclient));

    int init =0;
  do
  {

    usleep(Te); 

    //recevoir du client les positions desirées
    resultrClient=recvfrom(serveurduClient,&messageclient,sizeof(messageclient), 0,(struct sockaddr*)&sockAddrClient,(socklen_t*)&longaddrduClient);


    //si on recoit les consignes du server 
    if (resultrClient != -1 )
    {
      printf("\n jai recu du client : label=%d rr=%d rs=%d \n",messageclient.label,resultrClient, resultsClient);

      //Stocker les msgs du client
      tab[j]=messageclient.label;

      //n'envoie rien au serveur tant que ta pas recu l'ancien message
      if (resultrServeur != -1 && messageServeur.label == tab[index])
    {
      //envoier pour le serveur les position commander 
      resultsServeur=sendto(clientduServeur,&messageclient,sizeof(messageclient),0,(struct sockaddr*)&sockAddrServeur,sizeof(sockAddrServeur));
      printf("\n jai envoyer au serveur: : label=%d rr=%d rs=%d \n",messageclient.label,resultrServeur, resultsServeur );
      init = 1;
      index ++;
    }
    }
    //recevoir du serveur les positions actuelles
    resultrServeur=recvfrom(clientduServeur,&messageServeur,sizeof(messageServeur), 0,(struct sockaddr*)&sockAddrServeur,(socklen_t*)&longaddrduServeur);
    
    
    //si on recoit les consignes du server 
    if (resultrServeur != -1)
    {
      printf("\n jai recu du serveur: : label=%d rr=%d rs=%d \n",messageServeur.label,resultrServeur, resultsServeur );
        //envoyer au client les postions
        resultsServeur=sendto(serveurduClient,&messageServeur,sizeof(messageServeur),0,(struct sockaddr*)&sockAddrClient,sizeof(sockAddrClient));
        printf("\n jai envoyé au client les postions : : label=%d rr=%d rs=%d \n",messageServeur.label,resultrClient, resultsClient);
    }

    }
  while(messageclient.label<100);
  close(serveurduClient);
  close(clientduServeur);

  return 0;

}
