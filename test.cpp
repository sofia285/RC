#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <sys/stat.h>


using namespace std;

int main(int argc, char **argv)
{
    string rel_path = "/mnt/c/Users/2003s/OneDrive/Documentos/ist/RC/RC/";

    string uidDir = rel_path + "ASDIR/USERS/" + "112233";

    ifstream uidDirCheck(uidDir);
    if (!uidDirCheck.good()) {
        cout << "failed" <<endl;
        // User doesn't exist
        return 0;
    }
    cout << "success" <<endl;

}