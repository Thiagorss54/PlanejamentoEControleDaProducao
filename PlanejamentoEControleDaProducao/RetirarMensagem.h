#pragma once

#include <string>
#include "ListaEncadeada.h"

using namespace std;
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