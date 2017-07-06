#include <system.h>
#include <stdio.h>
#include <string.h>

#define MAXF 50

typedef struct address{
	char flag;
	char nome_emissor[35];
	char msg[25];
	struct address *prox_msg;
}mensagem;

typedef mensagem *PTR_MENSAGEM;

typedef struct desc_process
{
	char nome[35];
	enum {ativo, bloq_P, bloq_env, bloq_rec, terminado} estado;
	PTR_DESC contexto;
	int tam_fila; /*Tamanho da fila de mensagens*/
	int qtd_msg_fila;
	struct desc_process *prox_desc;
	struct desc_process *fila_sem;
	PTR_MENSAGEM  fila_msg_recebida;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

typedef struct registros{
	unsigned bx1,es1;
}regis;

typedef union k{
	regis x;
	char far *y;
}APONTA_REG_CRIT;

typedef struct semaforo
{
	int s;
	PTR_DESC_PROC Q;
}SEMAFORO;

extern APONTA_REG_CRIT a;

extern PTR_DESC_PROC prim;

extern PTR_DESC d_esc;

/*Rotinas basicas-------------------------------------------------------
----------------------------------------------------------------------*/

extern void far volta_dos();

extern void far criar_Processo(void far (*end_proc)(), char nome[35], int maxfila);

extern void far escalador();

extern void far dispara_sistema();

extern void far terminar_Processo();

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

/*Comunicacao e sincronizacao entre processos---------------------------
----------------------------------------------------------------------*/

/*Semaforos*/
extern void far inicia_semaforo(SEMAFORO *sem, int n);

extern void far P(SEMAFORO *sem);

extern void far V(SEMAFORO *sem);

/*Troca de mensagens*/
extern int far envia(char *msg, char *destino);

extern void far recebeQualquer(char *msg, char *emissor);

extern void far recebeEspecifico(char *msg, char *emissor);

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

