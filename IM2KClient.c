#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 512

//error - wrapper for perror
void error(char *msg) 
{
    perror(msg);
    exit(1);
}

int main(int argc, char **argv) 
{
    int sockfd; // socket
    int port; // port to listen on
    struct sockaddr_in server_addr; // server's addr
    char *buf[BUFSIZE]; // message buf
    char *ipaddress[INET_ADDRSTRLEN]; // dotted decimal server string
    int optval; // flag value for setsockopt
    int len; //message byte size 


    //check command line arguments 
    if (argc != 4) 
    {
        fprintf(stderr, "usage: %s <port> <IP Address> <Message>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);
    *ipaddress = argv[2];
    bzero(buf, BUFSIZE);
    *buf = argv[3];

    //socket: create the parent socket 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* 
    * setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    //build the server's Internet address
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, *ipaddress, (struct sockaddr *) &server_addr.sin_addr.s_addr) < 0)
        error("ERROR creating IP Address");
    server_addr.sin_port = htons((unsigned short)port);
 
    //bind: associate the parent socket with a port 
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        error("ERROR on binding");


    //sendto: send the datagram
    sendto(sockfd, *buf, BUFSIZE, 0, (struct sockaddr *) &server_addr, sizeof(server_addr));

    close(sockfd);
}
