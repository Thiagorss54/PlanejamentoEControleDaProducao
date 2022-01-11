#include "LeituraSupervisorio.h"
#include "FuncoesAuxiliares.h"

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


LeituraSupervisorio::LeituraSupervisorio() {
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

    mensagem << TIPO << "#" << FuncoesAuxiliares::Formatar(nseq, 5) << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9),5) << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9), 5) << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9), 5) << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9), 5) << "#" << FuncoesAuxiliares::Formatar(FuncoesAuxiliares::ValorAleatorio(999.9), 5) << "#" << FuncoesAuxiliares::GetHorario();
    std::cout << mensagem.str() << std::endl;

    Sleep(500);

    nseq++;
}


int main() {
    LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio();
    for (int i = 0; i < 100; ++i) {
        leituraSupervisorio->LerMensagem();
    }
    return 0;
}