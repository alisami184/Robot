//
//   C++ - VREP API for manipulator Robotis H
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

struct mesg {
    int label;
    float position[6];
  
};

#define ERROR (-1)


extern "C" {
 #include "extApi.h"
}

int handles[6],all_ok=1;
simxInt handle, error;


void GetHandles(int clientID){

    simxChar objectName[100];
    char str[10];
    for (int i=0; i < 6; i++) {
        strcpy(objectName, "joint");
        sprintf(str, "%d", i+1);
        strcat(objectName,str);
        error=simxGetObjectHandle(clientID, objectName, &handle, simx_opmode_oneshot_wait);
        if (error == simx_return_ok)
            handles[i]=handle;
        else {
            printf("Error in Object Handle - joint number %d\n", i);
            all_ok=0;
        }
    }
}

/////////////////////////////////////////////////////////
// Get the joint position
//
// Inputs:
// clientID
// qr : array of the getted joint values 
// Return: 0 if an error occurs in object handling, 1 otherwise
/////////////////////////////////////////////////////////
int GetJointPos(int clientID, float *qr){ 
 if (all_ok) {
 //Pause the communication thread
 simxPauseCommunication(clientID, 1);
 // Send the joint target positions
 for (int i=0; i < 6; i++)
 simxGetJointPosition(clientID, handles[i], &qr[i], simx_opmode_oneshot);

 //printf("QR0: %6.4f. QR1: %6.4f.QR2: %6.4f.QR3: %6.4f.QR4: %6.4f.QR5: %6.4f.\n", qr[0],qr[1],qr[2],qr[3], qr[4], qr[5]);
 // Resume the communication thread to update all values at the same time
 simxPauseCommunication(clientID, 0);
 return 1;
 }
 else
 return 0;
}

/////////////////////////////////////////////////////////
// Set the join position
//
// Inputs:
// clientID
// q : array of the joint values
// Return: 0 if an error occurs in object handling, 1 otherwise
/////////////////////////////////////////////////////////
int SetJointPos(int clientID, float *q)
{

 if (all_ok) {
 //Pause the communication thread
 simxPauseCommunication(clientID, 1);
 // Send the joint target positions
 for (int i=0; i < 6; i++)
 simxSetJointTargetPosition(clientID, handles[i], q[i], simx_opmode_oneshot);

 //printf("Q0: %6.4f. Q1: %6.4f.Q2: %6.4f.Q3: %6.4f.Q4: %6.4f.Q5: %6.4f.\n", q[0],q[1],q[2],q[3], q[4], q[5]);
 // Resume the communication thread to update all values at the same time
 simxPauseCommunication(clientID, 0);
 return 1;
 
 }
 else
 return 0;
}



int main(int argc,char* argv[])
{
    int portNb=5555; // the port number where to connect
    int timeOutInMs=5000; // connection time-out in milliseconds (for the first connection)
    int commThreadCycleInMs=5; // indicate how often data packets are sent back and forth - a default value of 5 is recommended
  

    // Connection to the server
    int clientID=simxStart((simxChar*)"162.38.40.128",portNb,true,true,timeOutInMs,commThreadCycleInMs);
   
    struct mesg message;
    int result, nsend;
    struct sockaddr_in sockAddr, sock;
    int serveur, client, nConnect, longaddr;
    int n , i, results, resultr ;
    long int  Te;
    double  Un,  Sn,  Snp , K;
    double tau, dt, A, B;
    int Rt;  

    serveur=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    sockAddr.sin_family=PF_INET;
    sockAddr.sin_port=htons(2000); 
    sockAddr.sin_addr.s_addr=0;
    longaddr=sizeof(sockAddr);

    error=bind(serveur,(struct sockaddr*)&sockAddr,longaddr);
    if(error==ERROR) {
        printf("\n erreur de bind du serveur UDP!! \n");
        exit(1);
    }
    
 

    results=ERROR;
    resultr=ERROR;

    fcntl(serveur,F_SETFL,fcntl(serveur,F_GETFL) | O_NONBLOCK); 
    //printf("la taille c'est %d\n",sizeof(message));


   GetHandles(clientID);
   
 if (clientID!=-1)
 {
   
 int nbloop=100;
 simxSynchronous(clientID,true); // Enable the synchronous mode (Blocking function call)
 simxStartSimulation(clientID, simx_opmode_oneshot);

       float t=0.0;
       float tfinal=5;
       float dt=0.01;

      
 int offsetTime=simxGetLastCmdTime(clientID)/1000;

 while (t < tfinal) {
 printf("Current time: %6.4f\n", t);
         
           
            //recevoir du client les positions desirées
            resultr=recvfrom(serveur,&message,sizeof(message), 0,(struct sockaddr*)&sockAddr,(socklen_t*)&longaddr);
            
            //printf("jai recu : \n rr=%d label=%d q0=%f\n",resultr,message.label,message.position[0]);
            //envoier pour coppeliq les position commander
            SetJointPos(clientID, message.position);
            simxSynchronousTrigger(clientID);

            //si on recoit les consignes du client
            if (resultr != -1)
            {
                printf("\n jai recu du retard : label=%d rr=%d rs=%d \n",message.label,resultr, results);
                GetJointPos(clientID,message.position);
                //envoyer au client les postions de Copelia
                results=sendto(serveur,&message,sizeof(message),0,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
                printf("\n jai envoyé au retard : label=%d rr=%d rs=%d \n",message.label,resultr, results);
            }

            simxSynchronousTrigger(clientID);
        
            t+=dt;
            usleep(dt*1000*1000);
        }

        simxStopSimulation(clientID, simx_opmode_oneshot);

        // Close the connection to the server
        simxFinish(clientID);
        close (serveur);
    }
    else
        printf("Connection to the server not possible\n");

    return(0);
}

