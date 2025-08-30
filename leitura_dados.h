#ifndef LEITURA_DADO_H
#define LEITURA_DADO_H

#include "hash.h"
#include "mapeando_id.h"
#include "respostas.h"
#include "insere_arv.h"
#include "mapeando_nascionalidade.h"


int cal_idade(char *ano);

void hash_inicializar();

void ler_campeaos(char* raiz_atual, int t);

void ler_tenistas(int t, int tam_NI, char* raiz_atual);

void iniciar(int t, char *raiz_atual, char *primFolha);

#endif