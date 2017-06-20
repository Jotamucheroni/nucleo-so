#include <stdio.h>
#include <dos.h> far
#include <stdlib.h>
#include <time.h>
#include <nucleo.h>

SEMAFORO vazia, cheia, mutex;
int buffer[10];
FILE *arq_saida;

void far produtor()
{
	unsigned espera, cont;

	for(cont = 0; cont < 100; cont++)
	{
		/*Produz mensagem*/
		espera = rand() % 3 + 1;
		sleep(espera);
		/*Deposita mensagem*/
		P(&vazia);
		P(&mutex);
		buffer[cont%10] = cont;
		fprintf(arq_saida, "Produtor depositou %d na celula %d\n", cont, cont%10);
		printf("Produtor depositou %d na celula %d\n", cont, cont%10);
		V(&mutex);
		V(&cheia);
	}
	terminar_Processo();
}

void far consumidor()
{
	unsigned espera, cont, Mensagem;

	for(cont = 0; cont < 100; cont++)
	{
		/*Retira mensagem*/
		P(&cheia);
		P(&mutex);
		Mensagem = buffer[cont%10];
		fprintf(arq_saida, "Consumidor retirou %d da celula %d\n", Mensagem, cont%10);
		printf("Consumidor retirou %d da celula %d\n", Mensagem, cont%10);
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
	criar_Processo(produtor, "Processo Produtor");
	criar_Processo(consumidor, "Processo Consumidor");
	dispara_sistema();
}

