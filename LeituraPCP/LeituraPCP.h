#pragma once

#include <string>

class LeituraPCP
{
private:
	bool status;
	int nseq;

public:
	LeituraPCP();
	void AlterarStatus();
	bool GetStatus();
	void LerMensagem(); //simula a leitura atrav�s da gera��o de dados	
	std::string GerarOperacao();
	std::string GerarHora();
	char charAleatorio();

};

