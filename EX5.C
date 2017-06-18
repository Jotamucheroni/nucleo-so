#include <stdio.h>
#include <dos.h> far
#include <stdlib.h>
#include <time.h>
#include <nucleo.h>

FILE *arq_saida;

void far emissor()
{
	unsigned espera, cont;
	char Mensagem[25], i;

	for(cont = 0; cont < 100; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
		/*Deposita mensagem*/
		sprintf(Mensagem, "%u", cont);
		i = envia(Mensagem, "PR");
		if (i==0)
		{
			printf("Nao achou destino! Abortar");
			terminar_Processo();
		}
		else
			while (i == 1) /* fila do destino cheia */
				i = envia(Mensagem, "PR");
	}
	terminar_Processo();
}

void far receptor()
{
	unsigned espera, cont;
	char Mensagem[25], Emissor[35];

	for(cont = 0; cont < 200; cont++)
	{
		/*Retira mensagem*/
		recebe(Mensagem, Emissor);
		fprintf(arq_saida, "O processo destino recebeu %s do processo %s\n", Mensagem, Emissor);
		printf("O processo destino recebeu %s do processo %s\n", Mensagem, Emissor);
		/*Consome mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
	}
	terminar_Processo();
}

main()
{
	srand(time(NULL));
	arq_saida = fopen("saidaM.txt", "w");
	criar_Processo(emissor, "E", 30);
	criar_Processo(emissor, "F", 30);
	criar_Processo(receptor, "PR", 30);
	dispara_sistema();
}

