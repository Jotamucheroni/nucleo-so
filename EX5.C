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

	for(cont = 0; cont < 25; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
		/*Deposita mensagem*/
		sprintf(Mensagem, "A%u", cont);
		i = envia(Mensagem, "RA");
		if (i==0)
		{
			printf("Nao achou destino! Abortar");
			terminar_Processo();
		}
		else
			while (i == 1) /* fila do destino cheia */
				i = envia(Mensagem, "RA");
	}
	terminar_Processo();
}

void far emissorB()
{
	unsigned espera, cont;
	char Mensagem[25], i;

	for(cont = 0; cont < 25; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
		/*Deposita mensagem*/
		sprintf(Mensagem, "B%u", cont);
		i = envia(Mensagem, "RB");
		if (i==0)
		{
			printf("Nao achou destino! Abortar");
			terminar_Processo();
		}
		else
			while (i == 1) /* fila do destino cheia */
				i = envia(Mensagem, "RB");
	}
	terminar_Processo();
}

void far emissorR()
{
	unsigned espera, cont;
	char Mensagem[25], i, Receptor[35];

	for(cont = 0; cont < 10; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 4 + 1;
		sleep(espera);
		if(espera % 2)
			strcpy(Receptor, "RA");
		else
			strcpy(Receptor, "RB");
		/*Deposita mensagem*/
		sprintf(Mensagem, "R%u", cont);
		i = envia(Mensagem, Receptor);
		if (i==0)
		{
			printf("Nao achou destino! Abortar");
			terminar_Processo();
		}
		else
			while (i == 1) /* fila do destino cheia */
				i = envia(Mensagem, Receptor);
	}
	terminar_Processo();
}

void far receptorA()
{
	unsigned espera, cont;
	char Mensagem[25], Emissor[35];

	for(cont = 0; cont < 25; cont++)
	{
		/*Retira mensagem*/
		recebe(Mensagem, Emissor);
		fprintf(arq_saida, "Receptor A recebeu %s do processo %s\n", Mensagem, Emissor);
		printf("Receptor A recebeu %s do processo %s\n", Mensagem, Emissor);
		/*Consome mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
	}
	terminar_Processo();
}

void far receptorB()
{
	unsigned espera, cont;
	char Mensagem[25], Emissor[35];

	for(cont = 0; cont < 25; cont++)
	{
		/*Retira mensagem*/
		recebe(Mensagem, Emissor);
		fprintf(arq_saida, "Receptor B recebeu %s do processo %s\n", Mensagem, Emissor);
		printf("Receptor B recebeu %s do processo %s\n", Mensagem, Emissor);
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
	criar_Processo(emissorA, "EA", 1);
	criar_Processo(emissorB, "EB", 1);
	criar_Processo(emissorR, "ER", 1);
	criar_Processo(receptorA, "RA", 50);
	criar_Processo(receptorB, "RB", 50);
	dispara_sistema();
}

