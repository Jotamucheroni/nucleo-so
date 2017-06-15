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

extern PTR_DESC_PROC prim;
extern PTR_DESC d_esc;

extern void far volta_dos();

extern void far criar_Processo(void far (*end_proc)(), char nome[35]);

extern void far escalador();

extern void far dispara_sistema();

extern void far terminar_Processo();

