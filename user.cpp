#include "user.h"

using namespace std;

typedef struct udp_contact {
    int fd;
    struct addrinfo *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
} udp_contact;

typedef struct tcp_contact {
    int fd;
    struct addrinfo *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
} tcp_contact;

int login(string user, string pass, udp_contact udp) {
    string msg;
    char buffer[9];
    memset(buffer, 0, 9);
    
    if (user.length() != 6 || user.find_first_not_of("0123456789") != string::npos) {
        cout << "Invalid username.\n";
        cout << "Username must be 6 characters and may only include digits.\n";
        return -1;
    }
    if (pass.length() != 8 || pass.find_first_not_of(ALPHANUMERIC) != string::npos) {
        cout << "Invalid password.\n";
        cout << "Password must be 8 characters and may only include letters and digits.\n";
        return -1;
    }

    msg = "LIN " + user + " " + pass + "\n";

    ssize_t n = sendto(udp.fd, msg.c_str(), 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) { /*error*/
        cout << SEND_ERROR;
        return -1;
    }

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 9, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    cout << "n = " << n << endl;
    cout << "buffer = " << buffer << endl;
    if (n == -1) { /*error*/
        cout << RECEIVE_ERROR;
        return -1;
    }

    if (strcmp(buffer, "RLI OK\n") == 0) {
        return 0;
    }
    else if (strcmp(buffer, "RLI REG\n") == 0) {
        cout << "User has been registered.\n";
        return 0;
    }
    else if (strcmp(buffer, "RLI NOK\n") == 0) {
        cout << "Wrong password.\n";
        return -1;
    }

    cout << UNKNOWN_ERROR;
    return -1;
}

int logout(string user, string pass, udp_contact udp) {
    string msg;
    char buffer[9];
    memset(buffer, 0, 9);

    if (user.empty() || pass.empty()) {
        cout << LOGIN_ERROR;
        return -1;
    }

    msg = "LOU " + user + " " + pass + "\n";

    ssize_t n = sendto(udp.fd, msg.c_str(), 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return -1;
    }

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 8, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return -1;
    }

    if (strcmp(buffer, "RLO OK\n") == 0) {
        return 0;
    }
    else if (strcmp(buffer, "RLO UNR\n") == 0) {
        cout << "User does not exist.\n";
        return -1;
    }
    else if (strcmp(buffer, "RLO NOK\n") == 0) {
        cout << LOGIN_ERROR;
        return -1;
    }

    cout << UNKNOWN_ERROR;
    return -1;
}

int unregister(string user, string pass, udp_contact udp) {
    string msg;
    char buffer[9];
    memset(buffer, 0, 9);

    if (user.empty() || pass.empty()) {
        cout << LOGIN_ERROR;
        return -1;
    }

    msg = "UNR " + user + " " + pass + "\n";

    ssize_t n = sendto(udp.fd, msg.c_str(), 20, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 8, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return -1;
    }

    if (strcmp(buffer, "RUR OK\n") == 0) {
        return 0;
    }
    else if (strcmp(buffer, "RUR UNR\n") == 0) {
        cout << "User does not exist.\n";
        return -1;
    }
    else if (strcmp(buffer, "RUR NOK\n") == 0) {
        cout << LOGIN_ERROR;
        return -1;
    }

    cout << UNKNOWN_ERROR;
    return -1;
}

int myauctions(string user, udp_contact udp) {
    string msg;
    char buffer[1024];
    memset(buffer, 0, 1024);

    if (user.empty()) {
        cout << LOGIN_ERROR;
        return -1;
    }

    msg = "LMA " + user + "\n";

    ssize_t n = sendto(udp.fd, msg.c_str(), 11, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);

    if (strcmp(buffer, "RMA NOK\n") == 0) {
        cout << "You have no auctions.\n";
        return 0;
    }
    else if (strcmp(buffer, "RMA NLG\n") == 0) {
        cout << LOGIN_ERROR;
        return -1;
    }
    else if (strncmp(buffer, "RMA OK", 6) == 0) {
        cout <<"Your auctions:\n";
        int aid, status;
        istringstream iss(buffer);
        iss.ignore(7); // Ignore "RMA OK "
        
        while (iss >> aid >> status) {
            cout << "Auction ID: %d - " << aid;
            if (status == 1) {
                cout << "Active";
            }
            else if (status == 0) {
                cout << "Closed";
            }
            cout << endl;
        }
        return 0;
    }

    cout << UNKNOWN_ERROR;
    return -1;
}

int mybids(string user, udp_contact udp) {
    string msg;
    char buffer[1024];
    memset(buffer, 0, 1024);

    if (user.empty()) {
        printf(LOGIN_ERROR);
        return -1;
    }

    msg = "LMB " + user + "\n";

    ssize_t n = sendto(udp.fd, msg.c_str(), 11, 0, udp.res->ai_addr, udp.res->ai_addrlen);
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

int show_record(string aid, udp_contact udp) {
    char msg[9], buffer[1024];
    memset(msg, 0, 9);
    memset(buffer, 0, 1024);

    // TODO: check if aid is valid/make aid valid
    sprintf(msg, "SRC %s\n", aid.c_str());

    ssize_t n = sendto(udp.fd, msg, 8, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);
    
    if (strcmp(buffer, "RRC NOK\n") == 0) {
        printf("Auction does not exist.\n");
        return 0;
    }
    else if (strncmp(buffer, "RRC OK", 6) == 0) {
        printf("Auction ID: %s\n", aid.c_str());
        string host_UID, auction_name, asset_fname, start_value, start_date, start_time, timeactive;
        char letter;
        std::istringstream iss(buffer);
        iss.ignore(7); // Ignore "RRD OK "
        iss >> host_UID >> auction_name >> asset_fname >> start_value >> start_date >> start_time >> timeactive;
        printf("Host UID: %s\n", host_UID.c_str());
        printf("Auction name: %s\n", auction_name.c_str());
        printf("Asset filename: %s\n", asset_fname.c_str());
        printf("Start value: %s\n", start_value.c_str());
        printf("Start date: %s\n", start_date.c_str());
        printf("Start time: %s\n", start_time.c_str());
        printf("Time active: %s\n", timeactive.c_str());

        iss >> letter;
        if (letter == 'B') {
            printf("Bids:\n");
            while (letter == 'B') {
                string bidder_UID, bid_value, bid_date, bid_time, bid_sec_time;
                iss >> bidder_UID >> bid_value >> bid_date >> bid_time >> bid_sec_time;
                printf("Bidder UID: %s\n", bidder_UID.c_str());
                printf("Bid value: %s\n", bid_value.c_str());
                printf("Bid date: %s\n", bid_date.c_str());
                printf("Bid time: %s\n", bid_time.c_str());
                printf("Bid time in seconds: %s\n", bid_sec_time.c_str());
                iss >> letter;
            }
        }

        if (letter != 'E') {
            printf(UNKNOWN_ERROR);
            return -1;
        }

        string end_date, end_time, end_sec_time;
        iss >> end_date >> end_time >> end_sec_time;
        printf("End date: %s\n", end_date.c_str());
        printf("End time: %s\n", end_time.c_str());
        printf("End time in seconds: %s\n", end_sec_time.c_str());

        return 0;
    }

    printf(UNKNOWN_ERROR);
    return -1;

}

int list(udp_contact udp) {
    char msg[5];
    char buffer[100000];
    memset(buffer, 0, 100000);

    sprintf(msg, "LST\n");

    ssize_t n = sendto(udp.fd, msg, 4, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    ssize_t addrlen = sizeof(udp.addr);
    n = recvfrom(udp.fd, buffer, 99999, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) /*error*/
        exit(1);
    
    printf("buffer = %s\n", buffer);

    if (strcmp(buffer, "RLS NOK\n") == 0) {
        printf("No auctions available.\n");
        return 0;
    }
    else if (strncmp(buffer, "RLS OK", 6) == 0) {
        printf("Auctions:\n");
        int aid, status;
        std::istringstream iss(buffer);
        iss.ignore(7); // Ignore "RLS OK "
        
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

int open(string user, string pass, string asset_fname, string start_value, string timeactive, tcp_contact tcp) {
    char msg[1024], buffer[1024];
    memset(msg, 0, 1024);
    memset(buffer, 0, 1024);

    if (asset_fname.empty() || start_value.empty() || timeactive.empty()) {
        printf("Invalid arguments.\n");
        return -1;
    }

    sprintf(msg, "OPN %s %s %s\n", asset_fname.c_str(), start_value.c_str(), timeactive.c_str());

    ssize_t n = connect(tcp.fd, tcp.res->ai_addr, tcp.res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    n = write(tcp.fd, msg, 1023);
    if (n == -1) /*error*/
        exit(1);

    n = read(tcp.fd, buffer, 1023);
    if (n == -1) /*error*/
        exit(1);

    if (strcmp(buffer, "ROP OK\n") == 0) {
        printf("Auction opened successfully.\n");
        return 0;
    }
    else if (strcmp(buffer, "ROP NOK\n") == 0) {
        printf("Auction could not be opened.\n");
        return -1;
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

tcp_contact start_tcp() {
    tcp_contact tcp;
    int errcode;
    ssize_t n;
    struct addrinfo hints;

    tcp.fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (tcp.fd == -1)                        /*error*/
        exit(1);
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    errcode = getaddrinfo(SERVER_NAME, SERVER_PORT, &hints, &tcp.res);
    // no futuro o port vai ter de ser +[nº de grupo] == 31
    if (errcode != 0) /*error*/
        exit(1);
    
    return tcp;
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
            string aid;
            stringstream ss(buffer);
            ss >> command;
            ss >> aid;
            show_record(aid, udp);
        }
        else if (strcmp(command, "list") == 0 || strcmp(command, "l") == 0) {
            list(udp);
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

    freeaddrinfo(udp.res);
    close(udp.fd);

    return 0;
}