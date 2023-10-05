#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string.h>
#include <ctime>
#include <iomanip>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#define REQUEST_BUFFER_SIZE 2048

enum op_codes {
    ADD = 0,
    LIST = 1,
    PRINT = 2
};

struct request_t {
    op_codes op;
    char nome[50], informacao[100], data[20];
};

int portaServidor(int argc, char* argv[], int* numeroPorta);
int estabelecerConexao(char* hostName, int numeroPorta, SOCKET* sockfd);
int registrarNovaRequisicao(request_t** request);
int enviarNovaRequisicao(SOCKET sockfd);
int enviarNovaListaRequisicao(SOCKET sockfd);
int enviarNovaRequisicaoImpressao(SOCKET sockfd);
int enviarRequisicao(SOCKET sockfd, request_t* request);

std::string pegarData() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    auto timeStr = oss.str();
    
    return timeStr;
}

std::string requestToStr(request_t* request) {
    if (request == NULL)
        return "";

    std::string requestText = "";
    requestText += std::to_string((int)request->op);
    requestText += std::string(";");

    requestText += std::string(request->nome);
    requestText += std::string(";");

    requestText += std::string(request->informacao);
    requestText += std::string(";");

    requestText += std::string(request->data);
    requestText += std::string(";");

    return requestText;
}

request_t* initRequest() {
    request_t* request = (request_t*)malloc(sizeof(request_t));
    memset(request->nome, 0, sizeof(request->nome));
    memset(request->informacao, 0, sizeof(request->informacao));
    memset(request->data, 0, sizeof(request->data));
    strcpy_s(request->data, sizeof(request->data), pegarData().c_str());

    return request;
}

request_t* strToRequest(char* str) {
    request_t* request = initRequest();

    char* slice;
    slice = strtok(str, ";");
    request->op = (op_codes)atoi(slice);

    slice = strtok(NULL, ";");
    strcpy_s(request->nome, sizeof(request->nome), slice);

    slice = strtok(NULL, ";");
    strcpy_s(request->informacao, sizeof(request->informacao), slice);

    slice = strtok(NULL, ";");
    strcpy_s(request->data, sizeof(request->data), slice);

    return request;
}

void pressAnyKeyToContinue() {
    std::cout << std::endl << "Pressione qualquer tecla para continuar..";
    std::cin.ignore().get();
}

int main(int argc, char* argv[]) {
    int returnCode = 0;
    bool connected = false;
    int userInput, numeroPorta;
    SOCKET sockfd;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "ERRO: Falha ao inicializar o Winsock." << std::endl;
        return 1;
    }

    returnCode = portaServidor(argc, argv, &numeroPorta);
    if (returnCode != 0) goto leave;

    system("cls");
    while (true) {
        std::cout << "======== Menu ========" << std::endl << std::endl;

        std::cout << "Endereco do Host: " << argv[1] << std::endl;
        std::cout << "Porta do Host: " << numeroPorta << std::endl;

        std::cout << "Conexao com o servidor: ";
        if (connected) std::cout << "Conectado";
        else std::cout << "Desconectado";
        std::cout << std::endl;

        std::cout << std::endl;

        std::cout << "[1] - Estabelecer conexao com o servidor" << std::endl;
        std::cout << "[2] - Enviar arquivo para impressao" << std::endl;
        std::cout << "[3] - Ver fila de impressao" << std::endl;
        std::cout << "[4] - Imprimir" << std::endl;
        std::cout << "[0] - Sair" << std::endl << std::endl;
        std::cout << "Digite a opcao desejada: " << std::endl;

        std::cin >> userInput;
        system("cls");
        switch (userInput) {
        case 0:
            goto leave;
            break;
        case 1:
            if (!connected) {
                connected = estabelecerConexao(argv[1], numeroPorta, &sockfd) == 0;
                if (connected) std::cout << "Conexao estabelecida com sucesso." << std::endl;
            }
            else {
                std::cout << "Conexao com o servidor ja foi estabelecida!" << std::endl;
            }
            break;
        case 2:
            if (connected) {
                enviarNovaRequisicao(sockfd);
            }
            else {
                std::cout << "Conexao com o servidor nao foi estabelecida." << std::endl;
            }
            break;
        case 3:
            if (connected) {
                enviarNovaListaRequisicao(sockfd);
            }
            else {
                std::cout << "Conexao com o servidor nao foi estabelecida." << std::endl;
            }
            break;
        case 4:
            if (connected) {
                enviarNovaRequisicaoImpressao(sockfd);
            }
            else {
                std::cout << "Conexao com o servidor nao foi estabelecida." << std::endl;
            }
            break;
        default:
            std::cout << "Digite uma entrada valida!" << std::endl;
            continue;
        }
        pressAnyKeyToContinue();
        system("cls");
    }

leave:
    if (connected) {
        shutdown(sockfd, SD_BOTH);
        closesocket(sockfd);
    }

    WSACleanup();
    return returnCode;
}

int estabelecerConexao(char* hostName, int numeroPorta, SOCKET* sockfd) {
    struct sockaddr_in serverAddress;
    struct hostent* server = gethostbyname(hostName);

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == INVALID_SOCKET) {
        std::cerr << "ERRO: Nao foi possivel abrir o socket." << std::endl;
        return 1;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;

    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr, server->h_length);
    serverAddress.sin_port = htons(numeroPorta);

    if (connect(*sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "ERRO: Nao foi possivel estabelecer uma conexao com o host." << std::endl;
        return 1;
    }

    return 0;
}

int registrarNovaRequisicao(request_t ** request){
    *request = initRequest();

    std::cout << "Digite o seu nome:" << std::endl;
    std::cin.ignore().getline((*request)->nome, sizeof((*request)->nome));
    if(strlen((*request)->nome) == 0)
        strcpy((*request)->nome, "-");

    std::cout << "Digite o conteudo do arquivo:" << std::endl;
    std::cin.getline((*request)->informacao, sizeof((*request)->informacao));
    if(strlen((*request)->informacao) == 0)
        strcpy((*request)->informacao, "-");

    (*request)->op = ADD;
    system("cls");
    return 0;
}

int enviarNovaRequisicao(SOCKET sockfd){
    request_t * request;
    registrarNovaRequisicao(&request);
    return enviarRequisicao(sockfd, request);
}

int enviarNovaListaRequisicao(SOCKET sockfd){
    request_t * request;
    request = initRequest();
    request->op = LIST;
    strcpy(request->nome, "-");
    strcpy(request->informacao, "-");
    return enviarRequisicao(sockfd, request);
}

int enviarNovaRequisicaoImpressao(SOCKET sockfd){
    request_t * request;
    request = initRequest();
    request->op = PRINT;
    strcpy(request->nome, "-");
    strcpy(request->informacao, "-");
    return enviarRequisicao(sockfd, request);
}

int enviarRequisicao(SOCKET sockfd, request_t* request) {
    char buffer[REQUEST_BUFFER_SIZE];
    memset(buffer, 0, REQUEST_BUFFER_SIZE);

    strcpy_s(buffer, REQUEST_BUFFER_SIZE, requestToStr(request).c_str());

    int writeResult = send(sockfd, buffer, REQUEST_BUFFER_SIZE, 0); // Usamos send em vez de write
    if (writeResult == SOCKET_ERROR) {
        std::cerr << "ERRO: Nao foi possivel realizar a escrita." << std::endl;
        return 1;
    }

    memset(buffer, 0, REQUEST_BUFFER_SIZE); // Limpe o buffer antes de usá-lo novamente
    int readResult = recv(sockfd, buffer, REQUEST_BUFFER_SIZE, 0); // Usamos recv em vez de read
    if (readResult == SOCKET_ERROR) {
        std::cerr << "ERRO: Nao foi possivel receber dados do servidor." << std::endl;
        return 1;
    }

    std::cout << buffer << std::endl;

    free(request);
    return 0;
}


int portaServidor(int argc, char *argv[], int * numeroPorta){
    if (argc < 3) {
        std::cout << "ERRO: Argumentos insuficientes. Uso: " << argv[0] << " [endereco do host] [porta]" << std::endl;
        return 1;
    }

    if (gethostbyname(argv[1]) == NULL) {
        std::cout << "ERRO: Endereco do host nao encontrado. " << std::endl;
        return 1;
    }

    *numeroPorta = atoi(argv[2]);
    return 0;
}