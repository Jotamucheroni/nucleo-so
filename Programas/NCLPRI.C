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

APONTA_REG_CRIT a;

PTR_DESC_PROC prim = NULL;
PTR_DESC_PROC fila_prior[4] = {NULL, NULL, NULL, NULL};
int cont_fila_prior;

PTR_DESC d_esc;

PTR_DESC_PROC procura_prox_ativo()
{
	PTR_DESC_PROC p_aux;
	int nivel;

	/*Inicia a busca pelo próximo processo apontado por prim e salva a
	 * o indice da lista que estava sendo executada */
	p_aux = prim->prox_desc;
	nivel = cont_fila_prior;
	/*Procura um processo ativo começando por aqueles com maior prioridade
	 * (0) e indo ate os de menor prioridade (MAX_P) e voltando depois para
	 * a primeira lista (0), caso nao econtre*/
	while(p_aux != prim || cont_fila_prior != nivel)	/*Caso o ponteiro volte para o mesmo BCP, na mesma fila, todos os processos ja foram percorridos*/
	{
		/*Verifica se o ponteiro chegou no final de uma fila*/
		if(p_aux == NULL)
		{
			cont_fila_prior++;						/*Vai para a proxima fila*/
			if(cont_fila_prior > MAX_P)
				cont_fila_prior = 0;				/*Volta para primeira fila*/
			p_aux = fila_prior[cont_fila_prior];
		}
		else
			if(p_aux->estado == ativo)
				return p_aux;
			else 
				p_aux = p_aux->prox_desc;
	}
	
	/*Voltamos ao processo que estava sendo executado. Como ele e o ultimo
	 * que pode estar ativo, o controle deve retornar para ele caso esteja 
	 * ativo ou retornar para o DOS caso nao esteja */ 
	if(p_aux->estado == ativo)
		return p_aux;
	return NULL;
}

void far volta_dos()
{
	disable();
	setvect(8, p_est->int_anterior);
	enable();
	exit(0);
}

void far criar_Processo(void far (*end_proc)(), char nome[35], int prior)
{
	PTR_DESC_PROC p_aux, q_aux;
	p_aux = (PTR_DESC_PROC) malloc(sizeof(DESCRITOR_PROC));
	strcpy(p_aux->nome, nome);
	p_aux->estado = ativo;
	if(prior < 0)
		prior = 0;
	else
		if(prior > MAX_P)
			prior = MAX_P;
	p_aux->prioridade = prior;
	p_aux->contexto = cria_desc();
	newprocess(end_proc, p_aux->contexto);
	/*Inserir no final da fila da respectiva prioridade*/
	p_aux->prox_desc = NULL;
	if(fila_prior[prior] == NULL)
        fila_prior[prior] = p_aux;
	else
	{
		/*Encontra ultimo elemento da fila*/
		q_aux = fila_prior[prior];
		while(q_aux->prox_desc != NULL)
			q_aux = q_aux->prox_desc;

	    /*Insere novo BCP no final*/
        q_aux->prox_desc = p_aux;
    }
}

void far escalador()
{
        p_est->p_origem = d_esc;
        p_est->p_destino = prim->contexto;
        p_est->num_vetor = 8;

        /*Inicia ponteiro para R.C. do DOS*/
        _AH=0x34;
        _AL=0x00;
        geninterrupt(0x21);
        a.x.bx1=_BX;
        a.x.es1=_ES;

        while(1)
	{
		iotransfer();
		disable();
		/* se nucleo nao estiver na R.C. troca o processo*/
		if (!*a.y)
		{
			if( (prim = procura_prox_ativo()) == NULL)
				volta_dos();
			p_est->p_destino = prim->contexto;
		}
		enable();
	}
}

void far dispara_sistema()
{
	PTR_DESC d_dispara;
	PTR_DESC_PROC p_aux;
	int i, nivel_ant;
	
	/*Fazer o ultimo elemento das filas apontar para
	 * a proxima fila não vazia de maior prioridade */
	for(i = MAX_P; i > 0; i--)
	{
		/*verifica se a fila esta vazia. Se estiver, vai para o proximo nivel de prioridade (mais alto)*/
		if(fila_prior[i] == NULL)
			continue;
			
		/*Procurar ultimo elemento da fila*/
		p_aux = fila_prior[i];
		while(p_aux->prox_desc != NULL)
			p_aux = p_aux->prox_desc;
		
		/*Encontrar a primeira fila de mais alto nivel nao vazia e fazer o 
		 * o ultimo elemento da fila atual apontar para ela. Caso nao exista,
		 * apontar para a fila de mais alta prioridade (NULL)*/
		for(nivel_ant = i-1; fila_prior[nivel_ant] == NULL && nivel_ant != 0; nivel_ant--);
		
		p_aux->prox_desc = fila_prior[nivel_ant];
	}
	/*Procura primeiro processo de mais alta prioridade*/
	for(i = 0; i < MAX_P && fila_prior[i] == NULL; i++);
	
	if(fila_prior == NULL)
		volta_dos();
	prim = fila_prior[i];
	cont_fila_prior = i;
		
    /*Transfere o controle para o escalador*/
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
