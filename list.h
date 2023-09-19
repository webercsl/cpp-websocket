#pragma once
#include <stdlib.h>
#include <string.h>
#include <iostream>

struct nodeValue{
    char *name, *content, *date;
};

struct node {
    struct nodeValue * value;
	struct node * next;
};

struct list {
    int size;
    node *first, *last;
};

nodeValue * createNodeValue(char *name, char *content, char *date){
    nodeValue * newNode = (nodeValue *) malloc(sizeof(nodeValue));

    newNode->name = (char *) malloc(strlen(name));
    strcpy(newNode->name, name);

    newNode->content = (char *) malloc(strlen(content));
    strcpy(newNode->content, content);

    newNode->date = (char *) malloc(strlen(date));
    strcpy(newNode->date, date);

    return newNode;
}

void add(list * l, nodeValue * val){
    if(l != NULL){
        node * newNode = (node *) malloc(sizeof(node));
        newNode->next = NULL;
        newNode->value = val;

        l->size = l->size + 1;

        if(l->first == NULL){
            l->first = newNode;
            l->last = newNode;
        }else{
            l->last->next = newNode;
            l->last = newNode;
        }
    }
}

list * initList(){
    list * l = (list *) malloc(sizeof(list));
    l->size = 0;
    l->first = NULL;
    l->last = NULL;

    return l;
}

bool empty(list * l){
    if(l->first == NULL) return true;
    return false;
}

void printNodeValue(nodeValue * n){
    if(n != NULL){
        std::cout << "\tNome: " << n->name << std::endl;
        std::cout << "\tArquivo: " << n->content << std::endl;
        std::cout << "\tData: " << n->date << std::endl;
    }
}

std::string getListString(list * l){
    if(l != NULL){
        std::string listString;
        node * searchNode = l->first;
        int position = 1;

        if(empty(l)){
            return "Lista Vazia!";
        }else{
            while (searchNode != NULL)
            {
                listString += "Posição " + std::to_string(position) + "\n\n";
                position++;
                listString += "\tNome: " + std::string(searchNode->value->name) + "\n";
                listString += "\tArquivo: " + std::string(searchNode->value->content) + "\n";
                listString += "\tData: " + std::string(searchNode->value->date) + "\n";

                if(searchNode->next != NULL){
                    listString += "\n";
                }

                searchNode = searchNode->next;
            }

            return listString;
        }
    }else{
        return "Lista Vazia!";
    }
}

nodeValue * remove(list * l){
    if(l != NULL && !empty(l)){
        node * nodeToBeRemoved = l->first;
        nodeValue * valueRemoved = nodeToBeRemoved->value;
        l->first = nodeToBeRemoved->next;

        if(l->first == NULL)
            l->last = NULL;

        free(nodeToBeRemoved);
        return valueRemoved;
    }

    return NULL;
}

void clearList(list * l){
    node * searchNode = l->first;
    nodeValue * valueAux;
    while (searchNode != NULL)
    {
        searchNode = searchNode->next;
        valueAux = remove(l);
        free(valueAux);
    }
}