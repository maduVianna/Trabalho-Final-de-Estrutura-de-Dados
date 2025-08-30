#ifndef RESPOSTAS_H
#define RESPOSTAS_H
#include "TLSE.h"
#include "insere_arv.h"

typedef struct tp{
    char id[51];
    int pontuacao;
}TP;

typedef struct atleta_in_atp_final{
    char id[51];
    int pontuacao;
    int gs;
    int atp;
    long ano; //só o ano, pontuação é mais caro e só calculo se precisar
}T8M;

//QUESTÃO 1
int max_titulos(THT *atleta, int *gs, int *atpm);
void imp_titulos(FILE *fp);
void Resp_titulos();

//QUESTÃO 2
int compatriota();

//QUESTÃO 3
int ler_ano(char *id);

void mapear_pontuacoes(char *raiz, char *primFolha, int t);
void ordenar_3(char *arquivo);
//void qs_aux(char *narq, int tipo, int ini, int fim);
//void qsPontuacao(char *narq);
void pontuacao_acum(char *arqPontuacao);

//QUESTÃO 4
void ranking_25(char *hashAnoTorneio, char *dadosAnoTorneio);

//QUESTÃO 5
void all_gs(char *hashAnoTorneio, char *dadosAnoTorneio);

//QUESTÃO 6
int furar_rank(char *hashTitulos, char *dadosTitulos, int torneio);

//questão 7 (retirar todos os jogadores em atividade ou não de um determinado país.
int max_titulosM(infos_atleta *jogador);
void remover_atletas(char *nacionalidades, char *raiz, int t, char *primFolha);

//QUESTÃO 8

int pont_anterior(T8M* atleta);
int compara_completa(const void *p1, const void* p2);
void participaram_atp(char *hashAnoTorneio, char *dadosAnoTorneio);

//QUESTÃO 9
void ordem_torneio(char *hashTorneio, char *dadosTorneio);
#endif