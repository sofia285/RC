#include "user.h"

#define ALPHANUMERIC "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define SERVER_NAME "tejo.tecnico.ulisboa.pt"
#define SERVER_PORT "58032"

using namespace std;

int login(char * username, char * password)
{
    string user = username;
    string pass = password;
    
    if (user.empty() || user.length() > 6 || user.find_first_not_of("0123456789") != string::npos) {
        printf("Invalid username.\n");
        printf("Username must be less than 6 characters and may only include digits.\n");
        return -1;
    }
    if (pass.empty() || pass.length() > 8 || pass.find_first_not_of(ALPHANUMERIC) != string::npos) {
        printf("Invalid password.\n");
        printf("Password must be less than 8 characters and may only include letters and digits.\n");
        return -1;
    }

    return 0;
}

int start_udp(string server_name = SERVER_NAME, string server_port = SERVER_PORT)
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

    errcode = getaddrinfo(SERVER_NAME, SERVER_PORT, &hints, &res);
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

    while(1) {
        char buffer[128];
        memset(command, 0, 128);
        fgets(buffer, 128, stdin);
        sscanf(buffer, "%s", command);

        if (strcmp(command, "exit") == 0) {
            exit(0);
        }
        else if (strcmp(command, "login") == 0) {
            char username[128], password[128];
            sscanf(buffer, "%s %s %s", command, username, password);
            if (!login(username, password)) {
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
                curr_user.clear();
                curr_pass.clear();
                printf("logout successful\n");
            }
        }
        else {
            printf("Command not found.\n");
        }
    }

    return 0;
}