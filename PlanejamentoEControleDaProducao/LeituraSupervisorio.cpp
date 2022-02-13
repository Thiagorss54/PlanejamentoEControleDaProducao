#include "LeituraSupervisorio.h"
#include "FuncoesAuxiliares.h"
#include "ListaEncadeada.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>

#define TIPO 5
#define NSEQ_MAX 99999

LeituraSupervisorio::LeituraSupervisorio(ListaEncadeada* listaEncadeada_) :listaEncadeada(listaEncadeada_) {
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

	mensagem << TIPO 
			 << "#" << FuncoesAuxiliares::Formatar(nseq, 5)
			 << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
			 << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
			 << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
			 << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
			 << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9f), 5)
			 << "#" << FuncoesAuxiliares::GetHorario();

	listaEncadeada->Inserir(mensagem.str());

	if (nseq == NSEQ_MAX) {
		nseq = 1;
	}
	else {
		nseq++;
	}
}