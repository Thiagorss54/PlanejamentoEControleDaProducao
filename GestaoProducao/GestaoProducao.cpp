#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
                                                    // _beginthreadex
typedef unsigned* CAST_LPDWORD;

HANDLE hEventGestao;
HANDLE hEventEsc;
HANDLE hPipeNotificacaoProducao;



DWORD WINAPI ThreadLimparConsole();


int main()
{
    SetConsoleTitle(L"Gestao da Producao");
    bool status = FALSE;
    hEventGestao = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoGestao");
    hEventEsc = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoEsc");
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

    //Thread LimparConsole

    HANDLE hThread;
    DWORD dwIdLimparConsole;

    hThread = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadLimparConsole,
        (LPVOID)(INT_PTR)0,
        0,
        (CAST_LPDWORD)&dwIdLimparConsole);
    //CheckForError(hThread);
    if (hThread != (HANDLE)-1L)
        printf("Thread LimparConsole criada com Id=%0x\n", dwIdLimparConsole);
    else {
        printf("Erro na criacao da thread LimparConsole ! N = %d Erro = %d\n", 0, errno);
        exit(0);
    }




    do {
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        tipoEvento = ret - WAIT_OBJECT_0;
        if (tipoEvento == 1) {
            std::cout << "STATUS:  ";
            if (status) {
                std::cout << "BLOQUEADO\n";
            }
            else {
                std::cout << "ATIVO\n";
            }
            status = !status;
        }

    } while (tipoEvento == 1);
    

    //esperar a thread acabar
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hPipeNotificacaoProducao);
    CloseHandle(hEventGestao);
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

    } while (result != FALSE);

    std::cout << "Thread LimparConsole terminando ...\n";
    _endthreadex(0);

    return 0;
}
