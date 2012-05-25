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
    int client_len; // byte size of client's address
    struct sockaddr_in server_addr; // server's addr
    struct sockaddr_in client_addr; // client addr
    struct hostent *hostp; // client host info
    char buf[BUFSIZE]; // message buf
    char ipstr[INET_ADDRSTRLEN]; // dotted decimal host addr string
    int optval; // flag value for setsockopt
    int len; // message byte size

     
    //check command line arguments 
    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);

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
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);

    //bind: associate the parent socket with a port 
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        error("ERROR on binding");

    //main loop: wait for a datagram, then echo it
    client_len = sizeof(client_addr);
    while (1) 
    {


        //recvfrom: receive a UDP datagram from a client
        bzero(buf, BUFSIZE);
        len = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &client_addr, &client_len);

        if (len < 0)
            error("ERROR in recvfrom");
 
        //gethostbyaddr: determine who sent the datagram
        hostp = gethostbyaddr((const char *)&client_addr.sin_addr.s_addr, sizeof(client_addr.sin_addr.s_addr), AF_INET);

        if (hostp == NULL)
            error("ERROR on gethostbyaddr");

        inet_ntop(AF_INET, &(client_addr.sin_addr), ipstr, sizeof(ipstr));

        if (ipstr == NULL)
            error("ERROR on inet_ntop\n");

        printf("server received datagram from %s (%s)\n", hostp->h_name, ipstr);
        printf("server received %d/%d bytes: %s\n", (int)strlen(buf), (int)len, buf);

    }
}
