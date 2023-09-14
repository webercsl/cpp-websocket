#pragma once
#include <iostream>

//funções/includes uteis tanto para o server quanto para o client

void pressAnyKeyToContinue(){
    std::cout << std::endl << "Pressione qualquer tecla para continuar..";
    std::cin.ignore().get();
}