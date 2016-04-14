#include <stdio.h>
#include <unistd.h>
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ros/ros.h"
#include "topics.h"
#include "roycobot/position2d.h"

#define SERVER "192.168.1.11"
// #define SERVER "127.0.0.1"
#define PORT 52000

pid_t procListener;
pid_t procSender;

struct udpMessage {
        int x;
        int y;
        int x2;
        int y2;
};
struct udpMessage udpMsg;

unsigned int x;
unsigned int y;

void die(char *s)
{
    perror(s);
    exit(1);
}

void Ontvanger(const roycobot::position2d::ConstPtr& signaal){
    x = signaal->x;
    y = signaal->y;
}

void listenerFunc(int argc, char **argv){
    ros::init(argc, argv, "otherrobotpos");
    ros::NodeHandle n;
    
    struct sockaddr_in si_me, si_other;
     
    int s, i, recv_len;
    unsigned int slen = sizeof(si_other);
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
    
    while(ros::ok()){
        if ((recv_len = recvfrom(s, &udpMsg, sizeof(struct udpMessage), 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        ROS_INFO("OTHERROBOTPOS: { x = %d ; y = %d }\n", udpMsg.x, udpMsg.y);
    }
    exit(0);
}

void senderFunc(int argc, char **argv){
    ros::init(argc, argv, "sharerobotpos");
    ros::NodeHandle n;
    
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    ros::Subscriber sub = n.subscribe<roycobot::position2d>(robotsharepos, 10, Ontvanger);
    while(ros::ok()){
        ros::spinOnce();
        
        udpMsg.x = udpMsg.x2 = x;
        udpMsg.y = udpMsg.y2 = y;
        
        ROS_INFO("SHARINGPOS: { x = %d ; y = %d }\n", udpMsg.x, udpMsg.y);
        
        if (sendto(s, &udpMsg, sizeof(udpMsg) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
    }
    exit(0);
}

int main(int argc, char **argv){
        procListener = fork();
        if(procListener == 0){
                listenerFunc(argc, argv);
        }
        
        senderFunc(argc, argv);
}
