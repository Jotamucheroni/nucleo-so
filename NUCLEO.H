#include <system.h>
#include <stdio.h>
#include <string.h>

typedef struct desc_process
{
	char nome[35];
	enum {ativo, terminado} estado;
	PTR_DESC contexto;
	struct desc_process *prox_desc;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;
PTR_DESC_PROC prim = NULL;
PTR_DESC d_esc;

void far volta_dos();

void far criar_Processo(void far (*end_proc)(), char nome[35]);

void far escalador();

void far dispara_sistema();

void far terminar_Processo();
