#pragma once

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>

static class FuncoesAuxiliares {
public:
    static std::string GetHorario();
    static std::string Formatar(int valor, const int tamanho);
    static std::string Formatar(float valor, const int tamanho);
    static float ValorAleatorio(float max);
    static int ValorAleatorio(int max);
    static std::vector<std::string> SepararString(std::string mensagem, std::string delimitador);
};