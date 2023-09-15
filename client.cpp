#include "client.h"

int main(int argc, char *argv[]){
    int returnCode = 0;
    bool connected = false;
    int userInput, hostPortNumber, sockfd;

    returnCode = getServerPortNumber(argc, argv, &hostPortNumber);
    if(returnCode != 0) goto leave;

    system("clear");
    while (true)
    {
        std::cout << "======== Menu ========" << std::endl <<std::endl;

        std::cout << "Endereço do Host: " << argv[1] << std::endl;
        std::cout << "Porta do Host: " << hostPortNumber << std::endl;

        std::cout << "Conexão com o servidor: ";
        if(connected) std::cout << "Conectado";
        else std::cout << "Desconectado";
        std::cout << std::endl;

        std::cout << std::endl;

        std::cout << "[1] - Estabelecer conexão com o servidor" << std::endl;
        std::cout << "[2] - Enviar arquivo para impressão" << std::endl;
        std::cout << "[3] - Ver fila de impressão" << std::endl;
        std::cout << "[0] - Sair" << std::endl << std::endl;
        std::cout << "Digite a opção desejada: " << std::endl;
        
        std::cin >> userInput;
        system("clear");
        switch(userInput){
            case 0:
                goto leave;
                break;
            case 1:
                if(!connected){
                    connected = establishConnection(argv[1], hostPortNumber, &sockfd) == 0;
                    if(connected) std::cout << "Conexão estabelecida com sucesso." << std::endl;
                }else{
                    std::cout << "Conexão com o servidor já foi estabelecida!" << std::endl;
                }
                break;
            case 2:
                if(connected){
                    sendNewAddRequest(sockfd);
                }else{
                    std::cout << "Conexão com o servidor não foi estabelecida." << std::endl;
                }
                break;
            default:
                std::cout << "Digite uma entrada válida!" << std::endl;
                continue;
        }
        pressAnyKeyToContinue();
        system("clear");
    }

leave:
    if(connected){
        shutdown(sockfd, SHUT_RDWR);
    }
    return returnCode;
}

int establishConnection(char * hostName, int hostPortNumber, int * sockfd){
    struct sockaddr_in serverAddress;
    struct hostent *server = gethostbyname(hostName);

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0){
        std::cout << "ERRO: não foi possível abrir o socket." << std::endl;
        return 1;
    }
    
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(hostPortNumber);

    if (connect(*sockfd,(struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0){
        std::cout << "ERRO: não foi possível estabelecer uma conexão com o host." << std::endl;
        return 1;
    }

    return 0;
} 

int registerNewRequest(request_t ** request){
    *request = (request_t *) malloc(sizeof(request_t));

    std::cout << "Digite o seu nome:" << std::endl;
    std::cin.ignore().getline((*request)->name, sizeof((*request)->name));

    std::cout << "Digite o conteúdo do arquivo" << std::endl;//Mudar para local do arquivo?
    std::cin.getline((*request)->content, sizeof((*request)->content));

    strcpy((*request)->date, getDateNowStr().c_str());
    system("clear");
    return 0;
}

int sendNewAddRequest(int sockfd){
    request_t * request;
    char buffer[REQUEST_BUFFER_SIZE];
    bzero(buffer, REQUEST_BUFFER_SIZE);

    registerNewRequest(&request);
    request->op = ADD;
    strcpy(buffer, requestToStr(request).c_str());

    int writeResult = write(sockfd, buffer, REQUEST_BUFFER_SIZE);
    if(writeResult < 0){
        std::cout << "ERRO: Não foi possível realizar a escrita." <<std::endl;
        return 1;
    }

    bzero(buffer, REQUEST_BUFFER_SIZE);
    int readResult = read(sockfd, buffer, REQUEST_BUFFER_SIZE);
    std::cout << buffer << std::endl;
    return 0;
}

int getServerPortNumber(int argc, char *argv[], int * hostPortNumber){
    if (argc < 3) {
        std::cout << "ERRO: Argumentos insuficientes. Uso: " << argv[0] << " [endereço do host] [porta]" << std::endl;
        return 1;
    }

    if (gethostbyname(argv[1]) == NULL) {
        std::cout << "ERRO: Endereço do host não encontrado. " << std::endl;
        return 1;
    }

    *hostPortNumber = atoi(argv[2]);
    return 0;
}