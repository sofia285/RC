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
    // Get the current system time
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());

    // Convert the system time to a local time structure
    tm* localTime = localtime(&currentTime);

    // Format the local time as a string
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

    // Print the formatted date and time
    cout << "Current Date and Time: " << buffer << std::endl;

    cout << time(nullptr) << endl;

    return 0;
}