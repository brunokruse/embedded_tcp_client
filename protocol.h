#include <stdio.h>
#include <stdlib.h>

// datastruct
typedef struct message_struct {
    char data[64]; // 64 bytes
} Data;

typedef struct header_struct {
    unsigned short magic; // 16 bits, uint8_t
    unsigned short length; // 16 bits
    unsigned char message_type; // 8 bits
    unsigned long long timestamp; // 64 bits
    unsigned char counter; // 8 bits
    unsigned short payload_crc16; // 16 bits
    unsigned short header_crc16; // 16 bits
} Header;

typedef struct full_message {
    Header header;
    Data payload;
} Message;

// vars
Data data = { 0 };
int counter = 0;
int message_type = 1;
char argv_payload[64]; // default
char header_data[144]; // header
int serverfd; // server descriptor

// get dynamic address info
struct addrinfo hints, *res, *p;
char ipstr[INET6_ADDRSTRLEN];
char *domain_name = "localhost";
char *received_ip_from_domain = "127.0.0.1"; // recalculate if needed
int server_port = 1234;

void assemblePacketWithPayload(char *inPayload);
void connectToServer(int inPort, char * inIp);
void sendMessage(unsigned char *inBuffer);
void getResponse();
void getHostIpFromDomain();
unsigned short crc16(const unsigned char* data_p, unsigned char length);

// https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
unsigned short crc16(const unsigned char* data_p, unsigned char length){
    unsigned char x;
    unsigned short crc = 0xf00d; // magic

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    }
    return crc;
}

// https://beej.us/guide/bgnet/html/
void getHostIpFromDomain() {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(domain_name, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    printf("%s\n", "receiving ip from domain...");
    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("%s: %s\n", ipver, ipstr);
        received_ip_from_domain = ipstr;
    }
}
