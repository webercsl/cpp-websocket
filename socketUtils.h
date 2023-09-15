#pragma once
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#define REQUEST_BUFFER_SIZE 174

enum op_codes{
    ADD = 0,
    LIST = 1
};

struct request_t{
    op_codes op;
    char name[50], content[100], date[19];
};

const char * requestToStr(request_t * request){
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

    return requestText.c_str();
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

void pressAnyKeyToContinue(){
    std::cout << std::endl << "Pressione qualquer tecla para continuar..";
    std::cin.ignore().get();
}