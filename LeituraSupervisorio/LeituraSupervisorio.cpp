// LeituraSupervisorio.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "LeituraSupervisorio.h"


#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <time.h>
#include <sstream>
#include <iomanip>]
#include <windows.h>
#include <cstdlib>

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

    mensagem << TIPO << "#" << Formatar(nseq, 5) << "#" << Formatar(ValorAleatorio(999.9),5) << "#" << Formatar(ValorAleatorio(999.9), 5) << "#" << Formatar(ValorAleatorio(999.9), 5) << "#" << Formatar(ValorAleatorio(999.9), 5) << "#" << Formatar(ValorAleatorio(999.9), 5) << "#" << this->GetHorario();
    Sleep(500);


    std::cout << mensagem.str() << std::endl;


    nseq++;
}

std::string LeituraSupervisorio::GetHorario()
{
  
    char timestamp[14];
    auto horario =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int hora = 0, minuto = 0, segundo = 0, milisegundo = 0;
    long long aux=0;
    
    milisegundo = (int)(horario % 1000);
    
    aux = (horario - milisegundo)/1000;
    segundo = aux % 60;
    minuto = (aux / 60) % 60;
    hora = (aux / 3600) % 24;
    

    //Transformar no formato do timestamp 

    std::stringstream ss;
    ss<< Formatar(hora,2) <<":" << Formatar(minuto,2) <<":" <<Formatar(segundo,2)<<"." << Formatar(milisegundo,3);


    return ss.str();
}
std::string LeituraSupervisorio::Formatar(int valor, const int tamanho )
{
    std::stringstream ss, aux;
 
    aux << valor;

    for (int i = 0; i < tamanho - (int)aux.str().size();++i) {
        ss << "0";
    }
    ss << valor;

    return ss.str();
    
}

std::string LeituraSupervisorio::Formatar(float valor, const int tamanho)
{
    std::stringstream ss, aux;

    ss << std::fixed << std::setprecision(1);
    aux<< std::fixed << std::setprecision(1);
    aux << valor;

    for (int i = 0; i < tamanho - (int)aux.str().size(); ++i) {
        ss << "0";
    }
    ss << valor;

    return ss.str();
}

float LeituraSupervisorio::ValorAleatorio(float max)
{
    float resultado = 0.0f;
    resultado = rand() / (RAND_MAX/max);

    return resultado;
}


int main() {
    LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio();
    for (int i = 0; i < 100; ++i) {
        leituraSupervisorio->LerMensagem();
    }
    return 0;
}