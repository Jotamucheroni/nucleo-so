/*Emerson Belancieir de Souza  RA: 151021971 
  Joao Pedro Mucheroni Covolan RA: 151022593
  Lucas Fernandes Nogueira     RA: 151022658
 * Esse programa demonstra o funcionamento do mecanismo de semaforo, atraves
 * do problema do produtor consumidor. O consumidor leva um tempo aleatorio
 * para tentar depositar no buffer, assim como o consumidor espera um tepo aleatorio 
 * para tentar consumir a proxima mensagem, permitindo que seja possivel tanto que
 * o produtor quanto o consumidor sejam bloqueados*/
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

	for(cont = 0; cont < 20; cont++)
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

	for(cont = 0; cont < 20; cont++)
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
	arq_saida = fopen("SSEM.txt", "w");
	printf("Processando...\nVeja a saida apos o termino no arquivo SSEM.TXT\n");
	criar_Processo(produtor, "Processo Produtor", 1);
	criar_Processo(consumidor, "Processo Consumidor", 1);
	dispara_sistema();
}

