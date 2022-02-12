#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <vector>

#include "../PlanejamentoEControleDaProducao/FuncoesAuxiliares.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
                                                    // _beginthreadex
typedef unsigned* CAST_LPDWORD;

HANDLE hEventProcesso;
HANDLE hEventEsc;
HANDLE hPipeNotificacaoProcesso;
HANDLE hEventRecebeMsg;
HANDLE hFileLista2;
HANDLE hLista2Mutex;

void ImprimirMensagem(std::string buffer);

DWORD WINAPI ThreadLimparConsole();
DWORD WINAPI ThreadReceberMensagens();

bool status = FALSE;

int main()
{
    SetConsoleTitle(L"Dados do Processo");
    
    hEventProcesso = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoProcesso");
    hEventEsc = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoEsc");
    hEventRecebeMsg = CreateEvent(NULL, TRUE, FALSE, L"RecebeMsgDadosProcesso");
    hLista2Mutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"Lista2Mutex");

    //Pipe
    hPipeNotificacaoProcesso = CreateFile(
        L"\\\\.\\pipe\\NOTIFICACAOPROCESSO",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    //Abrindo o arquivo para a lista 2
    hFileLista2 = CreateFile(L"..\\..\\FileDadosProcesso.txt",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFileLista2 == INVALID_HANDLE_VALUE) {
        std::cout << "ERROR -> " << GetLastError() << std::endl;
    }
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

    HANDLE Events[2] = { hEventEsc, hEventProcesso };
    DWORD ret;
    int tipoEvento;


    do {
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        tipoEvento = ret - WAIT_OBJECT_0;
        if (tipoEvento == 1) {
            status = !status;
            std::cout << "STATUS:  ";
            if (status) {
                std::cout << "ATIVO\n" ;
                SetEvent(hEventRecebeMsg);
            }
            else {
                std::cout << "BLOQUEADO\n";
                ResetEvent(hEventRecebeMsg);
            }
            
        }

    } while (tipoEvento == 1);
    

    //esperar a thread acabar
    WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);


    for (int i = 0; i < 2; ++i) {
        CloseHandle(hThreads[i]);
    }

    CloseHandle(hEventProcesso);
    CloseHandle(hPipeNotificacaoProcesso);
    CloseHandle(hEventRecebeMsg);
    CloseHandle(hFileLista2);
  
}

DWORD WINAPI ThreadLimparConsole()
{   

    bool result;
    do {
        ReadFile(hPipeNotificacaoProcesso,
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
    
    HANDLE EventsR[2] = { hEventEsc, hEventRecebeMsg };
    DWORD ret;
    int tipoEvento;
    char buffer[51]="";
    DWORD szBuffer = sizeof(buffer);
    DWORD dwNoBytesRead;
    do {
        ret = WaitForMultipleObjects(2, EventsR, FALSE, INFINITE);
        tipoEvento = ret - WAIT_OBJECT_0;

        if (tipoEvento == 1) {
           
           ReadFile(hFileLista2,
                buffer,
                szBuffer,
                &dwNoBytesRead,
                NULL);


            std::cout << buffer << std::endl;
            //SetEvent(hEventGestaoProducaoRead);
        }
    } while (tipoEvento == 1);
    std::cout << "Thread ReceberMensagem terminando ...\n";
    _endthreadex(0);
    return 0;
}

void ImprimirMensagem(std::string buffer) {
    std::vector<std::string> msg;
    msg = FuncoesAuxiliares::SepararString(buffer, "#");
    std::cout << "NSEQ: " << msg[1] << " TZ1: " << msg[2] << " TZ2: " << msg[3] << " TZ3: " << msg[4] << " VOL: " << msg[5] << " PRES: " << msg[6] << " HORA: " << msg[7] << std::endl;
}

