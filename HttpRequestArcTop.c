/* 
 * File:   main.c
 * Author: dushyant
 *
 * Created on August 3, 2017, 2:14 PM
 */

#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

void error(const char *msg) { perror(msg); exit(0); }

int main(int argc,char *argv[])
{
    /* first what are we going to send and where are we going to send it? */
    int portno =        80;
    char *host =        "api.masterdatanode.com";
    char data[1026] = "{ \"data\" : [{ \"latidude\" : \"28.6667\", \"longitude\" : \"77.2167\", \"machine\" : \"Mac OS\" }]}";
    char *message_fmt = "POST /demo/##APPNAME##/save/ HTTP/1.1\r\ncache-control: no-cache\r\naccess-token: ####ACCESSTOKEN######\r\ncontent-type:application/json\r\nHost: api.masterdatanode.com\r\nContent-Length: %d\r\n\r\n%s";     //{ \"data\" : [{ \"latidude\" : \"28.6667\", \"longitude\" : \"77.2167\", \"machine\" : \"Mac OS\" }]}";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024],response[4096];

    if (argc < 1) { puts("Parameters: <apikey> <command>"); exit(0); }

    /* fill in the parameters */
    sprintf(message,message_fmt,strlen(data),data);
    printf("Request:\n%s\n",message);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    printf("Check Socket .........");
    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) error("ERROR, no such host");

    printf("Define Host .........");
    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    printf("Memory and Port Allocated .........");
    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("Waiting for Response .........");
    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
        printf("-");
//        printf(bytes+"");
    } while (received < total);

    if (received == total)
        error("ERROR storing complete response from socket");

    /* close the socket */
    close(sockfd);

    /* process response */
    printf("Response:\n%s\n",response);

    return 0;
}

