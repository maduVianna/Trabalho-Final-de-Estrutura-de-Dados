#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_PAIS 15
#define THN_tam 51
#define THAN_tam 3
#define THAT_tam 35
#define THT_tam 73
#define TAM_ID 12
#define TAM_NOME 51
#define THTO_TAM 16



typedef struct nacionalidade{
    char nome[TAM_ID];
    int status;
    int prox;
}THN;

typedef struct anoNasc{
    char nome[TAM_ID];
    int status;
    int prox;
}THAN;

typedef struct anotorneios{
    char nome[TAM_ID];
    int gs;
    int atpM;
    int olim;
    int atp;
    int status;
    long prox;
}THAT;

typedef struct titulos{
    char nome[TAM_ID];
    int ao, fo, wib, uso, atpF, olim;
    int atpI, atpMi, atpMo, atpMa, atpR, atpCa, atpCi, atpS, atpP;
    int status, prox;
}THT;

typedef struct torneio{
    char id[TAM_ID];
    int mult;
    int status;
    long prox;
}THTO;

//BUSCA BINÁRIO NA TABELA DE NACIONALIDADES
int bb(FILE *fp, char *elem, int tipo);

//TABELA HASH DE NASCIONALIDADES
int THN_hash(char *nacionalidade);
void THN_inicializa(char *hashPais, char *dadosPais);
void THN_insere(char *hashPais, char *dadosPais, char *nome, char *nacionalidade);
void THN_removeAtleta(char *nome, char *nacionalidades);
void THN_imp(char *dadosPais);
void THN_imp_nacionalidade(char *hashPais, char *dadosPais, char *nacionalidade);


//TABELA HASH ATIVO OU NAO
int hash_Apo(int idade);
void THAN_inicializa(char *hashNasc, char *dadosAno);
void THAN_insere(char *hashAposentado, char *dadosAposentado, char *nome, int idade, int anoM);
void THAN_removeAtleta(char *nome, int idade);


//TABELA HASH TORNEIOS POR ANO
void THAT_inicializa(char *hashAnoTorneio, char *dadosAnoTorneio);
void THAT_insere(char *hashAnoTorneio, char *dadosAnoTorneio, char *nome, int ano,int cont);
THAT *THAT_busca(int ano, char *id);
void THAT_removeAtleta(char *dadosAnoTorneio, char *nome);
void THAT_imp(char *dadosAnoTorneiro);


//TABELA HASH NUMERO DE TITULOS
void THT_atualiza_titulo(THT *atleta, int cont);
void THT_inicializa(char *hashTitulos, char *dadosTitulos);
void THT_insere(char *hashTitulos, char *dadosTitulos, char *nome, int ntitulos, int cont);
void THT_removeAtleta(char *hashTitulos, char *dadosTitulos, char *nome, int ntitulos);


//TABELA HASH ATIPOS DE TORNEIOS
void THTO_inicializa(char *hashTorneio, char *dadosTorneio);
void THTO_insere(char *hashTorneio, char *dadosTorneio, char *nome, int torneio);
void THTO_removeAtleta(char *dadosTorneios, char *nome);

#endif