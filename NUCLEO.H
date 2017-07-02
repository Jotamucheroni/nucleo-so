#include <system.h>
#include <stdio.h>
#include <string.h>

#define MAX_P 3

typedef struct desc_process
{
	char nome[35];
	enum {ativo, terminado} estado;
	int prioridade;
	PTR_DESC contexto;
	struct desc_process *prox_desc;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

typedef struct registros{
	unsigned bx1,es1;
}regis;

typedef union k{
	regis x;
	char far *y;
}APONTA_REG_CRIT;

extern APONTA_REG_CRIT a;

extern PTR_DESC_PROC prim;
extern PTR_DESC_PROC fila_prior[4];
extern int cont_fila_prior;

extern PTR_DESC d_esc;

extern void far volta_dos();

extern void far criar_Processo(void far (*end_proc)(), char nome[35], int prior);

extern void far escalador();

extern void far dispara_sistema();

extern void far terminar_Processo();

