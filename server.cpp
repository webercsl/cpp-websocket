#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

#define FILE_PATH "impressao.txt"
#define MAX_REQUESTS 5
#define MAX_QUEUE_SIZE 5
#define REQUEST_BUFFER_SIZE 2048 // 174 por request, tamanho de 2048 para o retorno da fila

// STRUCTS
enum op_codes {
    ADD = 0,
    LIST = 1,
    PRINT = 2
};

struct request_t {
    op_codes op;
    char nome[50], informacao[100], data[20];
};

struct nodeValue {
    char* nome, * informacao, * data;
};

struct node {
    struct nodeValue* value;
    struct node* next;
};

struct queue {
    int size;
    node* first, * last;
};

// Prototipação
std::string pegarData();
std::string requestToStr(request_t* request);
request_t* iniciarRequisicao();
request_t* strToRequest(char* str);
void continuar();
nodeValue* criarValorNodo(char* nome, char* informacao, char* data);
void add(queue* fila, nodeValue* val);
queue* iniciarFila();
bool vazio(queue* fila);
void imprimirValorNodo(nodeValue* n);
std::string getQueueString(queue* fila);
nodeValue* remove(queue* fila);
void limparFila(queue* fila);
int fecharServidor(bool* servidorRodando, SOCKET serverSockFd, std::thread* serverThread);
int abrirServidor(SOCKET* sockfd, int numeroPorta);
void rodarServidor(bool* servidorRodando, int numeroPorta, SOCKET sockfdserver, queue* fila);
void processoRequisicao(SOCKET sockfdcli, queue* fila);
const char* arquivoFila(queue* fila, request_t* request);
const char* imprimir(queue* fila);
int portaServidor(int argc, char* argv[], int* numeroPorta);

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "ERRO: Falha ao inicializar o Winsock." << std::endl;
    return 1;
	}
		
    int returnCode = 0;
    int userInput, numeroPorta;
    SOCKET sockfd;
    queue* fila = iniciarFila();
    bool servidorRodando = false;
    std::thread serverThread;

    returnCode = portaServidor(argc, argv, &numeroPorta);
    if (returnCode != 0) goto leave;

    system("cls");
    while (true) {
        std::cout << "======== Menu ========" << std::endl << std::endl;

        std::cout << "Status do servidor: ";
        if (servidorRodando) std::cout << "Ligado";
        else std::cout << "Desligado";

        std::cout << std::endl << "Porta: " << numeroPorta << std::endl;

        std::cout << std::endl;

        std::cout << "[1] - Abrir servidor" << std::endl;
        std::cout << "[2] - Fechar servidor" << std::endl;
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
            if (!servidorRodando) {
                if (abrirServidor(&sockfd, numeroPorta) == 0) {
                    servidorRodando = true;
                    serverThread = std::thread(rodarServidor, &servidorRodando, numeroPorta, sockfd, fila);
                }
            }
            else {
                std::cout << "O servidor ja esta no ar!" << std::endl;
            }
            break;
        case 2:
            if (fecharServidor(&servidorRodando, sockfd, &serverThread) == 0) {
                std::cout << "Servidor fechado com sucesso." << std::endl;
            }
            else {
                std::cout << "Erro ao fechar o servidor." << std::endl;
            }
            break;
        case 3:
            std::cout << getQueueString(fila) << std::endl;
            break;
        case 4:
            std::cout << imprimir(fila) << std::endl;
            break;
        default:
            std::cout << "Digite uma entrada valida!" << std::endl;
            continue;
        }
        continuar();
        system("cls");
    }

leave:
    fecharServidor(&servidorRodando, sockfd, &serverThread);
    limparFila(fila);
    free(fila);

    return returnCode;
}

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

request_t* iniciarRequisicao() {
    request_t* request = (request_t*)malloc(sizeof(request_t));
    memset(request->nome, 0, sizeof(request->nome));
    memset(request->informacao, 0, sizeof(request->informacao));
    memset(request->data, 0, sizeof(request->data));
    strcpy_s(request->data, sizeof(request->data), pegarData().c_str());

    return request;
}

request_t* strToRequest(char* str) {
    request_t* request = iniciarRequisicao();

    char* next_token = NULL;
    char* slice;
    slice = strtok_s(str, ";", &next_token);
    request->op = (op_codes)atoi(slice);

    slice = strtok_s(NULL, ";", &next_token);
    strcpy_s(request->nome, sizeof(request->nome), slice);

    slice = strtok_s(NULL, ";", &next_token);
    strcpy_s(request->informacao, sizeof(request->informacao), slice);

    slice = strtok_s(NULL, ";", &next_token);
    strcpy_s(request->data, sizeof(request->data), slice);

    return request;
}

void continuar() {
    std::cout << std::endl << "Pressione qualquer tecla para continuar..";
    std::cin.ignore().get();
}

nodeValue* criarValorNodo(char* nome, char* informacao, char* data) {
    nodeValue* newNode = (nodeValue*)malloc(sizeof(nodeValue));

    newNode->nome = (char*)malloc(strlen(nome) + 1);
    strcpy_s(newNode->nome, strlen(nome) + 1, nome);

    newNode->informacao = (char*)malloc(strlen(informacao) + 1);
    strcpy_s(newNode->informacao, strlen(informacao) + 1, informacao);

    newNode->data = (char*)malloc(strlen(data) + 1);
    strcpy_s(newNode->data, strlen(data) + 1, data);

    return newNode;
}

void add(queue* fila, nodeValue* val) {
    if (fila != NULL) {
        node* newNode = (node*)malloc(sizeof(node));
        newNode->next = NULL;
        newNode->value = val;

        fila->size = fila->size + 1;

        if (fila->first == NULL) {
            fila->first = newNode;
            fila->last = newNode;
        }
        else {
            fila->last->next = newNode;
            fila->last = newNode;
        }
    }
}

queue* iniciarFila() {
    queue* fila = (queue*)malloc(sizeof(queue));
    fila->size = 0;
    fila->first = NULL;
    fila->last = NULL;

    return fila;
}

bool vazio(queue* fila) {
    if (fila->first == NULL) return true;
    return false;
}

void imprimirValorNodo(nodeValue* n) {
    if (n != NULL) {
        std::cout << "\tNome: " << n->nome << std::endl;
        std::cout << "\tInformacao: " << n->informacao << std::endl;
        std::cout << "\tData: " << n->data << std::endl;
    }
}

std::string getQueueString(queue* fila) {
    if (fila != NULL) {
        std::string queueString;
        node* searchNode = fila->first;
        int position = 1;

        if (vazio(fila)) {
            return "Fila Vazia!";
        }
        else {
            while (searchNode != NULL) {
                queueString += "Posicao " + std::to_string(position) + "\n\n";
                position++;
                queueString += "\tNome: " + std::string(searchNode->value->nome) + "\n";
                queueString += "\tInformacao: " + std::string(searchNode->value->informacao) + "\n";
                queueString += "\tData: " + std::string(searchNode->value->data) + "\n";

                if (searchNode->next != NULL) {
                    queueString += "\n";
                }

                searchNode = searchNode->next;
            }

            return queueString;
        }
    }
    else {
        return "Fila Vazia!";
    }
}

nodeValue* remove(queue* fila) {
    if (fila != NULL && !vazio(fila)) {
        node* nodeToBeRemoved = fila->first;
        nodeValue* valueRemoved = nodeToBeRemoved->value;
        fila->first = nodeToBeRemoved->next;

        if (fila->first == NULL)
            fila->last = NULL;

        free(nodeToBeRemoved);
        return valueRemoved;
    }

    return NULL;
}

void limparFila(queue* fila) {
    node* searchNode = fila->first;
    nodeValue* valueAux;
    while (searchNode != NULL) {
        searchNode = searchNode->next;
        valueAux = remove(fila);
        free(valueAux->nome);
        free(valueAux->informacao);
        free(valueAux->data);
        free(valueAux);
    }
}

int fecharServidor(bool* servidorRodando, SOCKET serverSockFd, std::thread* serverThread) {
    if (*servidorRodando) {
        shutdown(serverSockFd, SD_BOTH);
        *servidorRodando = false;
        serverThread->join();

        return 0;
    }

    return 1;
}

int abrirServidor(SOCKET* sockfd, int numeroPorta) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == INVALID_SOCKET) {
        std::cerr << "ERRO: nao foi possivel criar socket. Codigo de erro: " << WSAGetLastError() << std::endl;
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(numeroPorta);

    int bindResult = bind(*sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (bindResult == SOCKET_ERROR) {
        std::cerr << "ERRO: Erro ao fazer bind." << std::endl;
        closesocket(*sockfd);
        return 1;
    }

    listen(*sockfd, MAX_REQUESTS);
    std::cout << "Servidor aberto na porta: " << numeroPorta << std::endl;
    return 0;
}

void rodarServidor(bool* servidorRodando, int numeroPorta, SOCKET sockfdserver, queue* fila) {
    SOCKET sockfdcli;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    std::vector <std::thread> clientsProcessingThreads;
    std::vector <SOCKET> clientsSocksFds;

    while (*servidorRodando) {
        clilen = sizeof(cli_addr);
        sockfdcli = accept(sockfdserver, (struct sockaddr*)&cli_addr, &clilen);
        if (sockfdcli != INVALID_SOCKET) {
            clientsSocksFds.push_back(sockfdcli);
            std::thread clientThread(processoRequisicao, sockfdcli, fila);
            clientsProcessingThreads.push_back(std::move(clientThread));
        }
    }

    for (int i = 0; i < clientsSocksFds.size(); i++) {
        shutdown(clientsSocksFds.at(i), SD_BOTH);
        closesocket(clientsSocksFds.at(i));
    }

    for (int i = 0; i < clientsProcessingThreads.size(); i++) {
        clientsProcessingThreads.at(i).join();
    }

    closesocket(sockfdserver);
}

void processoRequisicao(SOCKET sockfdcli, queue* fila) {
    char buffer[REQUEST_BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, REQUEST_BUFFER_SIZE);

        int readResult = recv(sockfdcli, buffer, REQUEST_BUFFER_SIZE, 0);
        if (readResult < 0 || strlen(buffer) == 0) {
            int writeResult = send(sockfdcli, "Erro. Impossivel ler do cliente. Esse problema pode ocorrer quando o servidor foi fechado com uma conexao ativa.", 116, 0);
            return;
        }

        request_t* request = strToRequest(buffer);
        memset(buffer, 0, REQUEST_BUFFER_SIZE);
        switch (request->op) {
        case ADD:
            strcpy_s(buffer, sizeof(buffer), arquivoFila(fila, request));
            break;
        case LIST:
            strcpy_s(buffer, sizeof(buffer), getQueueString(fila).c_str());
            break;
        case PRINT:
            strcpy_s(buffer, sizeof(buffer), imprimir(fila));
            break;
        default:
            strcpy_s(buffer, sizeof(buffer), "Operacao nao reconhecida");
            break;
        }

        free(request);
        int writeResult = send(sockfdcli, buffer, strlen(buffer), 0);
        if (writeResult < 0) {
            //!!!
            return;
        }
    }
}

const char* arquivoFila(queue* fila, request_t* request) {
    if (fila->size == MAX_QUEUE_SIZE) {
        return "Pedido negado. Numero maximo de 5 arquivos atingido.";
    }
    else {
        add(fila, criarValorNodo(request->nome, request->informacao, request->data));
        return "Registro adicionado a fila com sucesso.";
    }
}

const char* imprimir(queue* fila) {
    if (vazio(fila))
        return "Fila vazia!";

    std::ofstream outfile;
    outfile.open(FILE_PATH, std::ios_base::app);

    for (node* searchNode = fila->first; searchNode != NULL; searchNode = searchNode->next) {
        outfile << searchNode->value->data << " - " << searchNode->value->nome << " / " << searchNode->value->informacao << std::endl;
    }

    limparFila(fila);
    outfile.close();
    return "Fila impressa com sucesso.";
}

int portaServidor(int argc, char* argv[], int* numeroPorta) {
    if (argc < 2) {
        std::cerr << "ERRO: Nao foi passado o numero da porta para o servidor." << std::endl;
        return 1;
    }

    *numeroPorta = atoi(argv[1]);
    return 0;
}
