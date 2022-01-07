// LeituraTeclado.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <conio.h>
#include "LeituraTeclado.h"

using namespace std;

int main()
{
	system("cls");
	bool l = false, p = false, r = false, s = false, e = false; //Trocar variavel por metodo BuscarStatus() das classes
	char instrucao;
	while (true) {
		GerarDashboard(l, p, r, s, e); 
		cout << "Insira uma instrucao: ";
		instrucao = _getch();
		ExecutarInstrucao(instrucao, l, p, r, s, e);
		Sleep(1000);
		system("cls");
	}
}

void ExecutarInstrucao(char instrucao, bool& l, bool& p, bool& r, bool& s, bool& e)
{
	switch (tolower(instrucao)) {
	case ('l'):
		//LeituraSupervisorio.AlterarStatus();
		l = !l;
		break;

	case ('p'):
		//LeituraPCP.AlterarStatus();
		p = !p;
		break;

	case ('r'):
		//CapturaMensagens.AlterarStatus();
		r = !r;
		break;

	case ('s'):
		//DadosProcesso.AlterarStatus();
		s = !s;
		break;

	case ('e'):
		//GestaoProducao.AlterarStatus();
		e = !e;
		break;

	case ('1'):
		//DadosProcesso.LimparConsole();
		cout << "1";
		break;

	case ('2'):
		//GestaoProducao.LimparConsole();
		cout << "2";
		break;

	case (27):
		//FecharTudo();
		cout << "esc";
		break;
	default:
		break;
	}
}

void GerarDashboard(bool l, bool p, bool r, bool s, bool e)
{
	cout << "Instrucao 'l' - Leitura do Supervisorio \tStatus: " << l << " \n";
	cout << "Instrucao 'p' - Leitura do PCP \tStatus: " << p << " \n";
	cout << "Instrucao 'r' - CapturaMensagens \tStatus: " << r << " \n";
	cout << "Instrucao 's' - DadosProcesso \tStatus: " << s << " \n";
	cout << "Instrucao 'e' - GestaoProducao \tStatus: " << e << " \n";
	cout << "Instrucao '1' - Limpar console DadosProcesso\n";
	cout << "Instrucao '2' - Limpar console GestaoProducao\n";
	cout << "Instrucao 'esc' - Encerrar processos\n";
}
