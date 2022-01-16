#include "LeituraPCP.h"
#include "../PlanejamentoEControleDaProducao/FuncoesAuxiliares.h"

#include <iostream>
#include <sstream>
#include <windows.h>

#define TIPO 0

LeituraPCP::LeituraPCP()
{
	//srand(time(NULL));
	status = false;
	nseq = 1;
}

void LeituraPCP::AlterarStatus()
{
	status =! status;
}

bool LeituraPCP::GetStatus()
{
	return status;
}

void LeituraPCP::LerMensagem()
{
	std::stringstream mensagem;

	mensagem << TIPO << "#" << FuncoesAuxiliares::Formatar(nseq, 5) << "#" << GerarOperacao() << "#" << GerarHora() << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(99999), 5);
	std::cout << mensagem.str() << std::endl;

	//Sleep(FuncoesAuxiliares::ValorAleatorio(5000));
	Sleep(3000);
	nseq++;
}

std::string LeituraPCP::GerarOperacao()
{
	std::stringstream ss;
	ss << charAleatorio() << charAleatorio() << charAleatorio() << "-" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(9999),4);

	return ss.str();
}

std::string LeituraPCP::GerarHora()
{
	std::stringstream ss;
	int hora = 0, minuto = 0, segundo = 0;
	hora = rand() % 24;
	minuto = rand() % 60;
	segundo = rand() % 60;
	ss << FuncoesAuxiliares::Formatar(hora,2) << ":" << FuncoesAuxiliares::Formatar(minuto, 2) <<":" << FuncoesAuxiliares::Formatar(segundo, 2);
	return ss.str();
}

char LeituraPCP::charAleatorio()
{
	int aux = rand() % 26 + 65;
	char result = aux;

	return result;
}

//int main() {
//	LeituraPCP* leituraPCP = new LeituraPCP();
//	for (int i = 0; i < 100; ++i) {
//		leituraPCP->LerMensagem();
//	}
//	return 0;
//}
