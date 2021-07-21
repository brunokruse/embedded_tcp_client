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

int main() {
    // get the ip of the host 
    getHostIpFromDomain();

    // create continous packet from struct data containing
    // header and payload information
    printf("%s\n", "creating packet...");
    assemblePacketWithPayload("[Bruno Kruse]\0");
    
    // create socket and send to server
    printf("%s\n", "connecting to server...");
    connectToServer(server_port, received_ip_from_domain);
    
    // send the message
    printf("%s\n", "sending message...");
    sendMessage();
    
    // get response
    printf("%s\n", "waiting response...");
    getResponse();
    
    return 0;
}

void assemblePacketWithPayload(char *inPayload) {
    char *payload_message;
    payload_message = inPayload;
    
    // Data *d = malloc(sizeof(Data));
    data.magic = htons(0xf00d); // endianess
    data.length = htons(strlen(payload_message)); // dynamically calculate length of payload
    data.message_type = htons(message_type); // send message type which is 1
    data.timestamp = htonl((unsigned long long)time(NULL)); // time.h 
    data.counter = htons(counter); // increment by one each send
    data.payload_crc16 = crc16((unsigned char *)payload_message, sizeof(payload_message)); // crc16
    memcpy(data.payload, payload_message, sizeof(data.payload));

    // calculate size of header_crc16 before payload
    // TODO: update this to use memcpy for bytestrings
    sprintf(header_data, "%d%d%i%lli%i%i", data.magic, data.length, data.message_type,
        data.timestamp, data.counter, data.payload_crc16);
        
    char *header_crc;
    header_crc = header_data;
    data.header_crc16 = crc16((unsigned char *)header_crc, sizeof(header_crc));
    
    // calculate bytestring
    /*
    unsigned char *buffer=(char*)malloc(sizeof(data));
    int i;

    //copying....
    memcpy(buffer,(const unsigned char*)&data,sizeof(data));

    //printing..
    printf("Copied byte array is:\n");
    for(i=0;i<sizeof(data);i++)
        printf("%02X ",buffer[i]);
    printf("\n");

    //freeing memory..
    free(buffer);
    */
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

void sendMessage() {
    // send the packet
    int nbytes;
    if ((nbytes = send(serverfd, &data, sizeof(data), 0)) != sizeof(data)) {
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
   printf("\nServer response: %s\n", server_response);
}
