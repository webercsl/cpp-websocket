#pragma once
#include "socketUtils.h"
#include "list.h"
#include <thread>
#include <vector>
#include <fstream>

#define FILE_PATH "printedList.txt"
#define MAX_REQUESTS 5
#define MAX_LIST_SIZE 5

void runServer(bool * serverRunning, int serverPortNumber, int sockfdserver, queue * q);
int getServerPortNumber(int argc, char *argv[], int * serverPortNumber);
int openServer(int * sockfd, int serverPortNumber);
void processRequest(int sockfdcli, queue * q);
int closeServer(bool * serverRunning, int serverSockFd, std::thread * serverThread);
const char * addFileToList(queue * q, request_t * request);
const char * printListToFile(queue * q);