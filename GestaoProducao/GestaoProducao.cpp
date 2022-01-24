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

int main()
{
    SetConsoleTitle(L"Gestao da Producao");
    bool status = FALSE;
    hEventGestao = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoGestao");
    hEventEsc = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoEsc");
    HANDLE Events[2] = { hEventEsc, hEventGestao };
    DWORD ret;
    int tipoEvento;
    
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

    CloseHandle(hEventGestao);
}
