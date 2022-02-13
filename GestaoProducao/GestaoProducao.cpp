#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <vector>
#include <string>

#define _CHECKERROR 1
#include "CheckForError.h"

#include "../PlanejamentoEControleDaProducao/FuncoesAuxiliares.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
													// _beginthreadex
typedef unsigned* CAST_LPDWORD;

// Criando handles
HANDLE hEventGestao;
HANDLE hEventEsc;
HANDLE hPipeNotificacaoProducao, hPipeGestaoProducao;
HANDLE hEventGestaoProducaoSent, hEventGestaoProducaoRead;

// Declarando threads
DWORD WINAPI ThreadLimparConsole();
DWORD WINAPI ThreadReceberMensagens();

// Declarando variáveis globais
bool status = FALSE;

// Declarando funções
void ImprimirMensagem(std::string buffer);

int main() {
	SetConsoleTitle(L"Gestao da Producao");

	// Abrindo eventos
	hEventGestao = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoGestao");
	CheckForError(hEventGestao);
	
	hEventEsc = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoEsc");
	CheckForError(hEventEsc);
	
	hEventGestaoProducaoSent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"GestaoProducaoSent");
	CheckForError(hEventGestaoProducaoSent);
	
	hEventGestaoProducaoRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"GestaoProducaoRead");
	CheckForError(hEventGestaoProducaoRead);

	// Pipe
	WaitNamedPipe(L"\\\\.\\pipe\\GESTAO", NMPWAIT_WAIT_FOREVER);
	WaitNamedPipe(L"\\\\.\\pipe\\NOTIFICACAOPRODUCAO", NMPWAIT_WAIT_FOREVER);

	hPipeNotificacaoProducao = CreateFile(
		L"\\\\.\\pipe\\NOTIFICACAOPRODUCAO",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	CheckForError(hPipeNotificacaoProducao);

	hPipeGestaoProducao = CreateFile(
		L"\\\\.\\pipe\\GESTAO",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	CheckForError(hPipeGestaoProducao);

	HANDLE hThreads[2];
	DWORD dwIdLimparConsole;

	//Thread LimparConsole
	hThreads[0] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadLimparConsole,
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdLimparConsole);
	CheckForError(hThreads[0]);
	if (hThreads[0] != (HANDLE)-1L)
		printf("Thread LimparConsole criada com Id=%0x\n", dwIdLimparConsole);
	else {
		printf("Erro na criacao da thread LimparConsole ! N = %d Erro = %d\n", 0, errno);
		exit(0);
	}

	// Thread Receber Mensagens
	hThreads[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ThreadReceberMensagens,
		(LPVOID)(INT_PTR)0,
		0,
		(CAST_LPDWORD)&dwIdLimparConsole);
	CheckForError(hThreads[1]);
	if (hThreads[1] != (HANDLE)-1L)
		printf("Thread ReceberMensagens criada com Id=%0x\n", dwIdLimparConsole);
	else {
		printf("Erro na criacao da thread ReceberMensagens ! N = %d Erro = %d\n", 0, errno);
		exit(0);
	}

	system("cls");

	// Aguarda comando ESC
	WaitForSingleObject(hEventEsc, INFINITE);

	// Esperar as threads acabar
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

	// Fechando Handles
	for (int i = 0; i < 2; ++i) {
		CloseHandle(hThreads[i]);
	}

	CloseHandle(hEventGestao);
	CloseHandle(hEventEsc);
	CloseHandle(hPipeNotificacaoProducao);
	CloseHandle(hPipeGestaoProducao);
	CloseHandle(hEventGestaoProducaoSent);
	CloseHandle(hEventGestaoProducaoRead);
}

DWORD WINAPI ThreadLimparConsole() {
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

	} while (result != FALSE);

	std::cout << "Thread LimparConsole terminando ...\n";
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ThreadReceberMensagens() {
	HANDLE Events[2] = { hEventEsc, hEventGestao };
	DWORD ret;
	int tipoEvento;
	char buffer[32];
	DWORD szBuffer = sizeof(buffer);
	DWORD dwNoBytesRead;
	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		tipoEvento = ret - WAIT_OBJECT_0;

		if (tipoEvento == 1) {
			ReadFile(hPipeGestaoProducao,
				buffer,
				szBuffer,
				&dwNoBytesRead,
				NULL);

			ImprimirMensagem(buffer);
		}
	} while (tipoEvento == 1);

	std::cout << "Thread ReceberMensagem terminando ...\n";
	_endthreadex(0);
	return 0;
}

void ImprimirMensagem(std::string buffer) {
	std::vector<std::string> msg;
	msg = FuncoesAuxiliares::SepararString(buffer, "#");
	if (msg.size() != 5) {
		return;
	}
	std::cout << "NSEQ: " << msg[1] 
			  << " OP: " << msg[2]
			  << " HORA DE INICIO: " << msg[3]
			  << " DURACAO: " << msg[4] 
			  << std::endl;
}