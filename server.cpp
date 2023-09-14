#include "socketUtils.h"
#include <thread>
#include <chrono>
//talvez adicionar um server.h, contendo os includes, e declaração de funções

void task1(bool * stop);

int main(){
    int userInput, returnCode = 0;
    bool stop = false;
    std::thread serverThread;

    while (true)
    {
        std::cout << "Menu:" << std::endl << std::endl;
        std::cout << "[1] - Abrir servidor" << std::endl;
        std::cout << "[2] - Fechar servidor" << std::endl;
        std::cout << "[3] - Status servidor" << std::endl;
        std::cout << "[4] - Mostrar lista" << std::endl;
        std::cout << "[0] - Sair" << std::endl;
        
        std::cin >> userInput;
        system("clear");
        switch(userInput){
            case 0:
                goto leave;
                break;
            case 1:
                serverThread = std::thread(task1, &stop);
                break;
            case 2:
                stop = true;
                break;
            case 3:
                
                break;
            case 4:
                
                break;
            default:
                std::cout << "Digite uma entrada válida!" << std::endl;
                continue;
        }
        pressAnyKeyToContinue();
        system("clear");
    }

leave:
    serverThread.join();
    return returnCode;
}

void task1(bool * stop){
    using namespace std::chrono_literals;
    while (!*stop)
    {
        std::cout << "isso é um teste";
        std::cout.flush();
        std::this_thread::sleep_for(2000ms);
    }
}