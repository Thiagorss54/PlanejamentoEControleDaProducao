// LeituraSupervisorio.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "LeituraSupervisorio.h"


#include <iostream>
#include <string>
#include <vector>



std::vector<std::string> SepararString(std::string mensagem, std::string delimitador) {
    std::vector<std::string> resultado;

    unsigned inicio = 0U;
    auto fim = mensagem.find(delimitador);

    while (fim != std::string::npos) {
        std::cout << mensagem.substr(inicio, fim - inicio) << std::endl;
        resultado.push_back(mensagem.substr(inicio, fim - inicio));
        inicio = fim + delimitador.length();
        fim = mensagem.find(delimitador, inicio);
    }

    return resultado;
}

LeituraSupervisorio::LeituraSupervisorio() {
    status = false;
}

void LeituraSupervisorio::AlterarStatus() {
    status = !status;
}

bool LeituraSupervisorio::GetStatus() {
    return status;
}