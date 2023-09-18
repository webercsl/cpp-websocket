#pragma once
#include "socketUtils.h"
#include "list.h"
#include <thread>
#include <vector>

#define MAX_REQUESTS 5
#define MAX_LIST_SIZE 5

void runServer(bool * serverRunning, int serverPortNumber, int sockfdserver, list * l);
int getServerPortNumber(int argc, char *argv[], int * serverPortNumber);
int openServer(int * sockfd, int serverPortNumber);
void processRequest(int sockfdcli, list * l);
int closeServer(bool * serverRunning, int serverSockFd, std::thread * serverThread);
const char * addFileToList(list * l, request_t * request);
