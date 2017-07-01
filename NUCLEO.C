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

APONTA_REG_CRIT a;

PTR_DESC_PROC prim = NULL;
PTR_DESC d_esc;

/*Rotinas basicas-------------------------------------------------------
----------------------------------------------------------------------*/

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

void far criar_Processo(void far (*end_proc)(), char nome[35], int maxfila)
{
	PTR_DESC_PROC p_aux, q_aux;
	PTR_MENSAGEM r_aux;
	p_aux = (PTR_DESC_PROC) malloc(sizeof(DESCRITOR_PROC));
	strcpy(p_aux->nome, nome);
	p_aux->estado = ativo;
	p_aux->contexto = cria_desc();
	newprocess(end_proc, p_aux->contexto);
	/*Cria fila de mensagens vazia*/
	/*Verifica se o tamanho da fila esta dentro dos limites*/
	if(maxfila < 1)
		maxfila = 1;
	else
		if(maxfila > MAXF)
			maxfila = MAXF;
	/*Define a fila e aloca espaco para as mensagens*/
	p_aux->tam_fila = maxfila;
	p_aux->qtd_msg_fila = 0;
	p_aux->fila_msg_recebida = (PTR_MENSAGEM) malloc(sizeof(mensagem));
	r_aux = p_aux->fila_msg_recebida;
	r_aux->flag = 0;
	maxfila--;
	while(maxfila)
	{
		r_aux->prox_msg = (PTR_MENSAGEM) malloc(sizeof(mensagem));
		r_aux = r_aux->prox_msg;
		r_aux->flag = 0;
		maxfila--;
	}
	/*Ultimo elemento aponta para o primeiro - fila circular*/
	/*p_aux->fila_msg_recebida será considerado o ultimo elemento e nao o primeiro,
	 *poupando um ponteiro e permitindo acessar diretamente tanto o ultimo quanto o
	 *primeiro elemento da fila*/
	r_aux->prox_msg = p_aux->fila_msg_recebida;
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

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

/*Comunicacao e sincronizacao entre processos---------------------------
----------------------------------------------------------------------*/

/*Semaforos*/
void far inicia_semaforo(SEMAFORO *sem, int n)
{
	sem->s = n;
	sem->Q = NULL;
}

void far P(SEMAFORO *sem)
{
	PTR_DESC_PROC p_aux;

	disable();
	if(sem->s > 0)
		sem->s--;
	else
	{
		/*Fila vazia*/
		if(sem->Q == NULL)
			sem->Q = prim;
		else
		{
			/*Encontra ultimo elemento da lista de bloqueados*/
			p_aux = sem->Q;
			while(p_aux->fila_sem != NULL)
				p_aux = p_aux->fila_sem;

			/*Insere BCP no final*/
			p_aux->fila_sem = prim;
		}
		prim->fila_sem = NULL;
		prim->estado = bloq_P;

		/*Enviar controle para proximo processo ativo*/
		p_aux = prim;
		if( (prim = procura_prox_ativo()) == NULL)
			volta_dos();
		transfer(p_aux->contexto, prim->contexto);
	}
	enable();
}

void far V(SEMAFORO *sem)
{
	PTR_DESC_PROC p_aux;
	disable();
	if(sem->Q == NULL) sem->s++;
	else
	{
		p_aux = sem->Q;
		sem->Q = sem->Q->fila_sem;
		p_aux->estado = ativo;
		p_aux->fila_sem = NULL;
	}
	enable();
}

/*Troca de mensagens*/
int far envia(char *msg, char *destino)
{
	PTR_DESC_PROC p_aux;
	PTR_MENSAGEM q_aux;

	disable();
	p_aux = prim;
	/*Procura processo receptor*/
	while(p_aux->prox_desc != prim && strcmp(p_aux->nome, destino) != 0)
		p_aux = p_aux->prox_desc;
	/*Nao achou receptor*/
	if(strcmp(p_aux->nome, destino) != 0)
	{
		enable();
		return 0;	
	}
	/*Fila do receptor cheia*/
	if(p_aux->qtd_msg_fila == p_aux->tam_fila)
	{
		enable();
		return 1;	
	}
	/*Procura espaco vazio na fila de mensagens do receptor*/
	q_aux = p_aux->fila_msg_recebida->prox_msg;
	while(q_aux->flag)
		q_aux = q_aux->prox_msg;
	/*Escreve a mensagem no espaco vazio*/
	q_aux->flag = 1;
	strcpy(q_aux->msg, msg);
    strcpy(q_aux->nome_emissor, prim->nome);
	p_aux->qtd_msg_fila++;
	/*Desbloqueia o receptor, caso esteja bloqueado esperando mensagem*/
	if(p_aux->estado == bloq_rec)
		p_aux->estado = ativo;
	/*Bloqueia-se, esperando confirmacao de que a mensagem foi recebida*/
	prim->estado = bloq_env;
	/*Transfere o controle para o proximo processo ativo*/
	p_aux = prim;
	if( (prim = procura_prox_ativo()) == NULL)
			volta_dos();
	transfer(p_aux->contexto, prim->contexto);
	return 2;	/*Sucesso - retorna da proxima vez que o processo emissor for despachado*/
}

void far recebeQualquer(char *msg, char *emissor)
{
	PTR_DESC_PROC p_aux;
	disable();
	/*Verifica se a fila de mensagens esta vazia*/
	if(prim->qtd_msg_fila == 0)
	{
		/*Bloqueia-se e passa controle para outro processo*/
		prim->estado = bloq_rec;
		p_aux = prim;
		if( (prim = procura_prox_ativo()) == NULL)
			volta_dos();
		transfer(p_aux->contexto, prim->contexto);
		disable();
	}
	/*Copia o conteudo da primeira mensagem*/
	strcpy(msg, prim->fila_msg_recebida->prox_msg->msg);
	strcpy(emissor, prim->fila_msg_recebida->prox_msg->nome_emissor);
	/*Retira a mensagem lida da fila*/
	prim->fila_msg_recebida->prox_msg->flag = 0;
	prim->qtd_msg_fila--;
	prim->fila_msg_recebida = prim->fila_msg_recebida->prox_msg;
	/*Procura procura processo emissor e desbloqueia*/
	p_aux = prim;
	while(strcmp(p_aux->nome, emissor) != 0)
		p_aux = p_aux->prox_desc;
	p_aux->estado = ativo;
	enable();
}

void far recebeEspecifico(char *msg, char *emissor)
{
	PTR_DESC_PROC p_aux;
	PTR_MENSAGEM q_aux, r_aux;
	char achou;
	disable();
	/*Verifica se a fila de mensagens esta vazia*/
	if(prim->qtd_msg_fila == 0)
	{
		/*Bloqueia-se e passa controle para outro processo*/
		prim->estado = bloq_rec;
		p_aux = prim;
		if( (prim = procura_prox_ativo()) == NULL)
			volta_dos();
		transfer(p_aux->contexto, prim->contexto);
		disable();
	}
	/*Verifica se existe mensagem do emissor desejado*/
	do
	{
		achou = 1;
		q_aux = prim->fila_msg_recebida->prox_msg;
		r_aux = prim->fila_msg_recebida;
		while(q_aux->flag && q_aux != prim->fila_msg_recebida && strcmp(q_aux->nome_emissor, emissor) != 0)
		{
			r_aux = q_aux;
			q_aux = q_aux->prox_msg;
		}
		if(q_aux->flag != 1 || strcmp(q_aux->nome_emissor, emissor) != 0)
		{
			achou = 0;
			prim->estado = bloq_rec;
			p_aux = prim;
			if( (prim = procura_prox_ativo()) == NULL)
				volta_dos();
			transfer(p_aux->contexto, prim->contexto);
			disable();
		}
	}while(achou == 0);
	
	/*Copia o conteudo da mensagem*/
	strcpy(msg, q_aux->msg);
	/*Retira a mensagem lida da fila*/
	q_aux->flag = 0;
	if(q_aux != prim->fila_msg_recebida)
	{
		r_aux->prox_msg = q_aux->prox_msg;
		q_aux->prox_msg = prim->fila_msg_recebida->prox_msg;
		prim->fila_msg_recebida->prox_msg = q_aux;
		prim->fila_msg_recebida = q_aux;
	}
	prim->qtd_msg_fila--;
	/*Procura procura processo emissor e desbloqueia*/
	p_aux = prim;
	while(strcmp(p_aux->nome, emissor) != 0)
		p_aux = p_aux->prox_desc;
	p_aux->estado = ativo;
	enable();
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

