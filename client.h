#pragma once
#include "socketUtils.h"
#include <netdb.h>

int getServerPortNumber(int argc, char *argv[], int * hostPortNumber);
int establishConnection(char * hostName, int hostPortNumber, int * sockfd);
int registerNewRequest(request_t ** request);
int sendNewAddRequest(int sockfd);