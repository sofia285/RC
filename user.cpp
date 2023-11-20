#include "user.h"

using namespace std;

int user(void)
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

int main(int argc, char **argv)
{
    char *nvalue = NULL;
    char *pvalue = NULL;
    int c;
    char command[128];

    opterr = 0;

    while ((c = getopt(argc, argv, "n:p:")) != -1)
        switch (c)
        {
        case 'n':
            nvalue = optarg;
            break;
        case 'p':
            pvalue = optarg;
            break;
        case '?':
            if (optopt == 'n' || optopt == 'p')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character '\\x%x'.\n",
                        optopt);
        default:
            abort();
        }
    
    printf("nvalue = %s, pvalue = %s\n", nvalue, pvalue);

    while(1) {
        memset(command, 0, 128);
        scanf("%s", command);

        if (strcmp(command, "exit") == 0) {
            exit(0);
        }
        else if (strcmp(command, "login") == 0) {
            printf("login\n");
        }
        else if (strcmp(command, "request") == 0) {
            printf("request\n");
        }
        else if (strcmp(command, "list") == 0) {
            printf("list\n");
        }
        else if (strcmp(command, "logout") == 0) {
            printf("logout\n");
        }
        else {
            printf("Command not found.\n");
        }
    }

    return 0;
}