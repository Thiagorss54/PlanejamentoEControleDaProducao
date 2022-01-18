#pragma once

#include <string>
#include "ListaEncadeada.h"

class LeituraPCP
{
private:
	bool status;
	int nseq;
	ListaEncadeada* listaEncadeada;

public:
	LeituraPCP(ListaEncadeada* listaEncadeada_);
	void AlterarStatus();
	bool GetStatus();
	void LerMensagem(); //simula a leitura atrav�s da gera��o de dados	
	std::string GerarOperacao();
	std::string GerarHora();
	char charAleatorio();

};

