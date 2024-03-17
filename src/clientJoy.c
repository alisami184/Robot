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
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

struct mesg {
    int label;
    float position[6];
  
};

#define ERROR (-1)
#define LABELS 5
#define FLAG 666

int main (int nba, char *arg[]) {
    int result;
    int nsend;
    int nconnect;
    struct mesg message;
    int addr;
    long int Te;

    struct sockaddr_in sockAddr, sock;
    int serveur, longaddr , results, resultr, resultr_s , results_s;

    //creation de la socket 
    serveur=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    sockAddr.sin_family=PF_INET;
    sockAddr.sin_port=htons(4000); 
    sockAddr.sin_addr.s_addr=inet_addr(arg[1]);
    addr=sizeof(sockAddr);

    struct timeb  taux;
    double temps_avant, temps_apres;
   
    //calcul le  q
    float q=0.0;
    //envoi le q
    message.label=0;
    message.position[1]=0.0;
    message.position[2]=0.0;
    message.position[3]=0.0;
    message.position[4]=0.0;
    message.position[5]=0.0;



    float t=0;
    Te=10000; // Te=10ms
    int timeOutInMs=5000;       // connection time-out in milliseconds (for the first connection)
    int commThreadCycleInMs=5;  // indicate how often data packets are sent back and forth - a default value of 5 is recommended
    int labelEnvoyer =1;
    fcntl(serveur,F_SETFL,fcntl(serveur,F_GETFL) | O_NONBLOCK); 

    double deltat=0.0;
    int tab[LABELS];
    double retard[LABELS];
    int index = 0;
    int compteur = 0,temporaire=0;
    int j=0;
    
    do
    {
        usleep(Te);

        //message.label++;
        message.position[0]=q;
        message.position[1]=q;
        message.position[2]=q;
        message.position[3]=q;
        message.position[4]=q;
        message.position[5]=q;

        

         

        //envoie au retard
        if (compteur <LABELS)
        {
            message.label= temporaire;
            tab[j]=message.label;
            results=sendto(serveur,&message,sizeof(message),0,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
            compteur ++ ;
                if(results != -1)
        printf("\n Consigne Client : label=%d rs=%d \n",message.label, results);
        printf("\n Envoyer des consignes number = %d \n", compteur);
        }

        //lancer le chrono
        ftime(&taux); 
         temps_avant = (1000.*taux.time + taux.millitm);
         retard[j] = temps_avant;
        
            
        //recevoir du serveur
        resultr=recvfrom(serveur,&message,sizeof(message), 0,(struct sockaddr*)&sockAddr,&addr);
        if(resultr != -1)
        printf("\n jai recu du retard : : label=%d  rr= %d \n",message.label,resultr);
        //si on recoit le return du serveur
        if (resultr != -1 && message.label == tab[index])
        {
            
             ftime(&taux); 
             temps_apres = 1000. * taux.time + taux.millitm;
             printf("\n\n\t\t\t\t\tregarde label: %d et messqge: %d RETARD: %.f\n" ,message.label,tab[index], temps_apres-retard[index]);
             //temps_avant = temps_apres; //enregistre le temp de apres comment nouveau debut
             //labelEnvoyer++;
             index ++;
 
        }
        //Message pour regarder qui tous marche bien
        //printf("\n client : \n  label=%d rr=%d rs=%d \n",message.label,resultr, results );
        t=t+0.001;
        //calculer le position
        //q=3*sinf(2*8*3.14/2.5*t);
        q=1;
        j++;
        temporaire++;
        //dernier recu on envoyer le flag
        if (index==LABELS-1)
        {
             message.label= FLAG;
            results=sendto(serveur,&message,sizeof(message),0,(struct sockaddr*)&sockAddr,sizeof(sockAddr));

        }

    }
    while(message.label != FLAG);

    close(serveur);

    return 0;

}



