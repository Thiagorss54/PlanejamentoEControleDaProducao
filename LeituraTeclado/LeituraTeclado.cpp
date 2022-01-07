// LeituraTeclado.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <conio.h>
#include "../LeituraSupervisorio/LeituraSupervisorio.h"

using namespace std;




void ExecutarInstrucao(char instrucao, LeituraSupervisorio *leituraSupervisorio, bool& p, bool& r, bool& s, bool& e)
{
	switch (tolower(instrucao)) {
	case ('l'):
		leituraSupervisorio->AlterarStatus();
		
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

		break;
	default:
		break;
	}
}

void GerarDashboard(LeituraSupervisorio *leituraSupervisorio, bool p, bool r, bool s, bool e)
{
	cout << "Instrucao 'l' - Leitura do Supervisorio \tStatus: " << leituraSupervisorio->GetStatus() << " \n";
	cout << "Instrucao 'p' - Leitura do PCP \tStatus: " << p << " \n";
	cout << "Instrucao 'r' - CapturaMensagens \tStatus: " << r << " \n";
	cout << "Instrucao 's' - DadosProcesso \tStatus: " << s << " \n";
	cout << "Instrucao 'e' - GestaoProducao \tStatus: " << e << " \n";
	cout << "Instrucao '1' - Limpar console DadosProcesso\n";
	cout << "Instrucao '2' - Limpar console GestaoProducao\n";
	cout << "Instrucao 'esc' - Encerrar processos\n";
}

int main()
{
	system("cls");
	LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio();
	bool l = false, p = false, r = false, s = false, e = false; //Trocar variavel por metodo BuscarStatus() das classes
	char instrucao;
	while (true) {
		GerarDashboard(leituraSupervisorio, p, r, s, e);
		cout << "Insira uma instrucao: ";
		instrucao = _getch();
		std::cout << instrucao;
		ExecutarInstrucao(instrucao, leituraSupervisorio, p, r, s, e);
		Sleep(1000);
		system("cls");
	}
}
