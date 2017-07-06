/* Emerson Belancieir de Souza  RA: 151021971
   Joao Pedro Mucheroni Covolan RA: 151022593
   Lucas Fernandes Nogueira     RA: 151022658
 * Esse programa demonstra o funcionamento do nucleo basico. Cada processo
 * escreve constantemente um caracter especifico que o identifica ate que
 * seja trocas pelo escalador*/
#include <stdio.h>
#include <nucleo.h>

FILE *arq_saida;

void far processo1()
{
	long int i = 0;
	while(i < 100000)
	{
           fprintf(arq_saida, "@");
           i++;
	}
	terminar_Processo();
}

void far processo2()
{
	long int i = 0;
	while(i < 100000)
        {
           fprintf(arq_saida, "X");
           i++;
	}
	terminar_Processo();
}

main()
{
	arq_saida = fopen("saidaBas.txt", "w");
	printf("Processando...\nVeja a saida apos o termino no arquivo SAIDABas.TXT");
	criar_Processo(processo1, "Processo @", 1);
	criar_Processo(processo2, "Processo X", 1);
	dispara_sistema();
}

