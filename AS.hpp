#ifndef __AS_H__
#define __AS_H__

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
namespace fs = std::filesystem;
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
string MyBids(string UID);
string ListAuctions();
string show_record(string UID);
int CreateASSETDir(string AID);
int CreateAIDDir(string AID);
int CreateBIDSDir (string AID);
int CreateSTARTFile(string AID, string UID, string Fname, string name, string start_value, string time_active);

#endif // __AS_H__