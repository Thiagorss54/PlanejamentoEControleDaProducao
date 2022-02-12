#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <cstring>

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

HANDLE hLista1Mutex, hLista2Mutex, hLista1Vazia, hLista2Vazia, hLista1Cheia, hLista2Cheia;		// Semáforo para indicar que a lista circular está livre
HANDLE hOut;												// Handle para a saída da console
HANDLE hEventLSup, hEventLPCP, hEventEsc, hEventRetirar, hEventGestao, hEventProcesso;
HANDLE hTemporizadorSup, hTemporizadorPCP;
HANDLE hPipeNotificacaoProducao, hPipeNotificacaoProcesso, hPipeGestaoProducao; // handle para pipes
HANDLE hFileLista2; //Handle para arquivo

//HANDLE hMemoriaCompartilhada,hEventEnviado,hEventLido; // handle para memoria compartilhada

char instrucao;
ListaEncadeada* lista1 = new ListaEncadeada(100);
//ListaEncadeada* lista2 = new ListaEncadeada(200);
LeituraSupervisorio* leituraSupervisorio = new LeituraSupervisorio(lista1);
LeituraPCP* leituraPCP = new LeituraPCP(lista1);
RetirarMensagem* retirarMensagem = new RetirarMensagem(lista1);
using namespace std;

//Variaveis para pipes de notificacao
bool sendMsg = TRUE;
bool sendMsgFalse = FALSE;

//Buffer para envio de mensagem por pipe
char buffer[32];
char finalizar[32] = "FINALIZAR";

// THREAD PRIMÁRIA
int main()
{
	SetConsoleTitle(L"Planejamento e Controle da Producao");

	//Pipe
	hPipeNotificacaoProcesso = CreateNamedPipe(
		L"\\\\.\\pipe\\NOTIFICACAOPROCESSO",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		sizeof(bool),
		sizeof(bool),
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);


	hPipeNotificacaoProducao = CreateNamedPipe(
		L"\\\\.\\pipe\\NOTIFICACAOPRODUCAO",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		sizeof(bool),
		sizeof(bool),
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	DWORD dwTamanhoBuffer = sizeof(buffer)*10;
	hPipeGestaoProducao = CreateNamedPipe(
		L"\\\\.\\pipe\\GESTAO",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
		PIPE_UNLIMITED_INSTANCES,
		dwTamanhoBuffer,
		dwTamanhoBuffer,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	//Criando Arquivo para lista 2
	
	hFileLista2 = CreateFile(L"..\\FileDadosProcesso.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFileLista2 == INVALID_HANDLE_VALUE) {
		cout << "ERROR -> " << GetLastError() << endl;
	}
	cout << "Success" << endl;

	// Criando Semaforos
	hLista1Mutex = CreateSemaphore(NULL, 1, 1, L"Lista1Livre");
	hLista2Mutex = CreateSemaphore(NULL, 1, 1, L"Lista2Livre");
	hLista1Vazia = CreateSemaphore(NULL, 100, 100, L"Lista1Vazia");
	hLista2Vazia = CreateSemaphore(NULL, 200, 200, L"Lista2Vazia");
	hLista1Cheia = CreateSemaphore(NULL, 0, 100, L"Lista1Cheia");
	hLista2Cheia = CreateSemaphore(NULL, 0, 200, L"Lista2Cheia");
	

	//Memoria Compartilhada
	
	//hMemoriaCompartilhada = CreateFileMapping

	//Criando eventos
	hEventEsc = CreateEvent(NULL, TRUE, FALSE, L"EventoEsc");
	CheckForError(hEventEsc);
	hEventLPCP = CreateEvent(NULL, TRUE, FALSE, L"EventoPCP");
	CheckForError(hEventLPCP);
	hEventLSup = CreateEvent(NULL, TRUE, FALSE, L"EventoSup");
	CheckForError(hEventLSup);
	hEventRetirar = CreateEvent(NULL, TRUE, FALSE, L"EventoRetirar");
	CheckForError(hEventRetirar);
	hEventGestao = CreateEvent(NULL, TRUE, FALSE, L"EventoGestao");
	CheckForError(hEventGestao);
	hEventProcesso = CreateEvent(NULL, TRUE, FALSE, L"EventoProcesso");
	CheckForError(hEventProcesso);

	//Temporizadores
	hTemporizadorSup = CreateWaitableTimer(NULL, FALSE, L"TemporizadorSup");
	hTemporizadorPCP = CreateSemaphore(NULL, 0, 1, L"TemporizadorPCP");

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

	ConnectNamedPipe(hPipeNotificacaoProducao, NULL);
	ConnectNamedPipe(hPipeNotificacaoProcesso, NULL);
	ConnectNamedPipe(hPipeGestaoProducao, NULL);

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


	//Desconectando Pipe
	DisconnectNamedPipe(hPipeGestaoProducao);
	DisconnectNamedPipe(hPipeNotificacaoProcesso);
	DisconnectNamedPipe(hPipeNotificacaoProducao);

	// Fecha todos os handles de objetos do kernel
	for (int i = 0; i < 4; ++i)
		CloseHandle(hThreads[i]);

	// Fecha os handles dos objetos de sincronização
	CloseHandle(hLista1Mutex);
	CloseHandle(hLista2Mutex);
	CloseHandle(hLista1Vazia);
	CloseHandle(hLista2Vazia);
	CloseHandle(hLista1Cheia);
	CloseHandle(hLista2Cheia);
	CloseHandle(hOut);

	//CloseHandle Event
	CloseHandle(hEventEsc);
	CloseHandle(hEventLPCP);
	CloseHandle(hEventLSup);
	CloseHandle(hEventRetirar);
	CloseHandle(hEventGestao);

	CloseHandle(hTemporizadorPCP);
	CloseHandle(hTemporizadorSup);
	CloseHandle(hPipeNotificacaoProcesso);
	CloseHandle(hPipeNotificacaoProducao);
	CloseHandle(hPipeGestaoProducao);

	CloseHandle(hFileLista2);

	//CloseHandle Memoria Compartilhada

	//CloseHandle(hMemoriaCompartilhada);
	//CloseHandle(hEventEnviado);
	//CloseHandle(hEventLido);

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
			std::cout << "Leitura Supervisorio desativada \n";
		}
		else {
			SetEvent(hEventLSup);
			SetConsoleTextAttribute(hOut, HLGREEN);
			std::cout << "Leitura Supervisorio ativada \n";
		}
		leituraSupervisorio->AlterarStatus();
		break;

	case ('p'):
		if (leituraPCP->GetStatus()) {
			ResetEvent(hEventLPCP);
			SetConsoleTextAttribute(hOut, HLRED);
			std::cout << "Leitura PCP desativada \n";
		}
		else {
			SetEvent(hEventLPCP);
			SetConsoleTextAttribute(hOut, HLGREEN);
			std::cout << "Leitura PCP ativada \n";
		}
		leituraPCP->AlterarStatus();
		break;

	case ('r'):
		if (retirarMensagem->GetStatus()) {
			ResetEvent(hEventRetirar);
			SetConsoleTextAttribute(hOut, HLRED);
			std::cout << "Retirar mensagem desativada \n";
		}
		else {
			SetEvent(hEventRetirar);
			SetConsoleTextAttribute(hOut, HLGREEN);
			std::cout << "Retirar mensagem ativada \n";
		}
		retirarMensagem->AlterarStatus();
		break;

	case ('s'):
		PulseEvent(hEventProcesso);
		break;

	case ('e'):
		PulseEvent(hEventGestao);
		break;
	case ('1'):
		WriteFile(hPipeNotificacaoProcesso,
			&sendMsg,
			sizeof(sendMsg),
			NULL,
			NULL);

		break;
	case ('2'):
		WriteFile(hPipeNotificacaoProducao,
			&sendMsg,
			sizeof(sendMsg),
			NULL,
			NULL);

		break;
	case (ESC):
		//Notificar finalizacao por pipe
		WriteFile(hPipeNotificacaoProcesso,
			&sendMsgFalse,
			sizeof(sendMsgFalse),
			NULL,
			NULL);
		WriteFile(hPipeNotificacaoProducao,
			&sendMsgFalse,
			sizeof(sendMsgFalse),
			NULL,
			NULL);
		//Notificar finalizacao por event
		SetEvent(hEventEsc);
		break;

	default:
		break;
	}
}

void GerarDashboard()
{
	SetConsoleTextAttribute(hOut, WHITE);
	std::cout << "Instrucao 'l' - Leitura do Supervisorio \n";
	std::cout << "Instrucao 'p' - Leitura do PCP \n";
	std::cout << "Instrucao 'r' - CapturaMensagens \n";
	std::cout << "Instrucao 's' - DadosProcesso \n";
	std::cout << "Instrucao 'e' - GestaoProducao \n";
	std::cout << "Instrucao 'esc' - Encerrar processos \n";
}

DWORD WINAPI ThreadTeclado() {
	//system("cls");
	GerarDashboard();

	do {
		instrucao = _getch();
		ExecutarInstrucao(instrucao, leituraSupervisorio, leituraPCP, retirarMensagem);
	} while (instrucao != ESC);

	ReleaseSemaphore(hLista1Vazia, 100, NULL);
	ReleaseSemaphore(hLista2Vazia, 200, NULL);

	std::cout << "Thread Teclado terminando... \n";
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
			WaitForSingleObject(hLista1Mutex, 1000);
			WaitForSingleObject(hLista1Vazia, INFINITE);
			leituraSupervisorio->LerMensagem();
			ReleaseSemaphore(hLista1Cheia, 1, NULL);
			ReleaseSemaphore(hLista1Mutex, 1, NULL);
		}
	} while (tipoEvento == 1);

	std::cout << "Thread Leitura Supervisorio terminando... \n";
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadLeituraPCP() {
	HANDLE Events[2] = { hEventEsc, hEventLPCP };
	DWORD ret;
	int tipoEvento, tempoDeLeitura;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;

		if (tipoEvento == 1) {
			tempoDeLeitura = (rand() % 4001) + 1000;
			WaitForSingleObject(hTemporizadorPCP, tempoDeLeitura);
			WaitForSingleObject(hLista1Mutex, 1000);
			WaitForSingleObject(hLista1Vazia, INFINITE);
			leituraPCP->LerMensagem();
			ReleaseSemaphore(hLista1Cheia, 1, NULL);
			ReleaseSemaphore(hLista1Mutex, 1, NULL);

		}
	} while (tipoEvento == 1);

	std::cout << "Thread Leitura PCP terminando... \n";
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadRetirarMensagem() {
	HANDLE Events[2] = { hEventEsc, hEventRetirar };
	DWORD ret;
	int tipoEvento;
	string mensagem;

	char mensagemParaDadosProcesso[51] = "";
	DWORD szMensagemParaDadosProcesso = sizeof(mensagemParaDadosProcesso);
	DWORD szBuffer = sizeof(buffer);
	DWORD dwNoBytesWrite;

	DWORD dwFilePointerPos;

	int numeroDeMensagensDadosProcesso =0;
	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;
		if (tipoEvento == 1) {
			WaitForSingleObject(hLista1Mutex, 1000);
			WaitForSingleObject(hLista1Cheia, INFINITE);
			SetConsoleTextAttribute(hOut, WHITE);
			mensagem = retirarMensagem->RetiraMensagem();
			ReleaseSemaphore(hLista1Vazia, 1, NULL);
			ReleaseSemaphore(hLista1Mutex, 1, NULL);
			
			if (mensagem[0] == '0') {
				//enviar para Gestão de producao PIPE
				strcpy_s(buffer, mensagem.c_str());
				WriteFile(hPipeGestaoProducao,
					buffer,
					szBuffer,
					&dwNoBytesWrite,
					NULL);
			}
			else {
				strcpy_s(mensagemParaDadosProcesso, mensagem.c_str());
				WaitForSingleObject(hLista2Mutex, 1000);
				WaitForSingleObject(hLista2Vazia, INFINITE);
				//lista2->Inserir(mensagem);

				printf("mensagem sendo escrita inferno -> %s\n", mensagemParaDadosProcesso);

				WriteFile(hFileLista2, mensagemParaDadosProcesso,
					szMensagemParaDadosProcesso,
					&dwNoBytesWrite,
					NULL);
				numeroDeMensagensDadosProcesso++;
				dwFilePointerPos = numeroDeMensagensDadosProcesso *szMensagemParaDadosProcesso;
				SetFilePointer(hFileLista2, dwFilePointerPos, NULL, FILE_BEGIN);
				



				std::cout << "Adicionando na segunda lista: " << mensagem << endl;
				ReleaseSemaphore(hLista2Cheia, 1, NULL);
				ReleaseSemaphore(hLista2Mutex, 1, NULL);
			}

			
		}

	} while (tipoEvento == 1);

	std::cout << "Thread RetirarMensagem terminando... \n";
	_endthreadex(0);
	return 0;
}
