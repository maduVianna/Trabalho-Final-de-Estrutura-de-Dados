#ifndef MAPEANDO_ID_H
#define MAPEANDO_ID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defines.h"

typedef struct id{
    char id[TAM_ID];
    char nome[TAM_NOME];
    char sobrenome[TAM_NOME];
}TID;

void trim(char *nome);

void ultimo_sobrenome(char *entrada, char *saida);

void libera_id(TID **vet_ids);
int compara_ID(const void *p1, const void* p2);

void ordenar_id(TID **vet_ids, int pos);

int busca_id(char *id, char *sobrenome, char *temp);

int busca_vetId(TID **vet_ids, TID* id, int tam);

void preenche_vetId(TID **vet_id, TID *id, int *pos);

TID* montar_id(char *nome,char * anoNasc, char *rank);

void imp_ids_table();



int compara_sob(const void *p1, const void* p2);
void ordenar_sob(TID **vet_ids, int pos);
int busca_sob(char *id, char *sobrenome);
#endif