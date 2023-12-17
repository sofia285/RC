#include "AS.hpp"

bool verbose = false;
int aid = 1;

int main(int argc, char **argv){
    char *pvalue = NULL;
    char *vvalue = NULL;
    int c;
    string commandUDP, commandTCP, UID, password, AID;
    int serverPort = 58031; // Default server port

    opterr = 0;

    while ((c = getopt(argc, argv, "p:v")) != -1) {
        switch (c) {
        case 'p':
            pvalue = optarg;
            break;
        case 'v':
            vvalue = optarg;
            verbose = true;
            cout << "Verbose mode on" << endl;
            break;
        case '?':
            if (optopt == 'p')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
            default:
                exit(1);
        }
    }

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            serverPort = stoi(argv[i + 1]);
        }
    }

    // Create UDP socket
    int serverSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocketUDP == -1) {
        perror("Error creating server UDP socket");
        return -1;
    }

    // Create TCP socket
    int serverSocketTCP = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketTCP == -1) {
        perror("Error creating server TCP socket");
        close(serverSocketUDP);
        return -1;
    }

    // Bind the socket to a specific port
    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    if (bind(serverSocketUDP, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding server UDP socket");
        close(serverSocketUDP);
        close(serverSocketTCP);
        return -1;
    }

    // Bind TCP socket
    if (bind(serverSocketTCP, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding server TCP socket");
        close(serverSocketUDP);
        close(serverSocketTCP);
        return -1;
    }

     // Listen for incoming TCP connections
    if (listen(serverSocketTCP, SOMAXCONN) == -1) {
        perror("Error listening on TCP socket");
        close(serverSocketUDP);
        close(serverSocketTCP);
        return -1;
    }

    // Set up variables for select()
    fd_set readfds;
    int maxSocket;

    // Receive and print messages from clients
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    string responseMessage;

    // Vector to store TCP client sockets
    vector<int> tcpClientSockets;

    while (1) {
        FD_ZERO(&readfds);

        // Add UDP socket to set
        FD_SET(serverSocketUDP, &readfds);
        maxSocket = serverSocketUDP;

        // Add TCP socket to set
        FD_SET(serverSocketTCP, &readfds);
        if (serverSocketTCP > maxSocket) {
            maxSocket = serverSocketTCP;
        }

        // Add TCP client sockets to set
        for (const auto& tcpClientSocket : tcpClientSockets) {
            FD_SET(tcpClientSocket, &readfds);
            if (tcpClientSocket > maxSocket) {
                maxSocket = tcpClientSocket;
            }
        }

        // Use select to wait for activity on any of the sockets
        int activity = select(maxSocket + 1, &readfds, nullptr, nullptr, nullptr);

        if (activity == -1) {
            perror("select error");
            return 1;
        }

        // Handle UDP data
        if (FD_ISSET(serverSocketUDP, &readfds)) {
            char buf[128] = {0};
            ssize_t bytesRead = recvfrom(serverSocketUDP, buf, sizeof(buf), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);
            if (bytesRead == -1) {
                perror("Error receiving data");
            } 
            istringstream iss(buf);
            iss >> commandUDP;
            
            // Login
            if (commandUDP == "LIN"){
                iss >> UID >> password;
                if (verbose) {
                    cout << "login: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                int userCheck = LoginUser(UID, password);
                if (userCheck == -1){
                    responseMessage = "RLI NOK\n";
                }
                else if (userCheck == 0) {
                    if (RegisterUser(UID, password) == 0) {
                        responseMessage = "RLI NOK\n";
                    }
                    else {
                        responseMessage = "RLI REG\n";
                    }
                }
                else if (userCheck == 1) {
                    if (CreateLogin(UID) == 0) {
                        responseMessage = "RLI NOK\n";
                    }
                    else {
                        responseMessage = "RLI OK\n";
                    }
                }
            }
            else if (commandUDP == "LOU") {
                iss >> UID >> password;
                if (verbose) {
                    cout << "logout: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                int userCheck = LogoutUser(UID, password);
                if (userCheck == -1){
                    responseMessage = "RLO NOK\n";
                }
                else if (userCheck == 0) {
                    responseMessage = "RLO UNR\n";
                }
                else if (userCheck == 1) {
                    if (EraseLogin(UID) == 0) {
                        responseMessage = "RLO NOK\n";
                    }
                    else {
                        responseMessage = "RLO OK\n";
                    }
                }
            }

            // Unregister
            else if (commandUDP == "UNR") {
                iss >> UID >> password;
                if (verbose) {
                    cout << "unregister: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }    
                int userCheck = UnregisterUser(UID, password);
                if (userCheck == -1) {
                    responseMessage = "RUR NOK\n";
                }
                else if (userCheck == 0) {
                    responseMessage = "RUR UNR\n";
                }
                else {
                    responseMessage = "RUR OK\n";
                }
            }

            // My Auctions
            else if (commandUDP == "LMA") {
                iss >> UID;
                if (verbose) {
                    cout << "my auctions: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                responseMessage = MyAuctions(UID);
            }

            // My Bids
            else if (commandUDP == "LMB") {
                iss >> UID;
                if (verbose) {
                    cout << "my bids: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                responseMessage = MyBids(UID);
            }

            // List Auctions
            else if (commandUDP == "LST") {
                if (verbose) {
                    cout << "list "<< endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                responseMessage = ListAuctions();
            }

            // Show Record
            else if (commandUDP == "SRC") {
                iss >> AID;
                if (verbose) {
                    cout << "show records: AID " << AID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                responseMessage = show_record(AID);
            } 

            else {
                if (verbose) {
                    cout << "Unknown command: " << commandUDP << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                responseMessage = "ERR\n";
            }

            // Send the response back to the client
            sendto(serverSocketUDP, responseMessage.c_str(), responseMessage.length(), 0,  (struct sockaddr*)&clientAddress, sizeof(clientAddress));
        }

        // Handle new TCP connections
        if (FD_ISSET(serverSocketTCP, &readfds)) {
            int newTcpClientSocket = accept(serverSocketTCP, nullptr, nullptr);
            if (newTcpClientSocket == -1) {
                perror("TCP accept failed");
                return -1;
            }

            char buf[128] = {0};
            string name, start_value, time_active, Fname, Fsize;
            ssize_t  bytesRead = read(newTcpClientSocket, buf, 128);

            if (bytesRead == -1) {
                // Client disconnected or error
                close(newTcpClientSocket);
                continue;
            }

            istringstream iss(buf);
            iss >> commandTCP;

            // Open
            if (commandTCP == "OPA") {
                iss >> UID >> password >> name >> start_value >> time_active >> Fname >> Fsize;
                if (verbose) {
                    cout << "open auction: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                ostringstream auction_name;
                auction_name << setw(3) << setfill('0') << aid++;
                AID = auction_name.str();
                if (CreateAIDDir(AID) == 0) {
                    responseMessage = "ROA NOK\n";
                }

                if (CreateASSETDir(AID) == 0) {
                    responseMessage = "ROA NOK\n";
                }

                if (CreateBIDSDir(AID) == 0) {
                    responseMessage = "ROA NOK\n";
                }

                string FnamePath = "./ASDIR/AUCTIONS/" + AID + "/ASSET/" + Fname;
                int file = open( (FnamePath).c_str() , O_WRONLY | O_CREAT, 0644);
                if (file == -1) {
                    return 1;
                }

                char buffer[32768];
                int totalBytesRead = 0;
                while ((bytesRead = read(newTcpClientSocket, buffer, sizeof(buffer))) > 0) {
                    ssize_t bytesWritten = write(file, buffer, bytesRead);
                    if (bytesWritten == -1) {
                        perror("Error writing to file");
                        break;
                    }
                    totalBytesRead += bytesRead;
                    if(totalBytesRead >= stoi(Fsize)){
                        break;
                    }
                }
                close(file);

                if (CreateHostedFile(UID, password, AID) == 0) {
                    responseMessage = "ROA NOK\n";
                }
                if (CreateSTARTFile(AID, UID, Fname, name, start_value, time_active) == 0) {
                    responseMessage = "ROA NOK\n";
                }
                else {
                    responseMessage = "ROA OK " + AID + "\n";
                }
            }            
            
            else if (commandTCP == "CLS") {
                iss >> UID >> password >> AID;
                if (verbose) {
                    cout << "close auction: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }

                int n = closeAuc(UID, password, AID);
                if (n == -1) {
                    responseMessage = "RCL NLG\n";
                }
                else if (n == 0) {
                    responseMessage = "RCL NOK\n";
                }
                else if (n == -2) {
                    responseMessage = "RCL EAU\n";
                }
                else if (n == -3) {
                    responseMessage = "RCL END\n";
                }
                else if (n == -4) {
                    responseMessage = "RCL EOW\n";
                }
                else if (n == 1){
                    responseMessage = "RCL OK\n";
                }
                    
            }

            else if (commandTCP == "SAS") {
                iss >> AID;
                if (verbose) {
                    cout << "show asset AID: " << AID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                int n = showAsset(AID, newTcpClientSocket);
            }

            else if (commandTCP == "BID") {
                string value;
                iss >> UID >> password >> AID >> value;
                if (verbose) {
                    cout << "bid: UID " << UID << endl;
                    cout << "IP: " << inet_ntoa(clientAddress.sin_addr) << " Port: "<< serverPort << endl;
                }
                responseMessage = bid(UID, password, AID, value);
            }

            if (commandTCP != "SAS") {
                cout << "Sending response: " << responseMessage << endl;
                bytesRead = write(newTcpClientSocket, responseMessage.c_str(), responseMessage.length());
                if (bytesRead == -1) {
                    return 1;
                }
            }
            close(newTcpClientSocket);

            // Add the new client socket to the vector
            //tcpClientSockets.push_back(newTcpClientSocket);
        }
    }

    // Close the socket (never reached in this example)
    close(serverSocketUDP);
    close(serverSocketTCP); 
    return 0;
}

// UDP functions
int CreateHOSTEDDir(string UID) {
    string HOSTED_dirname = "./ASDIR/USERS/" + UID + "/HOSTED";

    int ret = mkdir(HOSTED_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

int CreateBIDDEDDir(string UID) {
    string BIDDED_dirname =  "./ASDIR/USERS/" + UID + "/BIDDED";

    int ret = mkdir(BIDDED_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

// Create a new user UID directory
int CreateUSERSDir(string UID) {
    string UID_dirname = "./ASDIR/USERS/" + UID;

    int ret = mkdir(UID_dirname.c_str(),  0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

int removeUSERSDir(string UID) {
    string UID_dirname = "./ASDIR/USERS/" + UID;

    int ret = rmdir(UID_dirname.c_str());
    if (ret == -1) {
        return 0;
    }

    return 1;
}

// Create a user's password file
int CreatePassword(string UID, string password) {
    string pass_name = "./ASDIR/USERS/" + UID + "/" + UID + "_pass.txt";
    ofstream fp_pass(pass_name);

    if(!fp_pass.is_open()) {return 0;}

    fp_pass << password;

    return 1;
}

// Erase a user's password file
int ErasePassword(string UID, string password) {
    int result;
    string passwordFilePath = "./ASDIR/USERS/" + UID + "/" + UID + "_pass.txt";

    ifstream passwordFile(passwordFilePath);
    if (!passwordFile.good()) {
        return 0;
    }

    // Read the password from the file
    string storedPassword;
    passwordFile >> storedPassword;

    // Check if the provided password matches the stored password
    if (storedPassword == password) {
        result = remove(passwordFilePath.c_str());
        return (result == 0 ? 1 : 0);
    } else {
        return -1;
    }

    return (result == 0) ? 1 : 0;
}

// Create a user's login file
int CreateLogin(string UID) {
    string login_name = "./ASDIR/USERS/" + UID + "/" + UID + "_login.txt";
    ofstream fp_login(login_name);

    if (!fp_login.is_open()) {return 0;}

    fp_login << "Logged in\n";

    return 1;
}


// Erase a user's login file
int EraseLogin(string UID){
    string login_name = "./ASDIR/USERS/" + UID + "/" + UID + "_login.txt";
    int result = remove(login_name.c_str());

    return (result == 0) ? 0 : 1;
}

int RegisterUser(string UID, string password) {
    if (CreateUSERSDir (UID) == 0){
        return 0;
    }

    if (CreateLogin(UID) == 0) {
        return 0;
    }

    if (CreatePassword(UID, password) == 0) {
        return 0;
    }

    if (CreateHOSTEDDir(UID) == 0) {
        return 0;
    }

    if (CreateBIDDEDDir(UID) == 0) {
        return 0;
    }
    
    return 1;
}

int UnregisterUser(string UID, string password) {
    string uidDir = "./ASDIR/USERS/" + UID;

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // User doesn't exist
        return 0;
    }

    int userCheck = LogoutUser(UID, password);

    if (userCheck == -1) {
        // User not logged in
        return -1;
    } else if (userCheck == 0) {
        // User not registered
        return 0;
    }
    userCheck = ErasePassword(UID, password);
    if (userCheck == 0 || userCheck == -1) {
        return -1;
    }
    else {
        return 1;
    }
    return 1;
}

// Check if the user already exists -> 1
// Check if the user does not exist, and register him -> 0
// Check if the user exists, but the password is incorrect -> -1
int LoginUser(string UID, string password) {
    // Construct the path to the UID directory
    string uidDir = "./ASDIR/USERS/" + UID;

    // Open and check if the UID directory exists
    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // if user does not exist, create user
        return 0;
    }

    // Construct the path to the UID_pass.txt file
    string passwordFilePath = uidDir + "/" + UID + "_pass.txt";

    // Open and check if the password file exists
    ifstream passwordFile(passwordFilePath);
    if (!passwordFile.good()) {
        // if user does not exist, create user
        return 0;
    }

    // Read the password from the file
    string storedPassword;
    passwordFile >> storedPassword;

    // Check if the provided password matches the stored password
    if (storedPassword == password) {
        // the user exists and the password is correct
        return 1;
    } else {
        // the user exists but the password is incorrect
        return -1;
    }

    return 1;
}

int LogoutUser(string UID, string password){
    string uidDir = "./ASDIR/USERS/" + UID;

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // User doesn't exist
        return 0;
    }

    string passwordFilePath = uidDir + "/" + UID + "_pass.txt";
    string loginFilePath = uidDir + "/" + UID + "_login.txt";

    ifstream loginFile(loginFilePath);
    if (!loginFile.good()) {
        // User not logged in
        return -1;
    }

    ifstream passwordFile(passwordFilePath);
    if (!passwordFile.good()) {
        // User doesn't exist
        return 0;
    }

    string storedPassword;
    passwordFile >> storedPassword;

    if (storedPassword == password) {
        if (EraseLogin(UID) == 0) {
            // User exists, was logged in and the password is correct
            return 1;
        }
        else {
            return 0;
        }
    } else {
        // User exists but password is incorrect
        return -1;
    }
}

string MyAuctions(string UID) {
    string uidDir = "./ASDIR/USERS/" + UID;
    string aucDir = "./ASDIR/AUCTIONS";
    string msg = "RMA";
    string ok_msg = msg + " OK";
    string status = " 0";

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // User doesn't exist
        return msg + " NLG\n";
    }

    string loginFilePath = uidDir + "/" + UID + "_login.txt";
    ifstream loginFile(loginFilePath);
    if (int check = !loginFile.good()) {
        // User not logged in
        return msg + " NLG\n";
    }

    string hostedDir = uidDir + "/HOSTED";
    ifstream hostedDirCheck(hostedDir);
    if (!hostedDirCheck.good()) {
        // User doesn't exist
        return msg + " NLG\n";
    }

    if (fs::is_empty(hostedDir)) {
        // Directory is empty
        return msg + " NOK\n";
    }

    else {
        // Directory is not empty, retrieve file names without extensions
        for (const auto& entry : fs::directory_iterator(hostedDir)) {
            string AID = entry.path().stem().string();
            string aidDir = aucDir + "/" + AID;
            ifstream aidDirCheck(aidDir);
            if (!aidDirCheck.good()) {
                // User doesn't exist
                return msg + " NOK\n";
            }

            string startAIDFilePath = aidDir + "/" + "START_" + AID + ".txt";
            ifstream startAIDFile(startAIDFilePath);
            if (!startAIDFile.good()) {
                // User not logged in
                return msg + " NOK\n";
            }

            string endAIDFilePath = aidDir + "/" + "END_" + AID + ".txt";
            ifstream endAIDFile(endAIDFilePath);
            if (!endAIDFile.good()) {
                // User not logged in
                status = " 1";
            }
            else {
                status = " 0";
            }
            ok_msg = ok_msg + " " + AID + status;
        }
    }

    return ok_msg + "\n";
}

string MyBids(string UID) {
    string uidDir = "./ASDIR/USERS/" + UID;
    string aucDir = "./ASDIR/AUCTIONS";
    string msg = "RMB";
    string ok_msg = msg + " OK";
    string status = " 0";

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // User doesn't exist
        return msg + " NLG\n";
    }

    string loginFilePath = uidDir + "/" + UID + "_login.txt";
    ifstream loginFile(loginFilePath);
    if (int check = !loginFile.good()) {
        // User not logged in
        return msg + " NLG\n";
    }

    string biddedDir = uidDir + "/BIDDED";
    ifstream biddedDirCheck(biddedDir);
    if (!biddedDirCheck.good()) {
        // User doesn't exist
        return msg + " NLG\n";
    }

    if (fs::is_empty(biddedDir)) {
        // Directory is empty
        return msg + " NOK\n";
    }

    else {
        // Directory is not empty, retrieve file names without extensions
        for (const auto& entry : fs::directory_iterator(biddedDir)) {
            string AID = entry.path().stem().string();
            string aidDir = aucDir + "/" + AID;
            ifstream aidDirCheck(aidDir);
            if (!aidDirCheck.good()) {
                // User doesn't exist
                return msg + " NOK\n";
            }

            string startAIDFilePath = aidDir + "/" + "START_" + AID + ".txt";
            ifstream startAIDFile(startAIDFilePath);
            if (!startAIDFile.good()) {
                // User not logged in
                return msg + " NOK\n";
            }

            string endAIDFilePath = aidDir + "/" + "END_" + AID + ".txt";
            ifstream endAIDFile(endAIDFilePath);
            if (!endAIDFile.good()) {
                // User not logged in
                status = " 1";
            }
            else {
                status = " 0";
            }
            ok_msg = ok_msg + " " + AID + status;
        }
    }

    return ok_msg + "\n";
}

string ListAuctions() {
    string aucDir = "./ASDIR/AUCTIONS";
    string msg = "RLS";
    string status = " 0";
    string ok_msg = msg + " OK";

    if (fs::is_empty(aucDir)) {
        // Directory is empty
        return msg + " NOK\n";
    }

    else {
        // Directory is not empty, retrieve file names without extensions
        for (const auto& entry : fs::directory_iterator(aucDir)) {
            string AID = entry.path().stem().string();
            string aidDir = aucDir + "/" + AID;
            ifstream aidDirCheck(aidDir);
            if (!aidDirCheck.good()) {
                // User doesn't exist
                return msg + " NOK\n";
            }

            string startAIDFilePath = aidDir + "/" + "START_" + AID + ".txt";
            ifstream startAIDFile(startAIDFilePath);
            if (!startAIDFile.good()) {
                // User not logged in
                return msg + " NOK\n";
            }

            string endAIDFilePath = aidDir + "/" + "END_" + AID + ".txt";
            ifstream endAIDFile(endAIDFilePath);
            if (!endAIDFile.good()) {
                // User not logged in
                status = " 1";
            }
            else {
                status = " 0";
            }
            ok_msg = ok_msg + " " + AID + status;
        }
    }
    return ok_msg + "\n";
}

string show_record(string AID) {
    /*auctionDir = "./ASDIR/AUCTIONS/" + AID;
    ifstream aucDir(auctionDir);
    if (!aucDir.good()) {
        // if user does not exist, create user
        return "RRC NOK\n";
    }

    string msg = "RRC OK";

    string startFile = "./ASDIR/AUCTIONS/" + AID + "START_" + AID + ".txt";
    // Read the password from the file
    string UID, name, Fname, start_value, time_active, start_time, start_unixTime;
    ifstream fp_start(startFile);
    fp_start >> UID >> name >> Fname >> start_value >> time_active >> start_time >> start_unixTime;
    msg = msg + " " + UID + " " + name + " " + Fname + " " + start_value + " " + start_time + " " + time_active;

    string bidsDir = "./ASDIR/AUCTIONS/" + AID + "/BIDS";
    
    // Specify the path to the directory
    fs::path directory_path = bidsDir;

    // Check if the directory exists
    if (fs::exists(directory_path) && fs::is_directory(directory_path)) {
        // Iterate over the files in the directory and store their names
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            string filepath = (entry.path()).string();

            ofstream fp(filepath);

            if (!fp.is_open()) {return "RRC NOK";}

            string UID, value, time, unixTime;

            fp >> UID >> value >> time >> unixTime;
            msg = msg + " B " + UID + " " + value + " " + time + "\n";
        }
    }
    string endDir = "./ASDIR/AUCTIONS/" + AID + "/" + "END_" + AID + ".txt";
    ifstream fp_end(endDir);
    string end_time, end_unixTime;
    fp_end >> end_time >> end_unixTime;
    msg = msg + " E " + end_time + end_unixTime + "\n";
    return msg;*/
    return "RRC OK\n";
}

// TCP functions
int CreateASSETDir(string AID) {
    string ASSET_dirname = "./ASDIR/AUCTIONS/" + AID + "/ASSET";

    int ret = mkdir(ASSET_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

int CreateAIDDir(string AID) {
    string AID_dirname = "./ASDIR/AUCTIONS/" + AID;

    int ret = mkdir(AID_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

int CreateBIDSDir (string AID) {
    string BIDS_dirname = "./ASDIR/AUCTIONS/" + AID + "/BIDS";

    int ret = mkdir(BIDS_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

int CreateHostedFile(string UID, string password, string AID){
    // Construct the path to the UID_pass.txt file
    string passwordFilePath = "./ASDIR/USERS/" + UID + "/" + UID + "_pass.txt";

    // Open and check if the password file exists
    ifstream passwordFile(passwordFilePath);
    if (!passwordFile.good()) {
        // if user does not exist, create user
        return 0;
    }

    // Read the password from the file
    string storedPassword;
    passwordFile >> storedPassword;

    // Check if the provided password matches the stored password
    if (storedPassword != password) {
        // the user exists but the password is incorrect
        return 0;
    }

    string fileName = "./ASDIR/USERS/" + UID + "/HOSTED/" + AID + ".txt";
    ofstream fp_hosted(fileName);

    if (!fp_hosted.is_open()) {return 0;}

    fp_hosted << "Started auction\n";

    return 1;
}

int CreateSTARTFile(string AID, string UID, string Fname, string name, string start_value, string time_active) {
    string START_filename = "./ASDIR/AUCTIONS/" + AID + "/START_" + AID + ".txt";
    ofstream fp_start(START_filename);

    if (!fp_start.is_open()) {return 0;}
    string fulltime = to_string(time(nullptr));

    // Get the current system time
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());

    // Convert the system time to a local time structure
    tm* localTime = localtime(&currentTime);

    // Format the local time as a string
    char datetime[80];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localTime);

    fp_start << UID << " " << name << " " << Fname << " " << start_value << " " << time_active << " " << datetime << " " << fulltime;

    return 1;
}

int closeAuc(string UID, string password, string AID) {

    string uidDir = "./ASDIR/USERS";
    string aidDir = "./ASDIR/AUCTIONS";

    // Construct the path to the UID_pass.txt file
    string uidFilePath = uidDir + "/" + UID;

    // Open and check if the password file exists
    ifstream uidFile(uidFilePath);
    if (!uidFile.good()) {
        // if user does not exist
        return 0;
    }
    // Construct the path to the UID_pass.txt file
    string passwordFilePath = uidDir + "/" + UID + "/" + UID + "_pass.txt";

    // Open and check if the password file exists
    ifstream passwordFile(passwordFilePath);
    if (!passwordFile.good()) {
        // if user does not exist, create user
        return 0;
    }

    // Construct the path to the UID_login.txt file
    string loginFilePath = uidDir + "/" + UID + "/" + UID + "_login.txt";

    // Open and check if the login file exists
    ifstream loginFile(loginFilePath);
    if (!loginFile.good()) {
        // if user is not logged in
        return -1;
    }

    // Construct the path to the AID directory
    string aidDirPath = aidDir + "/" + AID;

    // Open and check if the aid directory exists
    ifstream new_aidDir(aidDirPath);
    if (!new_aidDir.good()) {
        return -2;
    }

    string endFilePath = aidDir + "/" + AID + "/" + "END_" + AID + ".txt";
    ifstream endFile(endFilePath);
    if (endFile.good()) {
        return -3;
    }

    string ownerFilePath = uidDir + "/" + UID + "/" + "HOSTED/" + AID + ".txt";
    ifstream ownerFile(ownerFilePath);
    if (!ownerFile.good()) {
        return -4;
    }

    // Read the password from the file
    string storedPassword;
    passwordFile >> storedPassword;

    // Check if the provided password matches the stored password
    if (storedPassword == password) {
        // the user exists and the password is correct
        string end_name = "./ASDIR/AUCTIONS/" + AID + "/" + "END_" + AID + ".txt";
        ofstream fp_end(end_name);
        if (!fp_end.is_open()) {return 0;}

        // Get the current date time
        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        tm* localTime = localtime(&currentTime);
        char datetime[80];
        strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localTime);

        string start_name = "./ASDIR/AUCTIONS/" + AID + "/" + "START_" + AID + ".txt";

        ifstream startFile(start_name);
        if (!startFile.good()) {
            return -2;
        }

        // Read the password from the file
        string name, Fname, start_value, time_active, start_time, start_unixTime;
        startFile >> name >> Fname >> start_value >> time_active >> start_time >> start_unixTime;
        int auctionDuration = time(nullptr) - stoi(start_unixTime);

        fp_end << datetime << " " << auctionDuration;
        return 1;
    } else {
        // the user exists but the password is incorrect
        return 0;
    }

    return 1;
}

int showAsset(string AID, int socketfd){
    int error = 0;

    string auctionDir = "./ASDIR/AUCTIONS/" + AID;

    ifstream dir(auctionDir);
    if (!dir.good()) {
        error = -1;
    }

    string START_dir = auctionDir + "/" + "START_" + AID + ".txt";
    ifstream startFile(START_dir);
    if (!startFile.good()) {
        error = -1;
    }

    string uid, name, fname; 
    startFile >> uid >> name >> fname;

    string file_dir = auctionDir + "/ASSET" + "/" + fname;

    struct stat file_stat;
    if (stat(file_dir.c_str(), &file_stat) == -1) {
        error = -1;
    }

    size_t fsize = file_stat.st_size;

    if (error == 0){
        string responseMessage = "RSA OK " + fname + " " + to_string(fsize) + " ";
        write(socketfd, responseMessage.c_str(), responseMessage.length());

        int file = open(file_dir.c_str(), O_RDONLY);
        if (file == -1) {
            error = -1;
        }
        ssize_t n = sendfile(socketfd, file, NULL, fsize);
        if (n == -1) {
            error = -1;
        }
        close(file);

        n = write(socketfd, "\n", 1);
        if (n == -1) {
            error = -1;
        }
    } 

    else {
        string responseMessage = "RSA NOK\n";
        write(socketfd, responseMessage.c_str(), responseMessage.length());
    }

    return 1;
}

string bid(string UID, string password, string AID, string value) {
    string uidDir = "./ASDIR/USERS/" + UID;
    string aidDir = "./ASDIR/AUCTIONS/" + AID + "/";

    // Open and check if the UID directory exists
    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // if user does not exist, create user
        return "RBD NOK\n";
    }

    // Construct the path to the UID_pass.txt file
    string loginFilePath = uidDir + "/" + UID + "_login.txt";

    // Open and check if the password file exists
    ifstream loginFile(loginFilePath);
    if (!loginFile.good()) {
        // if user does not exist, create user
        return "RBD NLG\n";
    }

    string endFilePath = aidDir + "END_" + AID + ".txt";
    ifstream endFile(endFilePath);
    if (endFile.good()) {
        return "RBD NOK\n";
    }

    // Specify the path to the directory
    fs::path directory_path = aidDir + "BIDS";

    // Check if the directory exists
    if (fs::exists(directory_path) && fs::is_directory(directory_path)) {
        // Create a vector to store file names in the directory
        vector<fs::path> file_names;

        // Iterate over the files in the directory and store their names
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            file_names.push_back(entry.path());
        }

        // Check if there are any files in the directory
        if (!file_names.empty()) {

            // Get the last file name (the highest value after sorting)
            fs::path last_file = file_names.back();

            string filepath = last_file.string();
            string filename = filepath.substr(filepath.length() - 10);
            int highestBid = stoi((filename).substr(0, 6));
            if (highestBid >= stoi(value)) {
                return "RBD REF\n";
            }
        }

        // Create a new file
        ostringstream bid_name;
        bid_name << setw(6) << setfill('0') << value;
        string file_name = aidDir + "BIDS/" + bid_name.str() + ".txt";
        ofstream fp_bid(file_name);

        if (!fp_bid.is_open()) {return "RDB NOK";}

        // Get the current date time
        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        tm* localTime = localtime(&currentTime);
        char datetime[80];
        strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localTime);

        string start_name = aidDir + "START_" + AID + ".txt";
        ifstream startFile(start_name);
        if (!startFile.good()) {
            return "RBD NOK\n";
        }
        string name, Fname, start_value, time_active, start_time, start_unixTime;
        startFile >> name >> Fname >> start_value >> time_active >> start_time >> start_unixTime;
        int auctionDuration = time(nullptr) - stoi(start_unixTime);

        fp_bid << UID << " " << value << " " << datetime << " " << auctionDuration ;

        if (!endFile.good()) {
            return "RBD ACC\n";
        }
    } else {
        return "RBD NOK\n";
    }
    return "RBD NOK\n";
}
