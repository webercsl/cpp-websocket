#include "socketUtils.h"
#include <thread>
#include <chrono>

void runServer(bool * stop);
int getServerPortNumber(int argc, char *argv[], int * serverPortNumber);

int main(int argc, char *argv[]){
    int returnCode = 0;
    int userInput, serverPortNumber;
    bool serverRunning = false;
    std::thread serverThread;

    returnCode = getServerPortNumber(argc, argv, &serverPortNumber);
    if(returnCode != 0) goto leave;

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
                //Talvez Passar para função.
                if(!serverRunning){
                    serverRunning = true;
                    serverThread = std::thread(runServer, &serverRunning);
                }else{
                    std::cout << "O servidor já esta no ar!" << std::endl;
                }
                break;
            case 2:
                serverRunning = false;//passar para função e add um textinho
                serverThread.join();
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

    if(serverRunning){
        serverRunning = false;
        serverThread.join();
    }

    return returnCode;
}

void runServer(bool * serverRunning){
    //iniciar server
    //se o port p=for passado via args sera necessario receber ele na funcao

    while (*serverRunning)
    {
        //esperar requests
        
    }

    //fechar portas
}

int getServerPortNumber(int argc, char *argv[], int * serverPortNumber){
    if (argc < 2) {
        std::cout << "ERRO: Não foi passado o número da porta para o servidor." << std::endl;
        return 1;
    }

    *serverPortNumber = atoi(argv[1]);
    return 0;
}