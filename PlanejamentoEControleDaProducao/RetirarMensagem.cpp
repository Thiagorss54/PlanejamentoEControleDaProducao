#include "RetirarMensagem.h"
#include "FuncoesAuxiliares.h"
#include "ListaEncadeada.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>

RetirarMensagem::RetirarMensagem(ListaEncadeada* listaEncadeada_): listaEncadeada(listaEncadeada_) {
	status = false;
}

void RetirarMensagem::AlterarStatus() {
	status = !status;
}

bool RetirarMensagem::GetStatus() {
	return status;
}

string RetirarMensagem::RetiraMensagem() {
	string msg = listaEncadeada->Remover();
	return msg;
}