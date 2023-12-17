#include <stdlib.h>
#include <stdio.h>
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
#include <filesystem>
#include <netinet/in.h>
#include <vector>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    string file = "000200.txt";

    int a = stoi(file.substr(0, 6));

    cout << a << endl;

    return 0;
}