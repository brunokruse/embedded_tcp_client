# README

<br> 

## Included Files
* client.c
* protocol.h
* readme.md
* makefile

<br>

## Compiling && Running
assuming use of ubuntu 20+ and gcc. simple make file for convenience.

example:

```
make
./client localhost 1234 payload
```


<br>

## Data Structure

```c
//  datastruct
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
```

<br>

## Serialization
```c
    unsigned char *buffer=(unsigned char*)malloc(sizeof(d));
    memcpy(buffer,(const unsigned char*)&d, sizeof(d));

    // debug
    int i;
    for(i=0;i<sizeof(Data);i++)
        printf("%02X ", inBuffer[i]);
    printf("\n");
```

```c
5b 42 72 75 6e 6f 20 4b 72 75 73 65 5d 0 0 
```
payload


```c
F0 0D 00 0D 00 00 00 00 60 F8 84 0F 00 00 00 00 00 00 32 9B 43 D5 5B 42 72 75 6E 6F 20 4B 72 75 73 65 5D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
```
sending this to server

<br>

## Methods

```c
void assemblePacketWithPayload(char* inPayload);
```
Assembles a packet using the data struct to send to drone. 

```c
void connectToServer();

//... socket
socket(AF_INET, SOCK_STREAM, 0);

//... connect
connect(serverfd, (struct sockaddr *) &server_address, len)

```
Creates a TCP socket connection to specified server address.

```c
void sendMessage();

//... example
send(serverfd, &data, sizeof(data), 0));
```
Sends full data struct header+payloadto server. I tried this way to keep the data continous.

```c
void getResponse();
```
Server should return 2 if successful.

```c
void getHostIpFromDomain();
```
Gets the ipv4/ipv6 address from the domain name

```c
unsigned short crc16(const unsigned char* data_p, unsigned char length);
```
generic crc16 checksum calculation, uses magic 0xf00d as bitflag

<br>

## Resources

* [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/index-wide.html)
* [crc16 implementation](https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum)
