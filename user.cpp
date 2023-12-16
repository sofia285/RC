#include "user.h"

using namespace std;

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
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 9, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) { /*error*/
        cout << RECEIVE_ERROR;
        return 1;
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
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 9, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
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
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 9, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
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
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }

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
            cout << "Auction ID: " << aid;
            if (status == 1) {
                cout << " - Active";
            }
            else if (status == 0) {
                cout << " - Closed";
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
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }

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
        iss.ignore(7); // Ignore "RMB OK "
        
        while (iss >> aid >> status) {
            cout << "Auction ID: " << aid;
            if (status == 1) {
                cout << " - Active";
            }
            else if (status == 0) {
                cout << " - Closed";
            }
            cout << endl;
        }
        return 0;
    }

    printf(UNKNOWN_ERROR);
    return -1;
}

int show_record(string aid, udp_contact udp) {
    string msg;
    char buffer[1024];
    memset(buffer, 0, 1024);

    if (aid.length() < 3) {
        aid.insert(0, 3 - aid.length(), '0');
    }

    msg = "SRC " + aid + "\n";

    ssize_t n = sendto(udp.fd, msg.c_str(), 8, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 1024, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }
        
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
            do {
                string bidder_UID, bid_value, bid_date, bid_time, bid_sec_time;
                iss >> bidder_UID >> bid_value >> bid_date >> bid_time >> bid_sec_time;
                printf("Bidder UID: %s\n", bidder_UID.c_str());
                printf("Bid value: %s\n", bid_value.c_str());
                printf("Bid date: %s\n", bid_date.c_str());
                printf("Bid time: %s\n", bid_time.c_str());
                printf("Bid time in seconds: %s\n", bid_sec_time.c_str());
                iss >> letter;
            } while (letter == 'B');
        }

        if (letter == 'E') {
            string end_date, end_time, end_sec_time;
            iss >> end_date >> end_time >> end_sec_time;
            printf("End date: %s\n", end_date.c_str());
            printf("End time: %s\n", end_time.c_str());
            printf("Duration: %s seconds\n", end_sec_time.c_str());
        }

        return 0;
    }

    printf(UNKNOWN_ERROR);
    return -1;

}

int list(udp_contact udp) {
    char msg[5];
    char buffer[6002];
    memset(buffer, 0, 6002);

    sprintf(msg, "LST\n");

    ssize_t n = sendto(udp.fd, msg, 4, 0, udp.res->ai_addr, udp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = recvfrom(udp.fd, buffer, 6001, 0, (struct sockaddr *)&udp.addr, &udp.addrlen);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }
    
    if (strcmp(buffer, "RLS NOK\n") == 0) {
        printf("No auctions available.\n");
        return 0;
    }
    else if (strncmp(buffer, "RLS OK", 6) == 0) {
        printf("Auctions:\n");
        int aid, status;
        istringstream iss(buffer);
        iss.ignore(7); // Ignore "RLS OK "
        
        while (iss >> aid >> status) {
            cout << "Auction ID: " << aid;
            if (status == 1) {
                cout << " - Active";
            }
            else if (status == 0) {
                cout << " - Closed";
            }
            cout << endl;
        }
        return 0;
    }

    printf(UNKNOWN_ERROR);
    return -1;
}

int open_auction(string user, string pass, string name, string asset_fname, string start_value, string timeactive, tcp_contact tcp) {
    string msg;
    char buffer[1024];
    size_t fsize;
    struct stat file_stat;
    memset(buffer, 0, 1024);

    if (user.empty() || pass.empty()) {
        cout << LOGIN_ERROR;
        return -1;
    }

    if (name.empty() || asset_fname.empty() || start_value.empty() || timeactive.empty()) {
        cout << "Invalid arguments.\n";
        return -1;
    }

    if (stat(asset_fname.c_str(), &file_stat) == -1) {
        cout << "Failed to find file " << asset_fname << "." << endl;
        return -1;
    }
    fsize = file_stat.st_size;

    msg = "OPA " + user + " " + pass + " " + name + " " + start_value + " " + timeactive + " " + asset_fname + " " + to_string(fsize) + " ";

    int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1) {
        cout << CONNECTION_ERROR;
        return 1;
    }

    ssize_t n = connect(fd, tcp.res->ai_addr, tcp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << CONNECTION_ERROR;
        return 1;
    }

    n = write(fd, msg.c_str(), msg.length());
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    int file = open(asset_fname.c_str(), O_RDONLY);
    n = sendfile(fd, file, NULL, fsize);
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }
    close(file);
    
    n = write(fd, "\n", 1);
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }
    
    n = read(fd, buffer, 1024);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }
    
    close(fd);

    if (strncmp(buffer, "ROA OK", 6) == 0) {
        istringstream iss(buffer);
        int aid;
        iss.ignore(7); // Ignore "ROA OK "
        iss >> aid;
        cout << "Auction opened successfully.\n";
        cout << "Auction ID: " << aid << endl;
        return 0;
    }
    else if (strcmp(buffer, "ROP NOK\n") == 0) {
        cout << "Auction could not be opened.\n";
        return -1;
    }
    else if (strcmp(buffer, "ROP NLG\n") == 0) {
        cout << LOGIN_ERROR;
        return -1;
    }

    cout << UNKNOWN_ERROR;
    return -1;

}

int close_auction(string user, string pass, string aid, tcp_contact tcp) {
    string msg;
    char buffer[9];
    memset(buffer, 0, 9);

    if (user.empty() || pass.empty()) {
        cout << LOGIN_ERROR;
        return -1;
    }

    if (aid.empty() || aid.length() != 3) {
        cout << "Invalid auction ID.\n";
        return -1;
    }

    msg = "CLS " + user + " " + pass + " " + aid + "\n";

    int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1) {
        cout << CONNECTION_ERROR;
        return 1;
    }

    ssize_t n = connect(fd, tcp.res->ai_addr, tcp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << CONNECTION_ERROR;
        return 1;
    }

    n = write(fd, msg.c_str(), msg.length());
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = read(fd, buffer, 9);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }

    close(fd);

    if (strcmp(buffer, "RCL OK\n") == 0) {
        cout << "Auction closed successfully.\n";
        return 0;
    }
    else if (strcmp(buffer, "RCL NOK\n") == 0) {
        cout << "Invalid user or password\n";
        return -1;
    }
    else if (strcmp(buffer, "RCL NLG\n") == 0) {
        cout << LOGIN_ERROR;
        return -1;
    }
        else if (strcmp(buffer, "RCL EAU\n") == 0) {
        cout << "Auction does not exist.\n";
        return -1;
    }
    else if (strcmp(buffer, "RCL EOW\n") == 0) {
        cout << "Auction could not be closed, as it is not the user's.\n";
        return -1;
    }
    else if (strcmp(buffer, "RCL END\n") == 0) {
        cout << "Auction has already been closed.\n";
        return -1;
    }

    cout << UNKNOWN_ERROR;
    return -1;
}

int show_asset(string aid, tcp_contact tcp) {
    string msg;
    // TODO: receive file
    char buffer[1024];
    memset(buffer, 0, 1024);

    if (aid.empty() || aid.length() != 3) {
        cout << "Invalid auction ID.\n";
        return -1;
    }

    msg = "SAS " + aid + "\n";

    int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1) {
        cout << CONNECTION_ERROR;
        return 1;
    }

    ssize_t n = connect(fd, tcp.res->ai_addr, tcp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << CONNECTION_ERROR;
        return 1;
    }

    n = write(fd, msg.c_str(), msg.length());
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = read(fd, buffer, 1024);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }

    cout << buffer << endl;

    if (strcmp(buffer, "RSA NOK\n") == 0) {
        cout << "A problem occured while sending the file.\n";
        close(fd);
        return -1;
    }
    else if (strncmp(buffer, "RSA OK", 6) == 0) {
        cout << "Asset:\n";
        string fname;
        int fsize;
        istringstream iss(buffer);
        iss.ignore(7); // Ignore "RAS OK "
        iss >> fname >> fsize;

        char buffer[fsize];
        n = read(fd, buffer, fsize);
        if (n == -1) {/*error*/
            cout << RECEIVE_ERROR;
            return 1;
        }

        cout << buffer << endl;

        cout << "File name: " << fname << endl;
        cout << "File size: " << fsize << endl;
        close(fd);
        return 0;
    }

    close(fd);
    cout << UNKNOWN_ERROR;
    return -1;
}

int bid(string user, string pass, string aid, string value, tcp_contact tcp) {
    string msg;
    char buffer[9];
    memset(buffer, 0, 9);

    if (user.empty() || pass.empty()) {
        cout << LOGIN_ERROR;
        return -1;
    }

    if (aid.empty() || aid.length() != 3) {
        cout << "Invalid auction ID.\n";
        return -1;
    }

    if (value.empty() || value.find_first_not_of("0123456789") != string::npos) {
        cout << "Invalid bid value.\n";
        return -1;
    }

    msg = "BID " + user + " " + pass + " " + aid + " " + value + "\n";

    int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1) {
        cout << CONNECTION_ERROR;
        return 1;
    }

    ssize_t n = connect(fd, tcp.res->ai_addr, tcp.res->ai_addrlen);
    if (n == -1) {/*error*/
        cout << CONNECTION_ERROR;
        return 1;
    }

    n = write(fd, msg.c_str(), msg.length());
    if (n == -1) {/*error*/
        cout << SEND_ERROR;
        return 1;
    }

    n = read(fd, buffer, 9);
    if (n == -1) {/*error*/
        cout << RECEIVE_ERROR;
        return 1;
    }
    
    close(fd);

    if (strcmp(buffer, "RBD ACC\n") == 0) {
        cout << "Bid placed successfully.\n";
        return 0;
    }
    else if (strcmp(buffer, "RBD NOK\n") == 0) {
        cout << "Auction has already closed.\n";
        return -1;
    }
    else if (strcmp(buffer, "RBD NLG\n") == 0) {
        cout << LOGIN_ERROR;
        return -1;
    }
    else if (strcmp(buffer, "RBD REF\n") == 0) {
        cout << "A larger bid has already been placed\n";
        return -1;
    }
    else if (strcmp(buffer, "RBD ILG\n") == 0) {
        cout << "You cannot bid on your own auction.\n";
        return -1;
    }

    cout << UNKNOWN_ERROR;
    return -1;
}

udp_contact start_udp(string asip, string asport)
{
    udp_contact udp;    
    int errcode;
    ssize_t n;
    struct addrinfo hints;

    if (asip.empty()) asip = SERVER_NAME;
    if (asport.empty()) asport = SERVER_PORT;

    udp.fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (udp.fd == -1) {
        cout << CONNECTION_ERROR;
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(asip.c_str(), asport.c_str(), &hints, &udp.res);
    // no futuro o port vai ter de ser +[nº de grupo] == 31
    if (errcode != 0) {/*error*/
        cout << CONNECTION_ERROR;
        exit(1);
    }
    
    return udp;
}

tcp_contact start_tcp(string asip, string asport) {
    tcp_contact tcp;
    int errcode;
    ssize_t n;
    struct addrinfo hints;

    if (asip.empty()) asip = SERVER_NAME;
    if (asport.empty()) asport = SERVER_PORT;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    errcode = getaddrinfo(asip.c_str(), asport.c_str(), &hints, &tcp.res);
    // no futuro o port vai ter de ser +[nº de grupo] == 31
    if (errcode == -1) {
        cout << CONNECTION_ERROR;
        exit(1);
    }

    return tcp;
}

int main(int argc, char **argv)
{
    string nvalue, pvalue;
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

    if (!nvalue.empty() && (nvalue == "-p" || nvalue == "-n")) {
        fprintf(stderr, "Option -n requires an argument.\n");
        exit(1);
    }

    if (!pvalue.empty() && (pvalue == "-p" || pvalue == "-n")) {
        fprintf(stderr, "Option -p requires an argument.\n");
        exit(1);
    }
    
    cout << "nvalue = " << nvalue << ", pvalue = " << pvalue << endl;

    udp_contact udp = start_udp(nvalue, pvalue);
    tcp_contact tcp = start_tcp(nvalue, pvalue);

    while(1) {
        char buffer[128];
        memset(command, 0, 128);
        printf("> ");
        fgets(buffer, 128, stdin);
        sscanf(buffer, "%s", command);

        if (strcmp(command, "exit") == 0) {
            if (!curr_user.empty() && !curr_pass.empty()) {
                logout(curr_user, curr_pass, udp);
            }
            freeaddrinfo(udp.res);
            close(udp.fd);
            freeaddrinfo(tcp.res);
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
            istringstream ss(buffer);
            ss >> command;
            ss >> aid;
            show_record(aid, udp);
        }
        else if (strcmp(command, "list") == 0 || strcmp(command, "l") == 0) {
            list(udp);
        }
        else if (strcmp(command, "logout") == 0) {
            if (logout(curr_user, curr_pass, udp) == 0) {
                curr_user.clear();
                curr_pass.clear();
                printf("You are now logged out.\n");
            }
        }
        else if (strcmp(command, "unregister") == 0) {
            if (unregister(curr_user, curr_pass, udp) == 0) {
                curr_user.clear();
                curr_pass.clear();
                printf("You have been unregistered.\n");
            }
        }
        else if (strcmp(command, "open") == 0) {
            string name, asset_fname, start_value, timeactive;
            istringstream ss(buffer);
            ss >> command;
            ss >> name >> asset_fname >> start_value >> timeactive;
            open_auction(curr_user, curr_pass, name, asset_fname, start_value, timeactive, tcp);
        }
        else if (strcmp(command, "close") == 0) {
            string aid;
            istringstream ss(buffer);
            ss >> command;
            ss >> aid;
            close_auction(curr_user, curr_pass, aid, tcp);
        }
        else if (strcmp(command, "show_asset") == 0 || strcmp(command, "sa") == 0) {
            string aid;
            istringstream ss(buffer);
            ss >> command;
            ss >> aid;
            show_asset(aid, tcp);
        }
        else if (strcmp(command, "bid") == 0 || strcmp(command, "b") == 0) {
            string aid, value;
            istringstream ss(buffer);
            ss >> command;
            ss >> aid >> value;
            bid(curr_user, curr_pass, aid, value, tcp);
        }
        else {
            printf("Command not found.\n");
        }
    }

    return 0;
}