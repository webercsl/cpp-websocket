#pragma once
#include "socketUtils.h"
#include "queue.h"
#include <thread>
#include <vector>
#include <fstream>

#define FILE_PATH "printedQueue.txt"
#define MAX_REQUESTS 5
#define MAX_QUEUE_SIZE 5

void runServer(bool * serverRunning, int serverPortNumber, int sockfdserver, queue * q);
int getServerPortNumber(int argc, char *argv[], int * serverPortNumber);
int openServer(int * sockfd, int serverPortNumber);
void processRequest(int sockfdcli, queue * q);
int closeServer(bool * serverRunning, int serverSockFd, std::thread * serverThread);
const char * addFileToQueue(queue * q, request_t * request);
const char * printQueueToFile(queue * q);