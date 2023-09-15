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
    //free(name);

    newNode->content = (char *) malloc(strlen(content));
    strcpy(newNode->content, content);
    //free(content);

    newNode->date = (char *) malloc(strlen(date));
    strcpy(newNode->date, date);
    //free(date);    

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

void printList(list * l){
    if(l != NULL){
        node * searchNode = l->first;
        int position = 1;

        if(empty(l)){
            std::cout << "Lista Vazia!";
        }else{
            while (searchNode != NULL)
            {
                std::cout << "Posição " << position++ << std::endl << std::endl;
                printNodeValue(searchNode->value);
                searchNode = searchNode->next;
                std::cout << std::endl;
            }
        }
    }
}