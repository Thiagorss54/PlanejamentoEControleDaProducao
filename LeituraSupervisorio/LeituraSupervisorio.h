#pragma once
#include <string>
#include "../PlanejamentoEControleDaProducao/ListaEncadeada.h"

struct Mensagem
{
	int tipo;
	int nseq;
	float tzona1;
	float tzona2;
	float tzona3;
	float volume;
	float pressao;
	char  timestamp[13];
	
};
class LeituraSupervisorio {
private:
	bool status;
	int nseq;
	ListaEncadeada* listaEncadeada;

public:
	LeituraSupervisorio(ListaEncadeada * listaEncadeada_);
	void AlterarStatus();
	bool GetStatus();
	void LerMensagem(); //simula a leitura através da geração de dados	

};


