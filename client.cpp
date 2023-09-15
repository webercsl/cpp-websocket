#include "client.h"

int main(int argc, char *argv[]){
    int returnCode = 0;
    int userInput;

    while (true)
    {
        std::cout << "Menu:" << std::endl << std::endl;
        std::cout << "[1] - Enviar arquivo para impressão" << std::endl;
        std::cout << "[2] - Ver fila de impressão" << std::endl;
        std::cout << "[0] - Sair" << std::endl;
        
        std::cin >> userInput;
        system("clear");
        switch(userInput){
            case 0:
                goto leave;
                break;
            case 1:

                break;
            case 2:

                break;
            default:
                std::cout << "Digite uma entrada válida!" << std::endl;
                continue;
        }
        pressAnyKeyToContinue();
        system("clear");
    }
    


leave:
    return returnCode;
}