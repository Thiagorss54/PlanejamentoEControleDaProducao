#pragma once
#include <iostream>
#include <string>
using std::string;

struct No {
	No* proximo;
	string msg;
};


class ListaEncadeada {
private:
	int tamanho;
	int tamanhoMax;
	No* inicio;
	No* fim;

public:
	ListaEncadeada(int tamanhoMax_);
	void Inserir(string msg);
	string Remover();
	bool Cheia();
	bool Vazia();
	void Print();

	~ListaEncadeada();

};