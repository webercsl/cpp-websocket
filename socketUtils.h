#pragma once
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <ctime>
#include <iomanip>

#define REQUEST_BUFFER_SIZE 2048//174 por request, tamanho de 2048 para o retorno da lista

enum op_codes{
    ADD = 0,
    LIST = 1
};

struct request_t{
    op_codes op;
    char name[50], content[100], date[19];
};

std::string getDateNowStr(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    auto timeStr = oss.str();

    return timeStr;
}

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

request_t * initRequest(){
    request_t * request = (request_t *)malloc(sizeof(request_t));
    bzero(request->name, sizeof(request->name));
    bzero(request->content, sizeof(request->content));
    bzero(request->date, sizeof(request->date));
    strcpy(request->date, getDateNowStr().c_str());

    return request;
}

request_t * strToRequest(char * str){
    request_t * request = initRequest();

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