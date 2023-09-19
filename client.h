#pragma once
#include "socketUtils.h"
#include <netdb.h>

int getServerPortNumber(int argc, char *argv[], int * hostPortNumber);
int establishConnection(char * hostName, int hostPortNumber, int * sockfd);
int registerNewAddRequest(request_t ** request);
int sendNewAddRequest(int sockfd);
int sendRequest(int sockfd, request_t * request);
int sendNewListRequest(int sockfd);
int sendNewPrintRequest(int sockfd);