#include "FuncoesAuxiliares.h"
#include <vector>
#include <iostream>

std::vector<std::string> FuncoesAuxiliares::SepararString(std::string mensagem, std::string delimitador) {
    std::vector<std::string> resultado;

    unsigned inicio = 0U;
    auto fim = mensagem.find(delimitador);

    while (fim != std::string::npos) {
        resultado.push_back(mensagem.substr(inicio, fim - inicio));
        inicio = (int)(fim + delimitador.length());
        fim = mensagem.find(delimitador, inicio);
    }
    resultado.push_back(mensagem.substr(inicio, mensagem.length()));

    return resultado;
}

std::string FuncoesAuxiliares::GetHorario() {
    char timestamp[14];
    auto horario = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int hora = 0, minuto = 0, segundo = 0, milisegundo = 0;
    long long aux = 0;

    milisegundo = (int)(horario % 1000);

    aux = (horario - milisegundo) / 1000;
    segundo = aux % 60;
    minuto = (aux / 60) % 60;
    hora = (aux / 3600) % 24;

    //Transformar no formato do timestamp 
    std::stringstream ss;
    ss << Formatar(hora, 2) << ":" 
       << Formatar(minuto, 2) << ":" 
       << Formatar(segundo, 2) << "." 
       << Formatar(milisegundo, 3);

    return ss.str();
}

std::string FuncoesAuxiliares::Formatar(int valor, const int tamanho) {
    std::stringstream ss, aux;
    aux << valor;
    for (int i = 0; i < tamanho - (int)aux.str().size(); ++i) {
        ss << "0";
    }
    ss << valor;
    return ss.str();
}

std::string FuncoesAuxiliares::Formatar(float valor, const int tamanho) {
    std::stringstream ss, aux;
    ss << std::fixed << std::setprecision(1);
    aux << std::fixed << std::setprecision(1);
    aux << valor;
    for (int i = 0; i < tamanho - (int)aux.str().size(); ++i) {
        ss << "0";
    }
    ss << valor;
    return ss.str();
}

float FuncoesAuxiliares::ValorAleatorio(float max) {
    float resultado = 0.0f;
    resultado = rand() / (RAND_MAX / max);
    return resultado;
}

int FuncoesAuxiliares::ValorAleatorio(int max) {
    int resultado = 0;
    resultado = rand() % (max + 1);
    return resultado;
}