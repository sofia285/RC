#include "user.h"

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
    char msg[21], buffer[9];
    memset(msg, 0, 21);
    memset(buffer, 0, 7);
    
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
    if (n == -1) {/*error*/
        printf(SEND_ERROR);
        return -1;
    }

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 8, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        printf(RECEIVE_ERROR);
        return -1;
    }

    if (strcmp(buffer, "RLI OK\n") == 0) {
        return 0;
    }
    else if (strcmp(buffer, "RLI REG\n") == 0) {
        printf("User has been registered.\n");
        return 0;
    }
    else if (strcmp(buffer, "RLI NOK\n") == 0) {
        printf("Wrong password.\n");
        return -1;
    }

    printf(UNKNOWN_ERROR);
    return -1;
}

int logout(string user, string pass, udp_contact udp)
{
    char msg[21], buffer[9];
    memset(msg, 0, 21);
    memset(buffer, 0, 9);

    sprintf(msg, "LOU %s %s\n", user.c_str(), pass.c_str());

    ssize_t n = sendto(udp.fd, msg, 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) {/*error*/
        printf(SEND_ERROR);
        return -1;
    }

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 8, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        printf(RECEIVE_ERROR);
        return -1;
    }

    if (strcmp(buffer, "RLO OK\n") == 0) {
        return 0;
    }
    else if (strcmp(buffer, "RLO UNR\n") == 0) {
        printf("User does not exist.\n");
        return -1;
    }
    else if (strcmp(buffer, "RLO NOK\n") == 0) {
        printf(LOGIN_ERROR);
        return -1;
    }

    printf(UNKNOWN_ERROR);
    return -1;
}

int unregister(string user, string pass, udp_contact udp) {
    char msg[21], buffer[9];
    memset(msg, 0, 21);
    memset(buffer, 0, 9);

    if (user.empty() || pass.empty()) {
        printf(LOGIN_ERROR);
        return -1;
    }

    sprintf(msg, "UNR %s %s\n", user.c_str(), pass.c_str());

    ssize_t n = sendto(udp.fd, msg, 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 8, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        printf(RECEIVE_ERROR);
        return -1;
    }

    if (strcmp(buffer, "RUR OK\n") == 0) {
        return 0;
    }
    else if (strcmp(buffer, "RUR UNR\n") == 0) {
        printf("User does not exist.\n");
        return -1;
    }
    else if (strcmp(buffer, "RUR NOK\n") == 0) {
        printf(LOGIN_ERROR);
        return -1;
    }

    printf(UNKNOWN_ERROR);
    return -1;
}

int myauctions(string user, udp_contact udp) {
    char msg[12], buffer[1024];
    memset(msg, 0, 12);
    memset(buffer, 0, 1024);

    if (user.empty()) {
        printf(LOGIN_ERROR);
        return -1;
    }

    sprintf(msg, "LMA %s\n", user.c_str());

    ssize_t n = sendto(udp.fd, msg, 11, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);

    if (strcmp(buffer, "RMA NOK\n") == 0) {
        printf("You have no auctions.\n");
        return 0;
    }
    else if (strcmp(buffer, "RMA NLG\n") == 0) {
        printf(LOGIN_ERROR);
        return -1;
    }
    else if (strncmp(buffer, "RMA OK", 6) == 0) {
        printf("Your auctions:\n");
        int aid, status;
        std::istringstream iss(buffer);
        iss.ignore(7); // Ignore "RMA OK "
        
        while (iss >> aid >> status) {
            printf("Auction ID: %d - ", aid);
            if (status == 1) {
                printf("Active\n");
            }
            else if (status == 0) {
                printf("Closed\n");
            }
        }
        return 0;
    }

    printf(UNKNOWN_ERROR);
    return -1;
}

int mybids(string user, udp_contact udp) {
    char msg[12], buffer[1024];
    memset(msg, 0, 12);
    memset(buffer, 0, 1024);

    if (user.empty()) {
        printf(LOGIN_ERROR);
        return -1;
    }

    sprintf(msg, "LMB %s\n", user.c_str());

    ssize_t n = sendto(udp.fd, msg, 11, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);

    if (strcmp(buffer, "RMB NOK\n") == 0) {
        printf("You have no bids.\n");
        return 0;
    }
    else if (strcmp(buffer, "RMB NLG\n") == 0) {
        printf(LOGIN_ERROR);
        return -1;
    }
    else if (strncmp(buffer, "RMB OK", 6) == 0) {
        printf("Your bids:\n");
        int aid, status;
        std::istringstream iss(buffer);
        iss.ignore(7); // Ignore "RMA OK "
        
        while (iss >> aid >> status) {
            printf("Auction ID: %d - ", aid);
            if (status == 1) {
                printf("Active\n");
            }
            else if (status == 0) {
                printf("Closed\n");
            }
        }
        return 0;
    }

    printf(UNKNOWN_ERROR);
    return -1;
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
        printf("> ");
        fgets(buffer, 128, stdin);
        sscanf(buffer, "%s", command);

        if (strcmp(command, "exit") == 0) {
            freeaddrinfo(udp.res);
            close(udp.fd);
            break;
        }
        else if (strcmp(command, "login") == 0) {
            char username[128], password[128];
            sscanf(buffer, "%s %s %s", command, username, password);
            if (login(username, password, udp) == 0) {
                curr_user = username;
                curr_pass = password;
                printf("You are now logged in.\n");
            } 
        }
        else if (strcmp(command, "myauctions") == 0 || strcmp(command, "ma") == 0) {
            myauctions(curr_user, udp);
        }
        else if (strcmp(command, "mybids") == 0 || strcmp(command, "mb") == 0) {
            mybids(curr_user, udp);
        }
        else if (strcmp(command, "show_record") == 0 || strcmp(command, "sr") == 0) {
            // TODO: show_record
        }
        else if (strcmp(command, "list") == 0) {
            // TODO: list
        }
        else if (strcmp(command, "logout") == 0) {
            if (logout(curr_user.c_str(), curr_pass.c_str(), udp) == 0) {
                curr_user.clear();
                curr_pass.clear();
                printf("You are now logged out.\n");
            }
        }
        else if (strcmp(command, "unregister") == 0) {
            if (unregister(curr_user.c_str(), curr_pass.c_str(), udp) == 0) {
                curr_user.clear();
                curr_pass.clear();
                printf("You have been unregistered.\n");
            }
        }
        else {
            printf("Command not found.\n");
        }
    }

    return 0;
}