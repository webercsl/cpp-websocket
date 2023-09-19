#include "server.h"

int main(int argc, char *argv[]){
    int returnCode = 0;
    int userInput, serverPortNumber, sockfd;
    list * l = initList();
    bool serverRunning = false;
    std::thread serverThread;

    returnCode = getServerPortNumber(argc, argv, &serverPortNumber);
    if(returnCode != 0) goto leave;

    system("clear");
    while (true)
    {
        std::cout << "======== Menu ========" << std::endl <<std::endl;

        std::cout << "Status do servidor: ";
        if(serverRunning) std::cout << "Ligado";
        else std::cout << "Desligado";

        std::cout << std::endl << "Porta: " << serverPortNumber << std::endl;

        std::cout << std::endl ;

        std::cout << "[1] - Abrir servidor" << std::endl;
        std::cout << "[2] - Fechar servidor" << std::endl;
        std::cout << "[3] - Ver fila de impressão" << std::endl;
        std::cout << "[4] - Imprimir" << std::endl;
        std::cout << "[0] - Sair" << std::endl << std::endl;
        std::cout << "Digite a opção desejada: " << std::endl;

        std::cin >> userInput;
        system("clear");
        switch(userInput){
            case 0:
                goto leave;
                break;
            case 1:
                if(!serverRunning){
                    if(openServer(&sockfd, serverPortNumber) == 0){
                        serverRunning = true;
                        serverThread = std::thread(runServer, &serverRunning, serverPortNumber, sockfd, l);
                    }
                }else{
                    std::cout << "O servidor já está no ar!" << std::endl;
                }
                break;
            case 2:
                if(closeServer(&serverRunning, sockfd, &serverThread) == 0){
                    std::cout << "Servidor fechado com sucesso." << std::endl;
                }else{
                    std::cout << "Erro ao fechar o servidor." << std::endl;
                }
                break;
            case 3:
                std::cout << getListString(l) << std::endl;
                break;
            case 4:
                std::cout << printListToFile(l) << std::endl;
                break;
            default:
                std::cout << "Digite uma entrada válida!" << std::endl;
                continue;
        }
        pressAnyKeyToContinue();
        system("clear");
    }

leave:
    closeServer(&serverRunning, sockfd, &serverThread);
    clearList(l);
    free(l);

    return returnCode;
}

int closeServer(bool * serverRunning, int serverSockFd, std::thread * serverThread){
    if(*serverRunning){
        shutdown(serverSockFd, SHUT_RDWR);
        *serverRunning = false;
        serverThread->join();

        return 0;
    }

    return 1;
}

int openServer(int * sockfd, int serverPortNumber){
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0){
        std::cout << "ERRO: não foi possível criar socket." << std::endl;
        return 1;
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(serverPortNumber);

    int bindResult = bind(*sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if(bindResult < 0){
        std::cout << "ERRO: Erro ao fazer bind." << std::endl;
        return 1;
    }

    listen(*sockfd, MAX_REQUESTS);
    std::cout << "Servidor aberto na porta: " << serverPortNumber << std::endl;
    return 0;
}

void runServer(bool * serverRunning, int serverPortNumber, int sockfdserver, list * l){
    int sockfdcli;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    std::vector <std::thread> clientsProcessingThreads;
    std::vector <int> clientsSocksFds;

    while (*serverRunning)
    {
        clilen = sizeof(cli_addr);
        sockfdcli = accept(sockfdserver, (struct sockaddr *) &cli_addr, &clilen);
        if(sockfdcli != -1){
            clientsSocksFds.push_back(sockfdcli);
            std::thread clientThread(processRequest, sockfdcli, l);
            clientsProcessingThreads.push_back(std::move(clientThread));
        }
    }

    for(int i = 0; i < clientsSocksFds.size(); i++){
        shutdown(clientsSocksFds.at(i), SHUT_RD);
    }

    for(int i = 0; i < clientsProcessingThreads.size(); i++){
        clientsProcessingThreads.at(i).join();
    }

    for(int i = 0; i < clientsSocksFds.size(); i++){
        shutdown(clientsSocksFds.at(i), SHUT_WR);
    }
}

void processRequest(int sockfdcli, list * l){
    char buffer[REQUEST_BUFFER_SIZE];

    while(true){
        bzero(buffer,REQUEST_BUFFER_SIZE);

        int readResult = read(sockfdcli,buffer,REQUEST_BUFFER_SIZE);
        if (readResult < 0 || strlen(buffer) == 0) {
            int writeResult = write(sockfdcli,"Erro. Impossível ler do cliente. Esse problema pode ocorrer quando o servidor foi fechado com uma conexão ativa.", 116);
            return;
        }

        request_t * request = strToRequest(buffer);
        bzero(buffer, REQUEST_BUFFER_SIZE);
        switch(request->op){
            case ADD:
                strcpy(buffer, addFileToList(l, request));
                break;
            case LIST:
                strcpy(buffer, getListString(l).c_str());
                break;
            case PRINT:
                strcpy(buffer, printListToFile(l));
                break;
            default:
                strcpy(buffer, "Operação não reconhecida");
                break;
        }

        free(request);
        int writeResult = write(sockfdcli, buffer, strlen(buffer));
        if (writeResult < 0){
            //!!!
            return;
        }
    }
}

const char * addFileToList(list * l, request_t * request){
    if(l->size == MAX_LIST_SIZE){
        return "Pedido negado. Número máximo de 5 arquivos atingido.";
    }else{
        add(l, createNodeValue(request->name, request->content, request->date));
        return "Registro adicionado a fila com sucesso.";
    }
}

const char * printListToFile(list * l){
    if(empty(l))
        return "Lista vazia!";

    std::ofstream outfile;
    outfile.open(FILE_PATH, std::ios_base::app);

    for(node * searchNode = l->first; searchNode != NULL; searchNode = searchNode->next){
        outfile << searchNode->value->date << " - " << searchNode->value->name << " / " << searchNode->value->content << std::endl;
    }

    clearList(l);
    outfile.close();
    return "Lista impressa com sucesso.";
}

int getServerPortNumber(int argc, char *argv[], int * serverPortNumber){
    if (argc < 2) {
        std::cout << "ERRO: Não foi passado o número da porta para o servidor." << std::endl;
        return 1;
    }

    *serverPortNumber = atoi(argv[1]);
    return 0;
}