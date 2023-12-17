#ifndef __USER_H__
#define __USER_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ALPHANUMERIC "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define FILE_NAMING "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-."
#define SERVER_NAME "127.0.0.1"
#define SERVER_PORT "58031"
#define UNKNOWN_ERROR "An unexpected error has occured. Please try again later.\n"
#define LOGIN_ERROR "You are not logged in.\n"
#define SEND_ERROR "An error occured while sending the message.\n"
#define RECEIVE_ERROR "An error occured while receiving the message.\n"
#define CONNECTION_ERROR "An error occured while connecting to the server.\n"

typedef struct udp_contact {
    int fd;
    struct addrinfo *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
} udp_contact;

typedef struct tcp_contact {
    struct addrinfo *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
} tcp_contact;

#endif // __USER_H__