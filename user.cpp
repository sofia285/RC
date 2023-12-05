#include "user.h"

#define ALPHANUMERIC "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define SERVER_NAME "tejo.tecnico.ulisboa.pt"
#define SERVER_PORT "58011"

using namespace std;

typedef struct udp_contact {
    int fd;
    struct addrinfo *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
} udp_contact;

int login(char * username, char * password, udp_contact udp)
{
    string user = username;
    string pass = password;
    char msg[21], buffer[128];
    memset(msg, 0, 21);
    
    if (user.length() != 6 || user.find_first_not_of("0123456789") != string::npos) {
        printf("Invalid username.\n");
        printf("Username must be 6 characters and may only include digits.\n");
        return -1;
    }
    if (pass.length() != 8 || pass.find_first_not_of(ALPHANUMERIC) != string::npos) {
        printf("Invalid password.\n");
        printf("Password must be 8 characters and may only include letters and digits.\n");
        return -1;
    }

    sprintf(msg, "LIN %s %s\n", user.c_str(), pass.c_str());

    ssize_t n = sendto(udp.fd, msg, 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 128, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);

    write(1, "echo: ", 6); // stdout
    write(1, buffer, n);

    return 0;
}

int logout(string user, string pass, udp_contact udp)
{
    char msg[21], buffer[128];
    memset(msg, 0, 21);

    sprintf(msg, "LOU %s %s\n", user.c_str(), pass.c_str());

    ssize_t n = sendto(udp.fd, msg, 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 128, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);

    write(1, "echo: ", 6); // stdout
    write(1, buffer, n);

    return 0;
}

int unregister(string user, string pass, udp_contact udp) {
    char msg[21], buffer[128];
    memset(msg, 0, 21);

    sprintf(msg, "UNR %s %s\n", user.c_str(), pass.c_str());

    ssize_t n = sendto(udp.fd, msg, 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 128, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);

    write(1, "echo: ", 6); // stdout
    write(1, buffer, n);
    return 0;
}

udp_contact start_udp()
{
    udp_contact udp;    
    int errcode;
    ssize_t n;
    struct addrinfo hints;

    udp.fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (udp.fd == -1)                        /*error*/
        exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(SERVER_NAME, SERVER_PORT, &hints, &udp.res);
    // no futuro o port vai ter de ser +[nº de grupo] == 31
    if (errcode != 0) /*error*/
        exit(1);
    
    return udp;
}

int main(int argc, char **argv)
{
    char *nvalue = NULL;
    char *pvalue = NULL;
    int c;
    char command[128];
    string curr_user, curr_pass;

    opterr = 0;

    while ((c = getopt(argc, argv, "n:p:")) != -1) {
        switch (c) {
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
                fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
            default:
                exit(1);
        }
    }

    if (nvalue && (strcmp(nvalue, "-p") == 0 || strcmp(nvalue, "-n") == 0)) {
        fprintf(stderr, "Option -n requires an argument.\n");
        exit(1);
    }

    if (pvalue && (strcmp(pvalue, "-p") == 0 || strcmp(pvalue, "-n") == 0)) {
        fprintf(stderr, "Option -p requires an argument.\n");
        exit(1);
    }
    
    printf("nvalue = %s, pvalue = %s\n", nvalue, pvalue);

    // TODO: make nvalue and pvalue change server name and port
    udp_contact udp = start_udp();

    while(1) {
        char buffer[128];
        memset(command, 0, 128);
        fgets(buffer, 128, stdin);
        sscanf(buffer, "%s", command);

        if (strcmp(command, "exit") == 0) {
            break;
        }
        else if (strcmp(command, "login") == 0) {
            char username[128], password[128];
            sscanf(buffer, "%s %s %s", command, username, password);
            if (!login(username, password, udp)) {
                printf("login successful\n");
                curr_user = username;
                curr_pass = password;
            }
        }
        else if (strcmp(command, "request") == 0) {
            printf("request\n");
        }
        else if (strcmp(command, "list") == 0) {
            printf("list\n");
        }
        else if (strcmp(command, "logout") == 0) {
            if (curr_user.empty()) {
                printf("You are not logged in.\n");
            }
            else {
                if (!logout(curr_user.c_str(), curr_pass.c_str(), udp)) {
                    curr_user.clear();
                    curr_pass.clear();
                    printf("logout successful\n");
                }
            }
        }
        else if (strcmp(command, "unregister") == 0) {
            if (curr_user.empty()) {
                printf("You are not logged in. Please log in to unregister user.\n");
            }
            else {
                if (!unregister(curr_user.c_str(), curr_pass.c_str(), udp)) {
                    curr_user.clear();
                    curr_pass.clear();
                    printf("unregister successful\n");
                }
            }
        }
        else {
            printf("Command not found.\n");
        }
    }

    freeaddrinfo(udp.res);
    close(udp.fd);

    return 0;
}