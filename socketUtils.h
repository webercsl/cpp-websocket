#pragma once
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>


void pressAnyKeyToContinue(){
    std::cout << std::endl << "Pressione qualquer tecla para continuar..";
    std::cin.ignore().get();
}