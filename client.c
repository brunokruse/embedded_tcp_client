#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <netdb.h>
#include "protocol.h"

int main(int argc, char *argv[]) {

    // assign domain name and port
    domain_name = argv[1];
    server_port = atoi(argv[2]);

    // get the ip of the host 
    getHostIpFromDomain();

    // create socket and send to server
    connectToServer(server_port, received_ip_from_domain);

    // send payload
    assemblePacketWithPayload("[Bruno Kruse]\0");

    // get response
    getResponse(); // connection reset by peer
    
    return 0;
}


void assemblePacketWithPayload(char *inPayload) {
    char *payload_message;
    payload_message = inPayload;
    
    char message[64] = "[Bruno Kruse]\0";
    unsigned short payload_crc16_calc = crc16((unsigned char *)message, sizeof(message));

    Data d = {
        .magic = htons(0xf00d),
        .length = htons(strlen(payload_message)),
        .message_type = htons(1),
        .timestamp = htonl((unsigned long long)time(NULL)),
        .counter = htons(1),
        .payload_crc16 = htons(payload_crc16_calc),
    };
    
    // TODO: this should be nested struct or union
    Header h = {
        .magic = htons(0xf00d),
        .length = htons(strlen(payload_message)),
        .message_type = htons(1),
        .timestamp = htonl((unsigned long long)time(NULL)),
        .counter = htons(1),
        .payload_crc16 = htons(payload_crc16_calc)
    };

    int size = sizeof(Header); //get 
    char buffer_calc[size];
    char* buffer_char;
    buffer_char = buffer_calc;
    memset(buffer_char, 0, size);
    memcpy(buffer_char, &h, size);
    d.header_crc16 = crc16((unsigned char *)buffer_char, sizeof(buffer_char));
    
    // finally add the payload
    strncpy(d.payload, message, sizeof(message));
    d.payload[sizeof(d.payload)] = '\0';
    
    // calculate byte string
    unsigned char *buffer=(unsigned char*)malloc(sizeof(d));
    memcpy(buffer,(const unsigned char*)&d, sizeof(d));

    // send buffer to server
    sendMessage(buffer);

    free(buffer);
    
}

void connectToServer(int inPort, char * inIp) {
    struct sockaddr_in server_address;
    serverfd = socket(AF_INET, SOCK_STREAM, 0); // maybe try SOCK_RAW, IPPROTO_TCP
    if (serverfd == -1) {
        perror("cannot create socket... \n");
        exit(0);
    }
    
    // connect to server
    int len; // length of server address
    char server_ip[32];
    strcpy(server_ip, inIp);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(inPort);
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    len = sizeof(server_address);

    // check
    if (connect(serverfd, (struct sockaddr *) &server_address, len) < 0) {
        perror("error making connection... \n"); // error
        exit(0);
    }
}

void sendMessage(unsigned char *inBuffer) {
    // send the packet
    // check
    int i;
    for(i=0;i<sizeof(Data);i++)
        printf("%02X ", inBuffer[i]);
    printf("\n");

    int nbytes;
    if ((nbytes = send(serverfd, inBuffer, sizeof(Data), 0)) != sizeof(Data)) {
        perror("bad write... \n");
        exit(0);
    }
    printf("bytes sent: %i\n", nbytes);
    counter += 1;
}

void getResponse() {
    char server_response[1024];
    memset(server_response, 0, sizeof(server_response)); 
    if (read(serverfd, server_response, sizeof(server_response)) < 0) {
        perror("cannot read... \n");
        exit(0);
    }

   // response confirm, waiting for a 2 if successful
   printf("Server response: %s\n", server_response);
}
