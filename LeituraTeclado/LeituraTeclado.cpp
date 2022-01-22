// LeituraTeclado.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>

#include "../PlanejamentoEControleDaProducao/LeituraSupervisorio.h"
#include "../PlanejamentoEControleDaProducao/LeituraPCP.h"
#include "../PlanejamentoEControleDaProducao/ListaEncadeada.h"
#include "../PlanejamentoEControleDaProducao/RetirarMensagem.h"



#define _CHECKERROR 1
#include "CheckForError.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
													// _beginthreadex
typedef unsigned* CAST_LPDWORD;

#define WHITE   FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define HLGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define HLRED   FOREGROUND_RED   | FOREGROUND_INTENSITY

#define	ESC 0x1B			// Tecla para encerrar o programa


DWORD WINAPI ThreadLeituraSupervisorio();
DWORD WINAPI ThreadLeituraPCP();
DWORD WINAPI ThreadTeclado();
DWORD WINAPI ThreadRetirarMensagem();

void ExecutarInstrucao(char instrucao,
	LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	RetirarMensagem* retirarMensagem, bool& s, bool& e);

void GerarDashboard(LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	RetirarMensagem* retirarMensagem, bool s, bool e);


HANDLE hListaLivre,hPosicoesLivres,hPosicoesOcupadas;		// Semáforo para indicar que a lista circular está livre
HANDLE hOut;						// Handle para a saída da console
HANDLE hEventLSup, hEventLPCP, hEventEsc, hEventRetirar,hEventGestao, hEventProcesso;

char instrucao; 
ListaEncadeada* lista1 = new ListaEncadeada(100);
LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio(lista1);
LeituraPCP* leituraPCP = new LeituraPCP(lista1);
RetirarMensagem* retirarMensagem = new RetirarMensagem(lista1);
bool s = false, e = false;
using namespace std;
// THREAD PRIMÁRIA
int main()
{
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(L"..\\x64\\Debug\\GestaoProducao.exe",
		NULL,        
		NULL,      
		NULL,         
		TRUE,         
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,   
		NULL,         
		L"..\\x64\\Debug",   
		&si,     
		&pi)       
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
	}

	STARTUPINFO si1;
	PROCESS_INFORMATION pi1;

	ZeroMemory(&si1, sizeof(si1));
	si1.cb = sizeof(si1);
	ZeroMemory(&pi1, sizeof(pi1));

	if (!CreateProcess(L"..\\x64\\Debug\\DadosProcesso.exe",
		NULL,
		NULL,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
		NULL,
		L"..\\x64\\Debug",
		&si1,
		&pi1)
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
	}
	

	HANDLE hThreads[4];       // Leitura PCP, Leitura S
	DWORD dwIdTelcado, dwIdSupervisorio, dwIdPCP, dwIdRetiraMsg;
	DWORD dwExitCode = 0;
	DWORD dwRet;

	hListaLivre = CreateSemaphore(NULL, 1, 1, L"ListaLivre");
	hPosicoesLivres = CreateSemaphore(NULL, 100, 100, L"ListaCheia");


	//Criando eventos

	hEventEsc = CreateEvent(NULL, TRUE, FALSE, L"EventoEsc");
	hEventLPCP = CreateEvent(NULL, TRUE, FALSE, L"EventoPCP");
	hEventLSup = CreateEvent(NULL, TRUE, FALSE, L"EventoSup");
	hEventRetirar = CreateEvent(NULL, TRUE, FALSE, L"EventoRetirar");
	hEventGestao = CreateEvent(NULL, TRUE, FALSE, L"EventoGestao");
	hEventProcesso = CreateEvent(NULL, TRUE, FALSE, L"EventoProcesso");

	// Obtém um handle para a saída da console
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		printf("Erro ao obter handle para a saída da console\n");

	//CRIACAO DAS THREADS
	hThreads[0] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadTeclado,	//Casting necessário
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdTelcado);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread Teclado  %d criada com Id=%0x\n", 0, dwIdTelcado);
	else {
		printf("Erro na criacao da thread Teclado ! N = %d Erro = %d\n", 2, errno);
		exit(0);
	}

	hThreads[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLeituraSupervisorio,	//Casting necessário
		(LPVOID)(INT_PTR)1,
		0,
		(CAST_LPDWORD)&dwIdSupervisorio);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[1] != (HANDLE)-1L)
		printf("Thread Leitura Supervisorio  %d criada com Id=%0x\n", 1, dwIdSupervisorio);
	else {
		printf("Erro na criacao da thread Leitura Supervisorio ! N = %d Erro = %d\n", 1, errno);
		exit(0);
	}

	hThreads[2] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLeituraPCP,	//Casting necessário
		(LPVOID)(INT_PTR)2,
		0,
		(CAST_LPDWORD)&dwIdPCP);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[2] != (HANDLE)-1L)
		printf("Thread Leitura PCP  %d criada com Id=%0x\n", 2, dwIdPCP);
	else {
		printf("Erro na criacao da thread LeituraPCP! N = %d Erro = %d\n", 2, errno);
		exit(0);
	}

	hThreads[3] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadRetirarMensagem,	//Casting necessário
		(LPVOID)(INT_PTR)3,
		0,
		(CAST_LPDWORD)&dwIdRetiraMsg);		//Casting necessário
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[3] != (HANDLE)-1L)
		printf("Thread Retirada mensagem  %d criada com Id=%0x\n", 3, dwIdRetiraMsg);
	else {
		printf("Erro na criacao da thread RetiraMensagem ! N = %d Erro = %d\n", 3, errno);
		exit(0);
	}


	// Aguarda término das threads 
	dwRet = WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);
	//CheckForError(dwRet == WAIT_OBJECT_0);


	// Fecha todos os handles de objetos do kernel
	for (int i = 0; i < 4; ++i)
		CloseHandle(hThreads[i]);

	// Fecha os handles dos objetos de sincronização
	CloseHandle(hListaLivre);
	CloseHandle(hPosicoesLivres);
	CloseHandle(hOut);
	//CloseHandle Event
	CloseHandle(hEventEsc);
	CloseHandle(hEventLPCP);
	CloseHandle(hEventLSup);
	CloseHandle(hEventRetirar);
	CloseHandle(hEventGestao);

	return EXIT_SUCCESS;
}


void ExecutarInstrucao(char instrucao,
	LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	RetirarMensagem* retirarMensagem,
	bool& s, bool& e)
{
	switch (tolower(instrucao)) {
	case ('l'):
		if (leituraSupervisorio->GetStatus()) {
			ResetEvent(hEventLSup);
		}
		else {
			SetEvent(hEventLSup);
		}
		leituraSupervisorio->AlterarStatus();
		break;

	case ('p'):
		if (leituraPCP->GetStatus()) {
			ResetEvent(hEventLPCP);
		}
		else {
			SetEvent(hEventLPCP);
		}
		leituraPCP->AlterarStatus();
		break;

	case ('r'):
		if (retirarMensagem->GetStatus()) {
			ResetEvent(hEventRetirar);
		}
		else {
			SetEvent(hEventRetirar);
		}
		retirarMensagem->AlterarStatus();
		break;

	case ('s'):
		PulseEvent(hEventProcesso);
		//dadosProcesso->AlterarStatus();
		break;

	case ('e'):
		PulseEvent(hEventGestao);
		//gestaoProducao->AlterarStatus();
		break;

	case ('1'):
		//dadosProcesso->LimparConsole();
		break;

	case ('2'):
		//gestaoProducao->LimparConsole();
		break;
	case (ESC):
		SetEvent(hEventEsc);
	default:
		break;
	}
}

void GerarDashboard(LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	RetirarMensagem* retirarMensagem,
	bool s, bool e)
{
	cout << "Instrucao 'l' - Leitura do Supervisorio \tStatus: " << leituraSupervisorio->GetStatus() << " \n";
	cout << "Instrucao 'p' - Leitura do PCP \tStatus: " << leituraPCP->GetStatus() << " \n";
	cout << "Instrucao 'r' - CapturaMensagens \tStatus: " << retirarMensagem->GetStatus() << " \n";
	cout << "Instrucao 's' - DadosProcesso \tStatus: " << s << " \n"; //dadosProcesso->GetStatus()
	cout << "Instrucao 'e' - GestaoProducao \tStatus: " << e << " \n"; //gestaoProducao->GetStatus()
	cout << "Instrucao '1' - Limpar console DadosProcesso\n"; //dadosProcesso->GetStatus()
	cout << "Instrucao '2' - Limpar console GestaoProducao\n"; //gestaoProducao->GetStatus()
	cout << "Instrucao 'esc' - Encerrar processos\n";
}
DWORD WINAPI ThreadTeclado() {
	DWORD dwStatus;
	BOOL bStatus;

	system("cls");
	bool s = false, e = false;
	do {
		
		instrucao = _getch();
		ExecutarInstrucao(instrucao, leituraSupervisorio, leituraPCP, retirarMensagem, s, e);
		
	} while (instrucao != ESC);
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadLeituraSupervisorio() {
	DWORD dwStatus;
	BOOL bStatus;


	HANDLE Events[2] = { hEventEsc, hEventLSup };
	DWORD ret;
	int tipoEvento;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;

		if (tipoEvento == 1) {
			WaitForSingleObject(hListaLivre, INFINITE);
			SetConsoleTextAttribute(hOut, HLGREEN);
			leituraSupervisorio->LerMensagem();
			ReleaseSemaphore(hListaLivre, 1, NULL);
		}

	} while (tipoEvento == 1 );

	cout << "Thread Leitura Supervisorio terminando..." << endl;
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadLeituraPCP() {
	DWORD dwStatus;
	BOOL bStatus;


	HANDLE Events[2] = { hEventEsc, hEventLPCP };
	DWORD ret;
	int tipoEvento;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;
		if (tipoEvento == 1) {
			WaitForSingleObject(hListaLivre, INFINITE);
			SetConsoleTextAttribute(hOut, HLRED);
			leituraPCP->LerMensagem();
			ReleaseSemaphore(hListaLivre, 1, NULL);
		}

	} while (tipoEvento == 1);

	cout << "Thread Leitura PCP terminando..." << endl;
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadRetirarMensagem() {
	DWORD dwStatus;
	BOOL bStatus;

	HANDLE Events[2] = { hEventEsc, hEventRetirar };
	DWORD ret;
	int tipoEvento;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;
		if (tipoEvento ==1) {
			WaitForSingleObject(hListaLivre, INFINITE);
			SetConsoleTextAttribute(hOut, WHITE);
			std::cout<< retirarMensagem->RetiraMensagem() << std::endl;
			ReleaseSemaphore(hListaLivre, 1, NULL);
			Sleep(2000);
		}

	} while (tipoEvento ==1);
	
	cout << "Thread RetirarMensagem terminando..." << endl;
	_endthreadex(0);
	return 0;
}
