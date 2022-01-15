// LeituraTeclado.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <pthread.h>
#include <semaphore.h>

#include "../LeituraSupervisorio/LeituraSupervisorio.h"
#include "../LeituraPCP/LeituraPCP.h"

using namespace std;

#define _CHECKERROR 1
#include "CheckForError.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
													// _beginthreadex
typedef unsigned* CAST_LPDWORD;

#define WHITE   FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define HLGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define HLRED   FOREGROUND_RED   | FOREGROUND_INTENSITY

#define	ESC 0x1B			// Tecla para encerrar o programa


DWORD WINAPI ThreadLeituraSupervisorio(LeituraSupervisorio* leituraSupervisorio);
DWORD WINAPI ThreadLeituraPCP(LeituraPCP* leituraPCP);
DWORD WINAPI ThreadTeclado(LeituraSupervisorio* leituraSupervisorio,
						   LeituraPCP* leituraPCP);
DWORD WINAPI ThreadRetirarMensagem();	// Thread representando a retirada de mensagem

void ExecutarInstrucao(char instrucao,
	LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	bool& r, bool& s, bool& e);

void GerarDashboard(LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	bool r, bool s, bool e);


HANDLE hAguardaTeclado;	// Permite acesso exclusivo à alterações no teclado
HANDLE hListaLivre;		// Semáforo para indicar que a lista circular está livre
HANDLE hOut;						// Handle para a saída da console

char instrucao;
LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio();
LeituraPCP* leituraPCP = new LeituraPCP();


// THREAD PRIMÁRIA
int main()
{
	HANDLE hThreads[4];       // Leitura PCP, Leitura S
	DWORD dwIdSupervisorio, dwIdPCP, dwIdRetiraMsg;
	DWORD dwExitCode = 0;
	DWORD dwRet;

	hAguardaTeclado = CreateMutex(NULL, FALSE, (LPCWSTR)"AguardaTeclado");
	hListaLivre = CreateSemaphore(NULL, 0, 1, (LPCWSTR)"ListaLivre");

	// Obtém um handle para a saída da console
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		printf("Erro ao obter handle para a saída da console\n");

	hThreads[0] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLeituraSupervisorio(leituraSupervisorio),	//Casting necessário
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdSupervisorio);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread Leitura Supervisorio  %d criada com Id=%0x\n", 0, dwIdSupervisorio);
	else {
		printf("Erro na criacao da thread Leitura Supervisorio ! N = %d Erro = %d\n", 0, errno);
		exit(0);
	}

	hThreads[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLeituraPCP(leituraPCP),	//Casting necessário
		(LPVOID)(INT_PTR)1,
		0,
		(CAST_LPDWORD)&dwIdPCP);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread Leitura PCP  %d criada com Id=%0x\n", 1, dwIdPCP);
	else {
		printf("Erro na criacao da thread LeituraPCP! N = %d Erro = %d\n", 1, errno);
		exit(0);
	}

	hThreads[2] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadRetirarMensagem,	//Casting necessário
		(LPVOID)(INT_PTR)2,
		0,
		(CAST_LPDWORD)&dwIdRetiraMsg);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread Retirada mensagem  %d criada com Id=%0x\n", 2, dwIdPCP);
	else {
		printf("Erro na criacao da thread RetiraMensagem ! N = %d Erro = %d\n", 2, errno);
		exit(0);
	}

	hThreads[3] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadTeclado(leituraSupervisorio, leituraPCP),	//Casting necessário
		(LPVOID)(INT_PTR)2,
		0,
		(CAST_LPDWORD)&dwIdRetiraMsg);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread Retirada mensagem  %d criada com Id=%0x\n", 2, dwIdPCP);
	else {
		printf("Erro na criacao da thread RetiraMensagem ! N = %d Erro = %d\n", 2, errno);
		exit(0);
	}
	//Lista
	
	
	
	do {
		
	} while (instrucao != ESC);

	// Aguarda término das threads homens e mulheres
	dwRet = WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
	//CheckForError(dwRet == WAIT_OBJECT_0);

	// Fecha todos os handles de objetos do kernel
	for (int i = 0; i < 4; ++i)
		CloseHandle(hThreads[i]);

	// Fecha os handles dos objetos de sincronização
	// [INSIRA AQUI AS CHAMADAS DE FECHAMENTO DE HANDLES]

	return EXIT_SUCCESS;
}


void ExecutarInstrucao(char instrucao,
	LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	bool& r, bool& s, bool& e)
{
	switch (tolower(instrucao)) {
	case ('l'):
		leituraSupervisorio->AlterarStatus();
		break;

	case ('p'):
		leituraPCP->AlterarStatus();
		break;

	case ('r'):
		//capturaMensagens->AlterarStatus();
		break;

	case ('s'):
		//dadosProcesso->AlterarStatus();
		break;

	case ('e'):
		//gestaoProducao->AlterarStatus();
		break;

	case ('1'):
		//dadosProcesso->LimparConsole();
		break;

	case ('2'):
		//gestaoProducao->LimparConsole();
		break;
	default:
		break;
	}
}

void GerarDashboard(LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	bool r, bool s, bool e)
{
	cout << "Instrucao 'l' - Leitura do Supervisorio \tStatus: " << leituraSupervisorio->GetStatus() << " \n";
	cout << "Instrucao 'p' - Leitura do PCP \tStatus: " << leituraPCP->GetStatus() << " \n";
	cout << "Instrucao 'r' - CapturaMensagens \tStatus: " << r << " \n"; //capturaMensagens->GetStatus()
	cout << "Instrucao 's' - DadosProcesso \tStatus: " << s << " \n"; //dadosProcesso->GetStatus()
	cout << "Instrucao 'e' - GestaoProducao \tStatus: " << e << " \n"; //gestaoProducao->GetStatus()
	cout << "Instrucao '1' - Limpar console DadosProcesso\n"; //dadosProcesso->GetStatus()
	cout << "Instrucao '2' - Limpar console GestaoProducao\n"; //gestaoProducao->GetStatus()
	cout << "Instrucao 'esc' - Encerrar processos\n";
}
DWORD WINAPI ThreadTeclado(LeituraSupervisorio* leituraSupervisorio, LeituraPCP* leituraPCP) {
	DWORD dwStatus;
	BOOL bStatus;
	bool r = false, s = false, e = false;
	do {
		GerarDashboard(leituraSupervisorio, leituraPCP, r, s, e);
		cout << "Insira uma instrucao: ";
		instrucao = _getch();
		cout << instrucao;
		ExecutarInstrucao(instrucao, leituraSupervisorio, leituraPCP, r, s, e);
		Sleep(1000);
		system("cls");
	} while (instrucao != ESC);
	return 0;
}

DWORD WINAPI ThreadLeituraSupervisorio(LeituraSupervisorio* leituraSupervisorio) {
	DWORD dwStatus;
	BOOL bStatus;

	do {
		if (leituraSupervisorio->GetStatus()) {
			leituraSupervisorio->LerMensagem();
		}

	} while (instrucao != ESC);
	return 0;
}

DWORD WINAPI ThreadLeituraPCP(LeituraPCP* leituraPCP) {
	DWORD dwStatus;
	BOOL bStatus;

	do {

	} while (instrucao != ESC);
	return 0;
}

DWORD WINAPI ThreadRetirarMensagem() {
	DWORD dwStatus;
	BOOL bStatus;

	do {
		

	} while (instrucao != ESC);
	return 0;
}