#include "LeituraPCP.h"
#include "LeituraPCP.h"
#include "FuncoesAuxiliares.h"

#include <iostream>
#include <sstream>
#include <windows.h>

#define TIPO 0
#define NSEQ_MAX 99999

LeituraPCP::LeituraPCP(ListaEncadeada* listaEncadeada_) :listaEncadeada(listaEncadeada_)
{
	//srand(time(NULL));
	status = false;
	nseq = 1;
}

void LeituraPCP::AlterarStatus()
{
	status = !status;
}

bool LeituraPCP::GetStatus()
{
	return status;
}

void LeituraPCP::LerMensagem()
{
	std::stringstream mensagem;

	mensagem << TIPO << "#" << FuncoesAuxiliares::Formatar(nseq, 5) << "#" << GerarOperacao() << "#" << GerarHora() << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(99999), 5);

	listaEncadeada->Inserir(mensagem.str());

	if (nseq == NSEQ_MAX) {
		nseq = 1;
	}
	else {
		nseq++;
	}
	
}

std::string LeituraPCP::GerarOperacao()
{
	std::stringstream ss;
	ss << charAleatorio() << charAleatorio() << charAleatorio() << "-" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(9999), 4);

	return ss.str();
}

std::string LeituraPCP::GerarHora()
{
	std::stringstream ss;
	int hora = 0, minuto = 0, segundo = 0;
	hora = rand() % 24;
	minuto = rand() % 60;
	segundo = rand() % 60;
	ss << FuncoesAuxiliares::Formatar(hora, 2) << ":" << FuncoesAuxiliares::Formatar(minuto, 2) << ":" << FuncoesAuxiliares::Formatar(segundo, 2);
	return ss.str();
}

char LeituraPCP::charAleatorio()
{
	int aux = rand() % 26 + 65;
	char result = aux;

	return result;
}