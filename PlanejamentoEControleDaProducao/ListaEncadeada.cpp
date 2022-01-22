#include "ListaEncadeada.h"
#include <iostream>

ListaEncadeada::ListaEncadeada(int tamanhoMax_) :tamanhoMax(tamanhoMax_) {
    inicio = nullptr;
    fim = nullptr;
    tamanho = 0;
}
void ListaEncadeada::Inserir(string valor) {
    if (Cheia()) {
        return;
    }

    No* novo = new No;
    novo->msg = valor;
    if (Vazia()) {

        inicio = novo;
        fim = novo;
        fim->proximo = inicio;

    }
    else {

        novo->proximo = inicio;
        fim->proximo = novo;
        fim = novo;
    }
    //std::cout << "Mensagem [" << tamanho << "] adicionada!" << std::endl;
    tamanho++;
    if (Cheia()) {
        std::cout << "LISTA CHEIA!" << std::endl;
    }
}

string ListaEncadeada::Remover() {
    if (Vazia()) {
        return "-1";
    }
    No* remover = new No;
    remover = inicio;
    if (tamanho == 1) {
        inicio = nullptr;
        fim = nullptr;
    }
    else {
        inicio = inicio->proximo;
    }
    string v = remover->msg;
    delete remover;
    tamanho--;
    return v;
}
bool ListaEncadeada::Cheia() {
    return (tamanho == tamanhoMax);
}
bool ListaEncadeada::Vazia() {
    return (tamanho == 0);
}

void ListaEncadeada::Print() {
    if (Vazia()) {
        return;
    }
    No* i = inicio;
    do {
        std::cout << i->msg << std::endl;
        i = i->proximo;
    } while (i != fim->proximo);

}
ListaEncadeada::~ListaEncadeada() {
    while (!Vazia()) {
        Remover();
    }
}