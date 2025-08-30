#include "TLSE.h"


//LISTAS THN -> NACIONALIDADES

TLSEN* TLSEN_inicializa(void){
  return NULL;
}

void TLSEN_insere_final(TLSEN **lista, THN atleta){
    TLSEN* novo = (TLSEN*)malloc(sizeof(TLSEN));
    if (!novo){
        printf("Erro ao alocar memória");
        exit(1);
    }

    novo->atleta = atleta;
    novo->prox = NULL;
    if (!(*lista)) (*lista) = novo;
    else{
        TLSEN *aux = (*lista);
        while (aux->prox != NULL) aux = aux->prox;
        aux->prox = novo;
    }
}

void TLSEN_libera(TLSEN *atletas){
    if(atletas){
        TLSEN_libera(atletas->prox);
        free(atletas);
    }
}

void TLSEN_imp(TLSEN *atletas){

    if(atletas){
        int cont =1;
        TLSEN *aux = atletas;
        while(aux){
            printf(" %d : %s \n", cont, aux->atleta.nome);
            aux = aux->prox;
            cont++;
        }
    } 
}

//LISTAS THAN -> APOSENTADOS

TLSET* TLSET_inicializa(void){
  return NULL;
}

void TLSET_insere_final(TLSET **lista, THAN atleta){
    TLSET* novo = (TLSET*)malloc(sizeof(TLSET));
    if (!novo){
        printf("Erro ao alocar memória");
        exit(1);
    }

    novo->atleta = atleta;
    novo->prox = NULL;
    if (!(*lista)) (*lista) = novo;
    else{
        TLSET *aux = (*lista);
        while (aux->prox != NULL) aux = aux->prox;
        aux->prox = novo;
    }
}

int TLSET_busca(TLSET * ativos, char* nome){
    if(!ativos) return 0;
    TLSET *aux = ativos;

    while(aux){
        if(!strcmp(aux->atleta.nome, nome)) return 1;
        aux= aux->prox;
    }
    return 0;
}

void TLSET_imp(TLSET *ativos){

    if(ativos){
        int cont =1;
        TLSET *aux = ativos;
        while(aux){
            printf(" %d : %s \n", cont, aux->atleta.nome);
            aux = aux->prox;
            cont++;
        }
    } 
}

void TLSET_libera(TLSET *ativos){
    if(ativos){
        TLSET_libera(ativos->prox);
        free(ativos);
    }
}

//LISTAS THAT -> ANO POR TORNEIO
TLSEAT* TLSEAT_inicializa(void){
    return NULL;
}

void TLSEAT_insere_final(TLSEAT **lista, char *nome, int ano){
    TLSEAT* novo = (TLSEAT*)malloc(sizeof(TLSEAT));
    if (!novo){
        printf("Erro ao alocar memória");
        exit(1);
    }

    strcpy(novo->nome, nome);
    novo->ano = ano;
    novo->prox = NULL;
    if (!(*lista)) (*lista) = novo;
    else{
        TLSEAT *aux = (*lista);
        while (aux->prox != NULL) aux = aux->prox;
        aux->prox = novo;
    }
}

void TLSEAT_imp(TLSEAT *lista){
    if(lista){
        TLSEAT *aux = lista;
        while(aux){
            printf(" Jogador: %s no ano %d \n", aux->nome, aux->ano);
            aux = aux->prox;
        }
    } 
}

void TLSEAT_libera(TLSEAT *lista){
    if(lista){
        TLSEAT_libera(lista->prox);
        free(lista);
    }
}