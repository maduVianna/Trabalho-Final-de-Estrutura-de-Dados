#include "mapeando_id.h"

//======== MAPEAMENTO ID ========

void trim(char *nome){
    int len = strlen(nome);
    while( len > 0 && isspace((unsigned char)nome[len-1])) nome[--len] = '\0';
}

void ultimo_sobrenome(char *entrada, char *saida){
    trim(entrada);
    char *ultmoEspaco = strrchr(entrada, ' ');

    if(ultmoEspaco) strcpy(saida, ultmoEspaco + 1); //se tiver mais de um sobrenome eu a ultima informação
    else strcpy(saida, entrada); // caso não, mantenho o sobrenome
    
}

void libera_id(TID **vet_ids){

    for (int i = 0; i < 400; i++){
        if (vet_ids[i]) free(vet_ids[i]); //libera a memória alocada para cada id
        else break; //se não tiver mais id, sai do loop
    }
    free(vet_ids);
}

int compara_ID(const void *p1, const void* p2){
    TID *id1 = *(TID**)p1;
    TID *id2 = *(TID**)p2;
    return strcmp(id1->sobrenome, id2->sobrenome);
}

void ordenar_id(TID **vet_ids, int pos){
    //ordenando o vetor de países
    qsort(vet_ids, pos, sizeof(TID*), compara_ID);

   
    //gravando os dados em arquivo binário com tamanho da string(qntd de caracteres do país), nome do país, índice
    FILE *fpbin = fopen("ids_table.bin", "wb+");
    if (!fpbin) {
        perror("Erro ao abrir ids_table.bin para escrita");
        exit(1);
    }

    for(int i=0; i< pos; i++){
        fwrite(vet_ids[i], sizeof(TID),1, fpbin);
    }
   
    fclose(fpbin);
}

int compara_sob(const void *p1, const void* p2){
    TID *id1 = *(TID**)p1;
    TID *id2 = *(TID**)p2;
    return strcmp(id1->id, id2->id);
}

void ordenar_sob(TID **vet_ids, int pos){
    //ordenando o vetor de países
    qsort(vet_ids, pos, sizeof(TID*), compara_sob);


    FILE *fpbin = fopen("sobrenome_table.bin", "wb+");
    if (!fpbin) {
        perror("Erro ao abrir sobrenome_table para escrita");
        exit(1);
    }

    for(int i=0; i< pos; i++){
        fwrite(vet_ids[i], sizeof(TID),1, fpbin);
    }
   
    fclose(fpbin);
}

int busca_vetId(TID **vet_ids, TID* id, int tam){
    if(!vet_ids) return 0;
    int i=0;
    while(i<tam)
        if(!strcmp(vet_ids[i++]->id, id->id)) return 1;
    return 0;
}

void preenche_vetId(TID **vet_id, TID *id, int *pos){
    //se a nacionalidade ainda não está no vetor países coloca
    if(!busca_vetId(vet_id, id, *pos))
            vet_id[(*pos)++] = id;

}

TID * montar_id(char *nome, char *ano, char*rank){

    char id[TAM_ID] = {0};
    char *sobrenome = strrchr(nome, ' '); //pega só o sobrenome
    if (sobrenome) sobrenome++; // pula o espaço

    trim(sobrenome);
    strncat(id, nome, 3);
    strcat(id,ano);
    strcat(id,rank);
    strncat(id, sobrenome, 2);

    TID *elem = (TID*)malloc(sizeof(TID));

    strncpy(elem->id, id, TAM_ID);
    elem->id[TAM_ID -1] = '\0';

    strncpy(elem->nome, nome, TAM_NOME);
    elem->nome[TAM_NOME -1] = '\0';

    strncpy(elem->sobrenome, sobrenome, TAM_NOME);
    elem->sobrenome[TAM_NOME -1] = '\0';

    //printf("\n %s\n ", sobrenome);
    return elem;
}

void imp_ids_table(){
    FILE *fp = fopen("ids_table.bin", "rb");
    if(!fp) exit(1);

    TID id = {0};
    fseek(fp, 0L, SEEK_SET);
    while(fread(&id, sizeof(TID), 1, fp)){
        printf("Sobrenome: %s ID: %s\n", id.sobrenome, id.id);
    }
    fclose(fp);
    return ;
}

//BUSCA BINÁRIO NA TABELA DE IDS PELO SOBRENOME
int busca_id(char *id, char *sobrenome, char *temp){
    FILE *fp = fopen("ids_table.bin", "rb");
    if(!fp){
        perror("Erro ao abrir arquivo ids_table.bin na função busca_ID");
        exit(1);
    }
    int tipo = sizeof(TID);
    int ini, fim, meio; //posições
    //int tam_buscado = strlen(elem);
    TID elem_meio = {0};
    ini = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    fim = ftell(fp) - tipo;
    while (ini <= fim){
        meio = (((ini + fim) / tipo)/2) * tipo;
        fseek(fp, meio, SEEK_SET);
        fread(&elem_meio, sizeof(TID), 1, fp);

        int cmp = strcmp(sobrenome, elem_meio.sobrenome);
        if ((cmp == 0) && (temp[1] == '.') ){
            if (strncmp(temp, elem_meio.nome, 1) == 0) { // achei quem eu quero
                strcpy(id, elem_meio.id);
                fclose(fp);
                return 1;
            }

            int i = meio - tipo; //olhar na metade superior ao sobrenome
            while (i >= ini) {
                fseek(fp, i , SEEK_SET);
                fread(&elem_meio, sizeof(TID), 1, fp);
                if (strcmp(sobrenome, elem_meio.sobrenome) != 0) break;
                if (strncmp(temp, elem_meio.nome, 1) == 0) {
                    strcpy(id, elem_meio.id);
                    fclose(fp);
                    return 1;
                }
                i -= tipo;
            }
            i = meio + tipo; //olhar na metade inferior ao sobrenome
            while (i <= fim) { 
                fseek(fp, i , SEEK_SET);
                fread(&elem_meio, sizeof(TID), 1, fp);
                if (strcmp(sobrenome, elem_meio.sobrenome) != 0) break;
                if (strncmp(temp, elem_meio.nome, 1) == 0) {
                    strcpy(id, elem_meio.id);
                    fclose(fp);
                    return 1;
                }
                i += tipo;
            }
            // se não achou ninguém com a inicial do nome igual
            break;
        } 
        else if(cmp == 0){
            strcpy(id, elem_meio.id);
            fclose(fp);
            return 1;
        }
        if (cmp > 0) ini = meio + tipo;
        if (cmp < 0) fim = meio - tipo;
    }
    fclose(fp);
    return 0;
}

//BUSCA BINARIA NA TEBELA PELO SOBRENOME NA TABELA IDS
int busca_sob(char *id, char *sobrenome){
    FILE *fp = fopen("sobrenome_table.bin", "rb");
    if(!fp){
        perror("Erro ao abrir arquivo sobrenome_table.bin na função busca_ID");
        exit(1);
    }
    int tipo = sizeof(TID);
    int ini, fim, meio; //posições
    TID elem_meio = {0};
    ini = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    fim = ftell(fp) - tipo;
    while (ini <= fim){
        meio = (((ini + fim) / tipo)/2) * tipo;
        fseek(fp, meio, SEEK_SET);
        fread(&elem_meio, sizeof(TID), 1, fp);

        int cmp = strcmp(id, elem_meio.id);
        if (cmp == 0){
            fclose(fp);
            strcpy(sobrenome, elem_meio.nome);
            return 1;
        } 
        if (cmp > 0) ini = meio + tipo;
        if (cmp < 0) fim = meio - tipo;
    }
    fclose(fp);
    return 0;
}