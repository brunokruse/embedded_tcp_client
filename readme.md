# README
*Drone Racing League embedded challenge*

<br> 

## Included Files
* client.c
* protocol.h
* readme.md
* makefile

<br>

## Compiling && Running
assuming use of ubuntu 20+ and gcc. simple make file for convenience.

<i>TODO: add args to client for --server_ip, --server_port, --payload<i>

```
make
./client
```

<br>

## Data Structure

```c
//  datastruct
typedef struct {
    unsigned short magic; // 16 bits
    unsigned short length; // 16 bits, uint_16t
    unsigned char message_type; // 8 bits, uint_t
    unsigned long long timestamp; // 64 bits, uint64_t
    unsigned char counter; // 8 bits
    unsigned short payload_crc16; // 16 bits, uint16_t
    unsigned short header_crc16; // 16 bits, uint16_t
    char payload[64]; // 64 bytes
} Data;
```

<br>

## Serialization
```c
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
```

```c
5b 42 72 75 6e 6f 20 4b 72 75 73 65 5d 0 0 
```
payload


```c
F0 0D 00 0D 01 00 00 00 60 F7 C5 FD 00 00 00 00 00 00 D5 44 6F D4 5B 42 72 75 6E 6F 20 4B 72 75 73 65 5D 00 25 30 32 58 20 00 25 64 25 64 25 69 25 6C 69 25 69 25 69 00 43 6F 70 69 65 64 20 62 79 74 65 20 61 72 72 61 79 20 69 73 3A 00 63 61 6E 6E 6F 74 20 00 00 00 
```
//TODO: full message output is garbled


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