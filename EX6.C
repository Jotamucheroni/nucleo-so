#include <stdio.h>
#include <dos.h> far
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <nucleo.h>

FILE *arq_saida;

void far emissorA()
{
	unsigned espera, cont;
	char Mensagem[25], i;

	for(cont = 1; cont <= 10; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		delay(espera*100);
		/*Deposita mensagem*/
		sprintf(Mensagem, "A%u", cont);
		i = envia(Mensagem, "Recep");
		if (i==0)
		{
			printf("Nao achou destino! Abortar");
			terminar_Processo();
		}
		else
			while (i == 1) /* fila do destino cheia */
				i = envia(Mensagem, "Recep");
	}
	terminar_Processo();
}

void far emissorB()
{
	unsigned espera, cont;
	char Mensagem[25], i;

	for(cont = 1; cont <= 10; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		delay(espera*100);
		/*Deposita mensagem*/
		sprintf(Mensagem, "B%u", cont);
		i = envia(Mensagem, "Recep");
		if (i==0)
		{
			printf("Nao achou destino! Abortar");
			terminar_Processo();
		}
		else
			while (i == 1) /* fila do destino cheia */
				i = envia(Mensagem, "Recep");
	}
	terminar_Processo();
}

void far receptor()
{
	unsigned espera, cont;
	char Mensagem[25], Emissor[35];

	for(cont = 0; cont < 20; cont++)
	{
		/*Retira mensagem*/
		strcpy(Emissor, (cont % 10) < 5 ? "EB" : "EA" );
		recebeEspecifico(Mensagem, Emissor);
			
		fprintf(arq_saida, "Receptor recebeu %s do processo %s\n", Mensagem, Emissor);
		printf("Receptor recebeu %s do processo %s\n", Mensagem, Emissor);
		/*Consome mensagem*/
		espera = rand() % 3 + 1;
		delay(espera*100);
	}
	terminar_Processo();
}

main()
{
	srand(time(NULL));
	arq_saida = fopen("saidaM2.txt", "w");
	criar_Processo(emissorA, "EA", 1);
	criar_Processo(emissorB, "EB", 1);
	criar_Processo(receptor, "Recep", 50);
	dispara_sistema();
}
