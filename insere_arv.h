#ifndef INSERE_ARV_H
#define INSERE_ARV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "defines.h"

extern int cont_folhas;
extern int cont_ni;

typedef struct{
    char id[TAM_ID];
    char nome[TAM_NOME];
    int rank;
    int morte;
    int anoMelRank;
    int idade;
    char nasc[TAM_PAIS];
    int ao, fo, wib, uso;
    int atpF, olim; 
    int iw, mi, mc, md, rm, ca, ci, sh, pa; 
} infos_atleta;

typedef struct {
    int nchaves;
    char prox[TAM_NO];
} TARVBM_F;

typedef struct {
    char id_no[TAM_NO];
    int nchaves;
    char **chaves;
    char **filhos;
} TARVBM_NI;


int tam_bloco(int t);
long calc_offset_ni(char* nome_no, int t);
void salva_no(char* nome_arq, TARVBM_NI* no, int t);
void libera_no(TARVBM_NI* no, int t);
void TARVBM_F_cria();
void TARVBM_NI_cria(char* arq_indice, int t, int tam_NI);
TARVBM_NI* TARVBM_NI_aux_cria(char* arq_indice, char* nome_no, int t);
void imprimir_folha(char* nome_no);
void TARVBM_divide(char* arq_indice, char* nome_pai, char* nome_filho, int pos_filho, int t, int tam_NI); 
void divide_primeira_folha(char* arq_indice, int t, int tam_NI);
void insere_incompleto(char* arq_indice, char* nome_no, infos_atleta* jogador, int t, int tam_NI);
void TARVBM_insere(char *arq_indice, infos_atleta* jogador, char* raiz_atual, int t, int tam_NI);
infos_atleta *TARVBM_F_busca(char *nomeArqFol, char *id, char* nome_fantasia);
infos_atleta *TARVBM_NI_busca(char *nomeArqInd, char *id, char *raiz, int t, char* nome_fantasia, char* prim_folha);
void TARVBM_F_atualizarJogador(infos_atleta* jogador, char* raiz_atual, int t);
void TARBM_imprime_rec(char* arq_indice, char* raiz, int nivel, int t);
void listar_folhas(char *nomeArqInd, int* num_folhas, char** folhas, char *primFolha, char *raiz, int t);
void TARVBM_remove(char *nomeArqInd, char *id, char *raiz_atual, int t, char *primFolha);
void compactar_posicao(char *nomeArqInd, long posVazia, long raizOffset, char *raiz_atual, int *posPai, int t);
void decrementar_id_no(char *id_no);

#endif