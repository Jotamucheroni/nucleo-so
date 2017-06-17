#include <stdio.h>
#include <dos.h> far
#include <stdlib.h>
#include <time.h>
#include <nucleo.h>

SEMAFORO vazia, cheia, mutex;
int buffer[10], contP = 0, contC = 0;
FILE *arq_saida;

void far produtorA()
{
	unsigned espera;
	char continua = 1;

	while(continua)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
		/*Deposita mensagem*/
		P(&vazia);
		P(&mutex);
		if(contP < 100)
		{
			buffer[contP%10] = contP;
			fprintf(arq_saida, "ProdutorA depositou %d na celula %d\n", contP, contP%10);
			printf("ProdutorA depositou %d na celula %d\n", contP, contP%10);
			contP++;
		}
		else
			continua = 0;
		V(&mutex);
		V(&cheia);
	}
	terminar_Processo();
}

void far produtorB()
{
	unsigned espera;
	char continua = 1;

	while(continua)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
		/*Deposita mensagem*/
		P(&vazia);
		P(&mutex);
		if(contP < 100)
		{
			buffer[contP%10] = contP;
			fprintf(arq_saida, "ProdutorB depositou %d na celula %d\n", contP, contP%10);
			printf("ProdutorB depositou %d na celula %d\n", contP, contP%10);
			contP++;
		}
		else
			continua = 0;
		V(&mutex);
		V(&cheia);
	}
	terminar_Processo();
}

void far consumidorA()
{
	unsigned espera, mensagem;
	char continua = 1;

	while(continua)
	{
		/*Retira mensagem*/
		P(&cheia);
		P(&mutex);
		if(contC < 100)
		{
			mensagem = buffer[contC%10];
			fprintf(arq_saida, "ConsumidorA retirou %d da celula %d\n", mensagem, contC%10);
			printf("ConsumidorA retirou %d da celula %d\n", mensagem, contC%10);
			contC++;
		}
		else
			continua = 0;
		V(&mutex);
		V(&vazia);
		/*Consome mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
	}
	terminar_Processo();
}

void far consumidorB()
{
	unsigned espera, mensagem;
	char continua = 1;

	while(continua)
	{
		/*Retira mensagem*/
		P(&cheia);
		P(&mutex);
		if(contC < 100)
		{
			mensagem = buffer[contC%10];
			fprintf(arq_saida, "ConsumidorB retirou %d da celula %d\n", mensagem, contC%10);
			printf("ConsumidorB retirou %d da celula %d\n", mensagem, contC%10);
			contC++;
		}
		else
			continua = 0;
		V(&mutex);
		V(&vazia);
		/*Consome mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
	}
	terminar_Processo();
}

main()
{
	inicia_semaforo(&vazia, 10);
	inicia_semaforo(&cheia, 0);
	inicia_semaforo(&mutex, 1);
	srand(time(NULL));
	arq_saida = fopen("saida.txt", "w");
	criar_Processo(produtorA, "Processo ProdutorA");
	criar_Processo(produtorB, "Processo ProdutorB");
	criar_Processo(consumidorA, "Processo ConsumidorA");
	criar_Processo(consumidorB, "Processo ConsumidorB");
	dispara_sistema();
}

