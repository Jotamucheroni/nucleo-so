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

PTR_DESC_PROC procura_prox_ativo()
{
	PTR_DESC_PROC p_aux;

	if(prim == NULL) return NULL;
	p_aux = prim;
	do
	{
		p_aux = p_aux->prox_desc;
		if(p_aux->estado == ativo)
			return p_aux;
	}while(p_aux != prim);
	return NULL;
}

void far volta_dos()
{
	disable();
	setvect(8, p_est->int_anterior);
	enable();
	exit(0);
}

void far criar_Processo(void far (*end_proc)(), char nome[35])
{
	PTR_DESC_PROC p_aux, q_aux;
	p_aux = (PTR_DESC_PROC) malloc(sizeof(DESCRITOR_PROC));
	strcpy(p_aux->nome, nome);
	p_aux->estado = ativo;
	p_aux->contexto = cria_desc();
	newprocess(end_proc, p_aux->contexto);
	/*Inserir no final da lista circular*/
	if(prim == NULL)
	{

                prim = p_aux;
		p_aux->prox_desc = p_aux;
	}
	else
	{
		/*Encontra ultimo elemento da lista*/
		q_aux = prim;
		while(q_aux->prox_desc != prim)
			q_aux = q_aux->prox_desc;

	        /*Insere novo BCP no final*/
                q_aux->prox_desc = p_aux;
                p_aux->prox_desc = prim;
        }
}

void far escalador()
{
        p_est->p_origem = d_esc;
        p_est->p_destino = prim->contexto;
        p_est->num_vetor = 8;

        while(1)
	{
		iotransfer();
		disable();
		if( (prim = procura_prox_ativo()) == NULL)
			volta_dos();
		p_est->p_destino = prim->contexto;
		enable();
	}
}

void far dispara_sistema()
{
	PTR_DESC d_dispara;
        d_dispara = cria_desc();
	d_esc = cria_desc();
	newprocess(escalador, d_esc);
	transfer(d_dispara, d_esc);
}

void far terminar_Processo()
{
	PTR_DESC_PROC p_aux;
	disable();
	prim->estado = terminado;
	p_aux = prim;
	if( (prim = procura_prox_ativo()) == NULL )
		volta_dos();
	transfer(p_aux->contexto, prim->contexto);
}

