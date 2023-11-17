#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

int main(void)
{
    int fd, errcode;
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n;
    char buffer[128], msg[128];
    struct addrinfo hints, *res;

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "58001", &hints, &res);
    // no futuro o port vai ter de ser +[nº de grupo] == 31
    if (errcode != 0) /*error*/
        exit(1);

    while (1)
    {
        memset(msg, 0, 128);
        read(0, msg, 128);
        write(1, "wow: ", 6);
        write(1, msg, 128);
        write(1, "\n", 2);
        n = sendto(fd, msg, 128, 0, res->ai_addr, res->ai_addrlen);
        if (n == -1) /*error*/
            exit(1);

        addrlen = sizeof(addr);
        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr *)&addr, &addrlen);
        if (n == -1) /*error*/
            exit(1);

        write(1, "echo: ", 6); // stdout
        write(1, buffer, n);
        write(1, "\n", 2);
    }

    freeaddrinfo(res);
    close(fd);
    exit(0);
}