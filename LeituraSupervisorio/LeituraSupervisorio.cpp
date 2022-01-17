#include "LeituraSupervisorio.h"
#include "../PlanejamentoEControleDaProducao/FuncoesAuxiliares.h"
#include "../PlanejamentoEControleDaProducao/ListaEncadeada.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>



#define TIPO 5


std::vector<std::string> SepararString(std::string mensagem, std::string delimitador) {
	std::vector<std::string> resultado;

	unsigned inicio = 0U;
	auto fim = mensagem.find(delimitador);

	while (fim != std::string::npos) {
		std::cout << mensagem.substr(inicio, fim - inicio) << std::endl;
		resultado.push_back(mensagem.substr(inicio, fim - inicio));
		inicio = (int)(fim + delimitador.length());
		fim = mensagem.find(delimitador, inicio);
	}

	return resultado;
}


LeituraSupervisorio::LeituraSupervisorio(ListaEncadeada* listaEncadeada_) :listaEncadeada(listaEncadeada_) {
	//srand(time(NULL));
	status = false;
	nseq = 1;
}

void LeituraSupervisorio::AlterarStatus() {
	status = !status;
}

bool LeituraSupervisorio::GetStatus() {
	return status;
}

void LeituraSupervisorio::LerMensagem() {

	std::stringstream mensagem;

	mensagem << TIPO << "#" << FuncoesAuxiliares::Formatar(nseq, 5)
		<< "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
		<< "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
		<< "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
		<< "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
		<< "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
		<< "#" << FuncoesAuxiliares::GetHorario();


	listaEncadeada->Inserir(mensagem.str());
	//std::cout << mensagem.str() << std::endl;

	Sleep(1000);

	nseq++;
}


//int main() {
//    LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio();
//    for (int i = 0; i < 100; ++i) {
//        leituraSupervisorio->LerMensagem();
//    }
//    return 0;
//}