#pragma once
#include <string>
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

public:
	LeituraSupervisorio();
	void AlterarStatus();
	bool GetStatus();
	void LerMensagem(); //simula a leitura através da geração de dados	

};


