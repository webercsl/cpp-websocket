#pragma once
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <ctime>
#include <iomanip>

#define REQUEST_BUFFER_SIZE 174

enum op_codes{
    ADD = 0,
    LIST = 1
};

struct request_t{
    op_codes op;
    char name[50], content[100], date[19];
};

std::string requestToStr(request_t * request){
    if(request == NULL)
        return "";

    std::string requestText = "";
    requestText += std::to_string((int)request->op);
    requestText += std::string(";");
    
    requestText += std::string(request->name);
    requestText += std::string(";");

    requestText += std::string(request->content);
    requestText += std::string(";");

    requestText += std::string(request->date);
    requestText += std::string(";");

    return requestText;
}

request_t * strToRequest(char * str){
    request_t * request = (request_t *)malloc(sizeof(request_t));

    char * slice;
    slice = strtok(str, ";");
    request->op = (op_codes)atoi(slice);

    slice = strtok(NULL, ";");
    strcpy(request->name, slice);

    slice = strtok(NULL, ";");
    strcpy(request->content, slice);

    slice = strtok(NULL, ";");
    strcpy(request->date, slice);

    return request;
}

std::string getDateNowStr(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    auto timeStr = oss.str();

    return timeStr;
}

void pressAnyKeyToContinue(){
    std::cout << std::endl << "Pressione qualquer tecla para continuar..";
    std::cin.ignore().get();
}