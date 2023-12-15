#include "AS.hpp"

using namespace std;

string rel_path = "/mnt/c/Users/2003s/OneDrive/Documentos/ist/RC/RC/"; // TODO: change this to the correct path

int CreateHOSTEDDir(string UID);
int CreateBIDDEDDir(string UID);
int CreateUSERSDir(string UID);
int removeUSERSDir(string UID);
int CreatePassword(string UID, string password);
int ErasePassword(string UID, string password);
int CreateLogin(string UID);
int EraseLogin(string UID);
int RegisterUser(string UID, string password);
int UnregisterUser(string UID, string password);
int LoginUser(string UID, string password);
int LogoutUser(string UID, string password);
string MyAuctions(string UID);


int main(int argc, char **argv){
    char *pvalue = NULL;
    char *vvalue = NULL;
    int c;
    string command;
    string curr_user, curr_pass;
    int serverPort = 58031; // Default server port

    opterr = 0;

    while ((c = getopt(argc, argv, "p:v:")) != -1) {
        switch (c) {
        case 'p':
            pvalue = optarg;
            break;
        case 'v':
            vvalue = optarg;
            break;
        case '?':
            if (optopt == 'p' || optopt == 'v')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
            default:
                exit(1);
        }
    }

    if (pvalue && (strcmp(pvalue, "-v") == 0 || strcmp(pvalue, "-p") == 0)) {
        fprintf(stderr, "Option -p requires an argument.\n");
        exit(1);
    }
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            serverPort = stoi(argv[i + 1]);
        }
    }

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        perror("Error creating server socket");
        return -1;
    }

    // Bind the socket to a specific port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding server socket");
        close(serverSocket);
        return -1;
    }

    cout << "Server listening on port " << serverPort << endl;

    // Receive and print messages from clients
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    string responseMessage;

    while (1) {
        char buf[128] = {0};
        //memset(command, 0, 128);
        ssize_t bytesRead = recvfrom(serverSocket, buf, sizeof(buf), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (bytesRead == -1) {
            perror("Error receiving data");
        } 
        printf("bytes read: %ld\n", bytesRead);
        printf("buf: %s\n", buf);

        istringstream iss(buf);
        iss >> command;

        // Login
        if (command == "LIN"){
            string username, password;
            iss >> username >> password;
            int userCheck = LoginUser(username, password);
            if (userCheck == -1){
                cout << "error" << endl;
                responseMessage = "RLI NOK\n";
            }
            else if (userCheck == 0) {
                cout << username << endl;
                if (RegisterUser(username, password) == 0) {
                    cout << "error registering" << endl;
                    responseMessage = "RLI NOK\n";
                }
                else {
                    responseMessage = "RLI REG\n";
                }
            }
            else if (userCheck == 1) {
                if (CreateLogin(username) == 0) {
                    cout << "error here" << endl;
                    responseMessage = "RLI NOK\n";
                }
                else {
                    responseMessage = "RLI OK\n";
                }
            }
        }

        if (command == "LOU") {
            string username, password;
            iss >> username >> password;
            int userCheck = LogoutUser(username, password);
            if (userCheck == -1){
                responseMessage = "RLO NOK\n";
            }
            else if (userCheck == 0) {
                responseMessage = "RLO UNR\n";
            }
            else if (userCheck == 1) {
                cout << "here" << endl;
                if (EraseLogin(username) == 0) {
                    responseMessage = "RLO NOK\n";
                }
                else {
                    responseMessage = "RLO OK\n";
                }
            }
        }

        // Unregister
        if (command == "UNR") {
            string username, password;
            iss >> username >> password;
            int userCheck = UnregisterUser(username, password);
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

        // my auctions
        if (command == "LMA") {
            string username;
            iss >> command >> username;

        }
        // Send the response back to the client
        cout << "Sending response: " << responseMessage << endl;
        cout << "responseMessage.length(): " << responseMessage.length() << endl;
        cout << "ASCII: " << (int)responseMessage[6] << " and " << (int)responseMessage[7] << endl;
        sendto(serverSocket, responseMessage.c_str(), responseMessage.length(), 0,  (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    }


    // Close the socket (never reached in this example)
    close(serverSocket);

    
   return 0;
}

int CreateHOSTEDDir(string UID) {
    string HOSTED_dirname = rel_path + "ASDIR/USERS/" + UID + "/HOSTED";

    int ret = mkdir(HOSTED_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

int CreateBIDDEDDir(string UID) {
    string BIDDED_dirname = rel_path + "ASDIR/USERS/" + UID + "/BIDDED";

    int ret = mkdir(BIDDED_dirname.c_str(), 0700);

    if (ret == -1) {
        return 0;
    }

    return 1;
}

// Create a new user UID directory
int CreateUSERSDir(string UID) {
    string UID_dirname = rel_path + "ASDIR/USERS/" + UID;
    cout << "UID dir name: " << UID_dirname << endl;

    int ret = mkdir(UID_dirname.c_str(),  0700);

    if (ret == -1) {
        cout << "Error making dir" << endl;
        return 0;
    }

    return 1;
}

int removeUSERSDir(string UID) {
    string UID_dirname = rel_path + "ASDIR/USERS/" + UID;

    int ret = rmdir(UID_dirname.c_str());
    if (ret == -1) {
        return 0;
    }

    return 1;
}

// Create a user's password file
int CreatePassword(string UID, string password) {
    string pass_name = rel_path + "ASDIR/USERS/" + UID + "/" + UID + "_pass.txt";
    ofstream fp_pass(pass_name);

    if(!fp_pass.is_open()) {return 0;}

    fp_pass << password;

    return 1;
}

// Erase a user's password file
int ErasePassword(string UID, string password) {
    int result;
    string passwordFilePath = rel_path + "ASDIR/USERS/" + UID + "/" + UID + "_pass.txt";

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
    string login_name = rel_path + "ASDIR/USERS/" + UID + "/" + UID + "_login.txt";
    ofstream fp_login(login_name);

    if (!fp_login.is_open()) {return 0;}

    fp_login << "Logged in\n";

    return 1;
}


// Erase a user's login file
int EraseLogin(string UID){
    string login_name = rel_path + "ASDIR/USERS/" + UID + "/" + UID + "_login.txt";
    int result = remove(login_name.c_str());

    return (result == 0) ? 1 : 0;
}

int RegisterUser(string UID, string password) {
    cout << "Registering user..." << endl;
    cout << UID << endl;
    if (CreateUSERSDir (UID) == 0){
        return 0;
    }
    cout << "Created user dir" << endl;

    if (CreateLogin(UID) == 0) {
        return 0;
    }
    cout << "Created user login" << endl;

    if (CreatePassword(UID, password) == 0) {
        return 0;
    }
    cout << "Created user" << endl;

    if (CreateHOSTEDDir(UID) == 0) {
        return 0;
    }
    cout << "Created host dir" << endl;

    if (CreateBIDDEDDir(UID) == 0) {
        return 0;
    }
    cout << "Created bid dir" << endl;
    
    return 1;
}

int UnregisterUser(string UID, string password) {
    string uidDir = rel_path + "ASDIR/USERS/" + UID;

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        cout << "failed to unregister user: " << uidDir << endl;
        // User doesn't exist
        return 0;
    }

    int userCheck = LogoutUser(UID, password);

    if (userCheck == -1) {
        cout << "here20" << endl;
        // User not logged in
        return -1;
    } else if (userCheck == 0) {
        // User not registered
        return 0;
    }
    userCheck = ErasePassword(UID, password);
    if (userCheck == 0 || userCheck == -1) {
        cout << "here21" << endl;
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
    string uidDir = rel_path + "ASDIR/USERS/" + UID;

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
    string uidDir = rel_path + "ASDIR/USERS/" + UID;

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
        // User exists, was logged in and the password is correct
        return 1;
    } else {
        // User exists but password is incorrect
        return -1;
    }
}

string MyAuctions(string UID) {
    string uidDir = rel_path + "ASDIR/USERS/" + UID;

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        // User doesn't exist
        return "NLG";
    }

    string loginFilePath = uidDir + "/" + UID + "_login.txt";
    ifstream loginFile(loginFilePath);
    if (!loginFile.good()) {
        // User not logged in
        return "NLG";
    }

    return "AAAA";
}

