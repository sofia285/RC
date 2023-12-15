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
#define SERVER_NAME "tejo.tecnico.ulisboa.pt"
#define SERVER_PORT "58011"
#define UNKNOWN_ERROR "An unexpected error has occured. Please try again later.\n"
#define LOGIN_ERROR "You are not logged in.\n"
#define SEND_ERROR "An error occured while sending the message.\n"
#define RECEIVE_ERROR "An error occured while receiving the message.\n"
#define CONNECTION_ERROR "An error occured while connecting to the server.\n"


#endif // __USER_H__