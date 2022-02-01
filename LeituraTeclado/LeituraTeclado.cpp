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
#include "../PlanejamentoEControleDaProducao/FuncoesAuxiliares.h"

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
	RetirarMensagem* retirarMensagem);

void GerarDashboard(LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	RetirarMensagem* retirarMensagem);

HANDLE hListaLivre, hPosicoesLivres;		// Semáforo para indicar que a lista circular está livre
HANDLE hOut;												// Handle para a saída da console
HANDLE hEventLSup, hEventLPCP, hEventEsc, hEventRetirar, hEventGestao, hEventProcesso;
HANDLE hTemporizadorSup,hTemporizadorPCP;

char instrucao;
ListaEncadeada* lista1 = new ListaEncadeada(100);
LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio(lista1);
LeituraPCP* leituraPCP = new LeituraPCP(lista1);
RetirarMensagem* retirarMensagem = new RetirarMensagem(lista1);
using namespace std;

// THREAD PRIMÁRIA
int main()
{
	SetConsoleTitle(L"Planejamento e Controle da Producao");

	// Criando Processo GestaoProducao
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
		printf("CreateProcess GestaoProducao failed (%d).\n", GetLastError());
	}

	// Criando Processo GestaoProducao
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
		printf("CreateProcess DadosProcesso failed (%d).\n", GetLastError());
	}

	// Criando Semaforos
	hListaLivre = CreateSemaphore(NULL, 1, 1, L"ListaLivre");
	hPosicoesLivres = CreateSemaphore(NULL, 100, 100, L"ListaCheia");

	//Criando eventos
	hEventEsc = CreateEvent(NULL, TRUE, FALSE, L"EventoEsc");
	hEventLPCP = CreateEvent(NULL, TRUE, FALSE, L"EventoPCP");
	hEventLSup = CreateEvent(NULL, TRUE, FALSE, L"EventoSup");
	hEventRetirar = CreateEvent(NULL, TRUE, FALSE, L"EventoRetirar");
	hEventGestao = CreateEvent(NULL, TRUE, FALSE, L"EventoGestao");
	hEventProcesso = CreateEvent(NULL, TRUE, FALSE, L"EventoProcesso");
	

	//Temporizadores
	hTemporizadorSup = CreateWaitableTimer(NULL, FALSE, L"TemporizadorSup");
	hTemporizadorPCP = CreateSemaphore(NULL, 0, 1, L"TemporizadorPCP");

	// Obtém um handle para a saída da console
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		printf("Erro ao obter handle para a saída da console\n");

	//CRIACAO DAS THREADS
	HANDLE hThreads[4];       // Leitura PCP, Leitura S
	DWORD dwIdTelcado, dwIdSupervisorio, dwIdPCP, dwIdRetiraMsg;
	DWORD dwExitCode = 0;
	DWORD dwRet;

	// Thread Teclado
	hThreads[0] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadTeclado,
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdTelcado);
	CheckForError(hThreads[0]);
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread Teclado  %d criada com Id=%0x\n", 0, dwIdTelcado);
	else {
		printf("Erro na criacao da thread Teclado ! N = %d Erro = %d\n", 0, errno);
		exit(0);
	}

	// Thread LeituraSupervisorio
	hThreads[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLeituraSupervisorio,
		(LPVOID)(INT_PTR)1,
		0,
		(CAST_LPDWORD)&dwIdSupervisorio);
	CheckForError(hThreads[1]);
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[1] != (HANDLE)-1L)
		printf("Thread Leitura Supervisorio  %d criada com Id=%0x\n", 1, dwIdSupervisorio);
	else {
		printf("Erro na criacao da thread Leitura Supervisorio ! N = %d Erro = %d\n", 1, errno);
		exit(0);
	}

	// Thread LeituraPCP
	hThreads[2] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLeituraPCP,
		(LPVOID)(INT_PTR)2,
		0,
		(CAST_LPDWORD)&dwIdPCP);
	CheckForError(hThreads[2]);
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[2] != (HANDLE)-1L)
		printf("Thread Leitura PCP  %d criada com Id=%0x\n", 2, dwIdPCP);
	else {
		printf("Erro na criacao da thread LeituraPCP! N = %d Erro = %d\n", 2, errno);
		exit(0);
	}

	// ThreadRetirarMensagem
	hThreads[3] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadRetirarMensagem,
		(LPVOID)(INT_PTR)3,
		0,
		(CAST_LPDWORD)&dwIdRetiraMsg);
	CheckForError(hThreads[3]);
	SetConsoleTextAttribute(hOut, WHITE);
	if (hThreads[3] != (HANDLE)-1L)
		printf("Thread Retirada mensagem  %d criada com Id=%0x\n", 3, dwIdRetiraMsg);
	else {
		printf("Erro na criacao da thread RetiraMensagem ! N = %d Erro = %d\n", 3, errno);
		exit(0);
	}


	// Aguarda término das threads 
	dwRet = WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);
	CheckForError(dwRet == WAIT_OBJECT_0);

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
	CloseHandle(hTemporizadorPCP);
	CloseHandle(hTemporizadorSup);


	return EXIT_SUCCESS;
}


void ExecutarInstrucao(char instrucao,
	LeituraSupervisorio* leituraSupervisorio,
	LeituraPCP* leituraPCP,
	RetirarMensagem* retirarMensagem)
{
	switch (tolower(instrucao)) {
	case ('l'):
		if (leituraSupervisorio->GetStatus()) {
			ResetEvent(hEventLSup);
			SetConsoleTextAttribute(hOut, HLRED);
			cout << "Leitura Supervisorio desativada \n";
		}
		else {
			SetEvent(hEventLSup);
			SetConsoleTextAttribute(hOut, HLGREEN);
			cout << "Leitura Supervisorio ativada \n";
		}
		leituraSupervisorio->AlterarStatus();
		break;

	case ('p'):
		if (leituraPCP->GetStatus()) {
			ResetEvent(hEventLPCP);
			SetConsoleTextAttribute(hOut, HLRED);
			cout << "Leitura PCP desativada \n";
		}
		else {
			SetEvent(hEventLPCP);
			SetConsoleTextAttribute(hOut, HLGREEN);
			cout << "Leitura PCP ativada \n";
		}
		leituraPCP->AlterarStatus();
		break;

	case ('r'):
		if (retirarMensagem->GetStatus()) {
			ResetEvent(hEventRetirar);
			SetConsoleTextAttribute(hOut, HLRED);
			cout << "Retirar mensagem desativada \n";
		}
		else {
			SetEvent(hEventRetirar);
			SetConsoleTextAttribute(hOut, HLGREEN);
			cout << "Retirar mensagem ativada \n";
		}
		retirarMensagem->AlterarStatus();
		break;

	case ('s'):
		PulseEvent(hEventProcesso);
		break;

	case ('e'):
		PulseEvent(hEventGestao);
		break;

	case (ESC):
		SetEvent(hEventEsc);
		break;

	default:
		break;
	}
}

void GerarDashboard()
{
	SetConsoleTextAttribute(hOut, WHITE);
	cout << "Instrucao 'l' - Leitura do Supervisorio \n";
	cout << "Instrucao 'p' - Leitura do PCP \n";
	cout << "Instrucao 'r' - CapturaMensagens \n";
	cout << "Instrucao 's' - DadosProcesso \n";
	cout << "Instrucao 'e' - GestaoProducao \n";
	cout << "Instrucao 'esc' - Encerrar processos \n";
}
DWORD WINAPI ThreadTeclado() {
	system("cls");
	GerarDashboard();

	do {
		instrucao = _getch();
		ExecutarInstrucao(instrucao, leituraSupervisorio, leituraPCP, retirarMensagem);
	} while (instrucao != ESC);

	cout << "Thread Teclado terminando... \n";
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadLeituraSupervisorio() {
	
	HANDLE Events[2] = { hEventEsc, hEventLSup };
	DWORD ret;
	int tipoEvento;
	LARGE_INTEGER time;
	time.QuadPart = -0LL;
	SetWaitableTimer(hTemporizadorSup, &time, 500, NULL, NULL, FALSE);
	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;

		if (tipoEvento == 1) {
			WaitForSingleObject(hTemporizadorSup, INFINITE);
			WaitForSingleObject(hListaLivre, INFINITE);
			if (!lista1->Cheia()) {
				
				leituraSupervisorio->LerMensagem();
			}
			ReleaseSemaphore(hListaLivre, 1, NULL);

			
			
		}
	} while (tipoEvento == 1);

	cout << "Thread Leitura Supervisorio terminando... \n";
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadLeituraPCP() {
	HANDLE Events[2] = { hEventEsc, hEventLPCP };
	DWORD ret;
	int tipoEvento,tempoDeLeitura;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;

		if (tipoEvento == 1) {
			tempoDeLeitura = (rand() % 4001) + 1000;
 			WaitForSingleObject(hTemporizadorPCP, tempoDeLeitura);
			WaitForSingleObject(hListaLivre, INFINITE);
			if (!lista1->Cheia()) {
				leituraPCP->LerMensagem();
			}
			ReleaseSemaphore(hListaLivre, 1, NULL);
			
		}
	} while (tipoEvento == 1);

	cout << "Thread Leitura PCP terminando... \n";
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadRetirarMensagem() {
	HANDLE Events[2] = { hEventEsc, hEventRetirar };
	DWORD ret;
	int tipoEvento;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;
		if (tipoEvento == 1) {
			WaitForSingleObject(hListaLivre, INFINITE);
			if (!lista1->Vazia()) {
				SetConsoleTextAttribute(hOut, WHITE);
				retirarMensagem->RetiraMensagem();
			}
			ReleaseSemaphore(hListaLivre, 1, NULL);
		}
	} while (tipoEvento == 1);

	cout << "Thread RetirarMensagem terminando... \n";
	_endthreadex(0);
	return 0;
}