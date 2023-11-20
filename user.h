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

int login();
int logout();
int request();
int list();


#endif // __USER_H__