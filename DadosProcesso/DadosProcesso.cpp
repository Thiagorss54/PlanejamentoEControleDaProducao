#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);	// Casting para terceiro e sexto parâmetros da função
                                                    // _beginthreadex
typedef unsigned* CAST_LPDWORD;

HANDLE hEventProcesso;
HANDLE hEventEsc;

int main()
{
    bool status = FALSE;
    hEventProcesso = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoProcesso");
    hEventEsc = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoEsc");
    HANDLE Events[2] = { hEventEsc, hEventProcesso };
    DWORD ret;
    int tipoEvento;

    do {
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        tipoEvento = ret - WAIT_OBJECT_0;
        if (tipoEvento == 1) {
            std::cout << "STATUS:  ";
            if (status) {
                std::cout << "BLOQUEADO" << std::endl;
            }
            else {
                std::cout << "ATIVO" << std::endl;
            }
            status = !status;
        }

    } while (tipoEvento == 1);



    CloseHandle(hEventProcesso);
}
