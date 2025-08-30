#ifndef TLSE_H
#define TLSE_H

#include "hash.h"

typedef struct lsen{
    THN atleta;
    struct lsen *prox;
}TLSEN;

typedef struct lset{
    THAN atleta;
    struct lset *prox;
}TLSET;

typedef struct lseat{
    char nome[TAM_NOME];
    int ano;
    struct lseat *prox;
}TLSEAT;


//auxliares nacionalidade;
TLSEN* TLSEN_inicializa(void);
void TLSEN_insere_final(TLSEN **lista, THN atleta);
TLSEN* THN_buscaPorPais(FILE *fpd, int pos);
void TLSEN_libera(TLSEN *atletas);
void TLSEN_imp(TLSEN *atletas);

//auxliares aposentados;
TLSET* TLSET_inicializa(void);
void TLSET_insere_final(TLSET **lista, THAN atleta);
TLSET* THAN_busca(int h);
int TLSET_busca(TLSET * ativos, char* nome);
void TLSET_imp(TLSET *ativos);
void TLSET_libera(TLSET *ativos);

//auxliares ano por torneio;
TLSEAT* TLSEAT_inicializa(void);
void TLSEAT_insere_final(TLSEAT **lista, char *nome, int ano);
void TLSEAT_imp(TLSEAT *lista);
void TLSEAT_libera(TLSEAT *lista);

#endif