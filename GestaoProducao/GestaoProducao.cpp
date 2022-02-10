#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#define _CHECKERROR 1
#include "../GestaoProducao/CheckForError.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
													// _beginthreadex
typedef unsigned* CAST_LPDWORD;

HANDLE hEventGestao;
HANDLE hEventEsc;
HANDLE hPipeNotificacaoProducao, hPipeGestaoProducao;
HANDLE hEventGestaoProducaoSent, hEventGestaoProducaoRead;
HANDLE hEventRecebeMsg;

DWORD WINAPI ThreadLimparConsole();
DWORD WINAPI ThreadReceberMensagens();


bool status = FALSE;

int main()
{
	SetConsoleTitle(L"Gestao da Producao");

	hEventGestao = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoGestao");
	hEventEsc = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoEsc");
	hEventGestaoProducaoSent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"GestaoProducaoSent");
	hEventGestaoProducaoRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"GestaoProducaoRead");


	hEventRecebeMsg = CreateEvent(NULL, TRUE, FALSE, L"RecebeMsg");

	HANDLE Events[2] = { hEventEsc, hEventGestao };
	DWORD ret;
	int tipoEvento;



	//Pipe
	hPipeNotificacaoProducao = CreateFile(
		L"\\\\.\\pipe\\NOTIFICACAOPRODUCAO",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	hPipeGestaoProducao = CreateFile(
		L"\\\\.\\pipe\\GESTAO",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);


	//Thread LimparConsole


	HANDLE hThreads[2];
	DWORD dwIdLimparConsole;

	hThreads[0] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLimparConsole,
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdLimparConsole);
	//CheckForError(hThread);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread LimparConsole criada com Id=%0x\n", dwIdLimparConsole);
	else {
		printf("Erro na criacao da thread LimparConsole ! N = %d Erro = %d\n", 0, errno);
		exit(0);
	}

	hThreads[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadReceberMensagens,
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdLimparConsole);
	//CheckForError(hThread);
	if (hThreads[1] != (HANDLE)-1L)
		printf("Thread ReceberMensagens criada com Id=%0x\n", dwIdLimparConsole);
	else {
		printf("Erro na criacao da thread ReceberMensagens ! N = %d Erro = %d\n", 0, errno);
		exit(0);
	}


	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;
		if (tipoEvento == 1) {
			status = !status;
			if (status) {
				SetEvent(hEventRecebeMsg);
			}
			else {
				ResetEvent(hEventRecebeMsg);
			}
		}

	} while (tipoEvento == 1);


	//esperar a thread acabar
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);



	for (int i = 0; i < 2; ++i) {
		CloseHandle(hThreads[i]);
	}

	CloseHandle(hPipeNotificacaoProducao);
	CloseHandle(hEventGestao);
	CloseHandle(hPipeGestaoProducao);
	CloseHandle(hEventRecebeMsg);
}

DWORD WINAPI ThreadLimparConsole()
{
	bool result;
	do {
		ReadFile(hPipeNotificacaoProducao,
			&result,
			sizeof(result),
			NULL,
			NULL);

		if (result != FALSE) {
			system("cls");
		}
		printf("resutl -> %d\n", result);

	} while (result != FALSE);

	std::cout << "Thread LimparConsole terminando ...\n";
	_endthreadex(0);

	return 0;
}

DWORD WINAPI ThreadReceberMensagens() {
	HANDLE EventsR[2] = { hEventEsc, hEventRecebeMsg };
	DWORD ret;
	int tipoEvento;
	char buffer[32];
	do {
		ret = WaitForMultipleObjects(2, EventsR, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;

		if (tipoEvento == 1) {
			WaitForSingleObject(hEventGestaoProducaoSent, INFINITE);
			ResetEvent(hEventGestaoProducaoSent);
			ReadFile(hPipeGestaoProducao,
				buffer,
				sizeof(buffer),
				NULL,
				NULL);

			if (buffer[0] != 'F') {
				std::cout << buffer << std::endl;
			}
			SetEvent(hEventGestaoProducaoRead);
		}
	} while (tipoEvento == 1);
	std::cout << "Thread ReceberMensagem terminando ...\n";
	_endthreadex(0);
	return 0;
}