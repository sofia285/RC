#ifndef __USER_H__
#define __USER_H__

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>

#define ALPHANUMERIC "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define SERVER_NAME "127.0.0.1"
#define SERVER_PORT "12345"
#define UNKNOWN_ERROR "An unexpected error has occured. Please try again later.\n"
#define LOGIN_ERROR "You are not logged in.\n"
#define SEND_ERROR "An error occured while sending the message.\n"
#define RECEIVE_ERROR "An error occured while receiving the message.\n"


#endif // __USER_H__