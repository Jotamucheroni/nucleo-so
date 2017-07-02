#include <stdio.h>
#include <dos.h> far
#include <nucleo.h>

FILE *arq_saida;

void far processo1()
{
	unsigned int i = 1;
	while(i <= 12000)
	{
           fprintf(arq_saida, (i%80) ? "1": "1\n");
           i++;
	}
	terminar_Processo();
}

void far processo2()
{
	unsigned int i = 1;
	while(i <= 12000)
	{
           fprintf(arq_saida, (i%80) ? "2": "2\n");
           i++;
	}
	terminar_Processo();
}

void far processo3()
{
	unsigned int i = 1;
	while(i <= 8000)
	{
           fprintf(arq_saida, (i%80) ? "3": "3\n");
           i++;
	}
	terminar_Processo();
}

void far processo4()
{
	unsigned int i = 1;
	while(i <= 8000)
	{
           fprintf(arq_saida, (i%80) ? "4": "4\n");
           i++;
	}
	terminar_Processo();
}

void far processo5()
{
	unsigned int i = 1;
	while(i <= 8000)
	{
           fprintf(arq_saida, (i%80) ? "5": "5\n");
           i++;
	}
	terminar_Processo();
}

void far processo6()
{
	unsigned int i = 1;
	while(i <= 4000)
	{
           fprintf(arq_saida, (i%80) ? "6": "6\n");
           i++;
	}
	terminar_Processo();
}

main()
{
	arq_saida = fopen("saidaPri.txt", "w");
	printf("Processando...\nVeja a saida apos o termino no arquivo SAIDAPRI.TXT");
	criar_Processo(processo1, "P1", 0);
	criar_Processo(processo2, "P2", 0);
	criar_Processo(processo3, "P3", 1);
	criar_Processo(processo4, "P4", 1);
	criar_Processo(processo5, "P5", 1);
	criar_Processo(processo6, "P6", 3);
	dispara_sistema();
}

