#pragma once
#include <string>
#include "ListaEncadeada.h"


class RetirarMensagem
{
private:
	bool status;
	ListaEncadeada* listaEncadeada;

public:
	RetirarMensagem(ListaEncadeada* listaEncadeada_);
	void AlterarStatus();
	bool GetStatus();
	string RetiraMensagem();
};

