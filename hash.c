#include "TLSE.h"

/*
========================================
FUNÇÕES HASH NACIONALIDADE
========================================
*/


//BUSCA BINÁRIO NA TABELA DE NACIONALIDADES
int bb(FILE *fp, char *elem, int tipo){
    int ini, fim, meio; //posições
    //int tam_buscado = strlen(elem);
    char elem_meio[TAM_PAIS+1];
    ini = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    fim = ftell(fp) - tipo;
    while (ini <= fim){
        meio = (((ini + fim) / tipo)/2) * tipo;
        fseek(fp, meio, SEEK_SET);
        fread(elem_meio, sizeof(char), TAM_PAIS, fp);
        elem_meio[TAM_PAIS] = '\0';

        int cmp = strncmp(elem, elem_meio, TAM_PAIS);
        if (cmp == 0) return meio + sizeof(char) * TAM_PAIS;
        if (cmp > 0) ini = meio + tipo;
        if (cmp < 0) fim = meio - tipo;
    }
    return -1;
}

int THN_hash(char *nacionalidade){

    FILE *fp = fopen("nacionalities_table.bin", "rb");
    if (!fp) exit(1);
    int tipo = sizeof(int) + sizeof(char) * 15, idx;
    int desl = bb(fp, nacionalidade, tipo); //achar a posição da nacionalidade no arquivo
    if(desl != -1){
        fseek(fp, desl, SEEK_SET);
        fread(&idx, sizeof(int), 1, fp);
        fclose(fp);
        return idx;
    }
    fclose(fp);
    return -1;
}

//TABELA HASH DE NASCIONALIDADES

void THN_inicializa(char *hashPais, char *dadosPais){
    FILE *fpd = fopen(dadosPais, "wb"); //cria os dois arquivos e faz as verficações se abriram certinho
    if (!fpd) exit(1);
    fclose(fpd);

    FILE *fph = fopen(hashPais, "wb");
    if (!fph) exit(1);

    int end = -1;
    for (int i = 0; i < THN_tam; i++) { //preenche todo o arquivo da hash com -1(null)
        fwrite(&end, sizeof(int), 1, fph);
    }
    fclose(fph);
}

void THN_insere(char *hashPais, char *dadosPais, char *nome, char *nacionalidade){
    FILE *fph = fopen(hashPais, "rb+");
    if (!fph) {
        printf("Erro ao abrir hashPais.bin");
        exit(1);
    }
    
    int h = THN_hash(nacionalidade), pos; // busca binaria para saber a posição onde pais tem que ir na hash
    if(h == -1) {
        fclose(fph);
        return;
    }
    fseek(fph, (h*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);
    int ant, ppl;
    FILE *fpd = fopen(dadosPais, "rb+"); //abrir para edição
    if(!fpd) exit(1);
    ant = -1;
    ppl = -1; //primeira posição livre
    THN aux;
    
    while (pos != -1){ //caso em que houve colisões na hash 
        fseek(fpd, pos, SEEK_SET);
        fread(&aux, sizeof(THN), 1, fpd);
        if (!strcmp(aux.nome, nome)){ //só edita informaçoes
            aux.status = 1;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&aux, sizeof(THN), 1, fpd);
            fclose(fpd); fclose(fph);
            return;
        }
        if ((!aux.status && ppl == -1)) ppl = pos; //marca que ha uma posição livre antes do final do arquivo

        ant = pos;
        pos = aux.prox;
    }


    THN elem; //inicializa novo elemento
    elem.status = 1;
    strncpy(elem.nome, nome, sizeof(elem.nome));
        
    if (ppl == -1){  //caso não haja posiçoes livre no meio do arquivo
        elem.prox = -1;
        fseek(fpd, 0L, SEEK_END);
        pos = ftell(fpd);
        fwrite(&elem, sizeof(THN), 1, fpd);

        if (ant == -1){ //primeiro elemento
            fseek(fph, h*sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fph);
        }
        
        else{
            aux.prox = pos;
            fseek(fpd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THN), 1, fpd);
        }
        fclose(fpd);
        fclose(fph);
        return;
    }
    fseek(fpd, ppl, SEEK_SET);
    fread(&aux, sizeof(THN), 1, fpd);
    elem.prox = aux.prox;
    fseek(fpd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THN), 1, fpd);

    fclose(fpd);
    fclose(fph);
}

TLSEN* THN_buscaPorPais(FILE *fpd, int pos){

    TLSEN* atletasConterraneos = NULL;
    THN elem;
    while (pos != -1){
        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THN), 1, fpd);
        TLSEN_insere_final(&atletasConterraneos, elem);
        pos = elem.prox;
    }

    return atletasConterraneos;
}

void THN_removeAtleta(char *nome, char *nacionalidades){
    FILE *fph = fopen("hashPais.bin", "rb+");
    if (!fph){
        printf("Erro ao abrir hashPais.bin em THN_removeAtleta");
        exit(1);
    }

    int h = THN_hash(nacionalidades), pos;
    if (h == -1) {
        printf("País inexistente dentro da hashPais");
        fclose(fph);
        return;
    }

    fseek(fph, h*sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);
    fclose(fph);

    if (pos == -1) {
        printf("Não existe atletas desse país.");
        return;
    }

    FILE *fpd = fopen("dadosPais.bin", "rb+");
    if (!fpd) {
        printf("Erro ao abrir dadosPais.bin");
        exit(1);
    }

    THN elem;
    while (pos != -1) {
        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THN), 1, fpd);

        if (!strcmp(elem.nome, nome)){
            elem.status = 0;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&elem, sizeof(THN), 1, fpd);
            fclose(fpd); 
            return;
        }
        pos = elem.prox;
    }
    fclose(fpd); 
    printf("O atleta não existe em %s.", nacionalidades);
}

void THN_imp(char *dadosPais){
    FILE *fpd = fopen(dadosPais, "rb");
    if (!fpd) {
        printf("Erro ao abrir %s ", dadosPais);
        exit(1);
    }

    THN elem;

    while(fread(&elem, sizeof(THN), 1, fpd)){
        if(elem.status) printf("Jogador: %s\n", elem.nome);
    }

    fclose(fpd);
}

void THN_imp_nacionalidade(char *hashPais, char *dadosPais, char *nacionalidade){
    FILE *fph = fopen(hashPais, "rb");
    if (!fph) {
        printf("Erro ao abrir hashPais.bin");
        exit(1);
    }
    
    int h = THN_hash(nacionalidade), pos; // busca binaria para saber a posição onde pais tem que ir na hash
    if(h == -1) {
        printf("Não ha atletas nesse pais");
        fclose(fph);
        return;
    }

    fseek(fph, (h*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);

    FILE *fpd = fopen(dadosPais, "rb"); 
    if(!fpd) {
        printf("Erro ao abrir hashPais.bin");
        exit(1);
    }
    THN elem ={0};
    
    while (pos != -1){ //caso em que houve colisões na hash 
        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THN), 1, fpd);
        if(elem.status) printf("Jogador: %s\n", elem.nome);

        pos = elem.prox;
    }

    fclose(fpd);
    fclose(fph);
}


/*
===================================
FUNÇÕES HASH APOSENTADOS
===================================
*/

int hash_Apo(int idade){
    idade += 21;
    //printf(" = %d =", idade);
    if(idade >= 39) return 0; //aposentado
    return 1;
}

void THAN_inicializa(char *hashAposentado, char *dadosAposentado){
    FILE *fpd = fopen(dadosAposentado, "wb"); //cria os dois arquivos e faz as verficações se abriram certinho
    if (!fpd) exit(1);
    fclose(fpd);

    FILE *fph = fopen(hashAposentado, "wb");
    if (!fph) exit(1);

    int end = -1;
    for (int i = 0; i < THAN_tam; i++) { //preenche todo o arquivo da hash com -1(null)
        fwrite(&end, sizeof(int), 1, fph);
    }
    fclose(fph);
}

void THAN_insere(char *hashAposentado, char *dadosAposentado, char *nome, int idade, int anoM){
    if(anoM != 0) return;

    FILE *fph = fopen(hashAposentado, "rb+");
    if (!fph) {
        printf("Erro ao abrir hashAposentado.bin");
        exit(1);
    }
    int h = hash_Apo(idade);
    //if(h == 0) printf("Aposentado: %s\n ", nome);

    int pos;
    fseek(fph, (h*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);

    int ant, ppl;
    FILE *fpd = fopen(dadosAposentado, "rb+"); //abrir para edição
    if(!fpd) exit(1);

    ant = -1;
    ppl = -1; //primeira posição livre
    THAN aux;
    
    
    while (pos != -1){ //caso em que houve colisões na hash 
        fseek(fpd, pos, SEEK_SET);
        fread(&aux, sizeof(THAN), 1, fpd);

        
        if (!strcmp(aux.nome, nome)){ //só edita informaçoes
            aux.status = 1;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&aux, sizeof(THAN), 1, fpd);
            fclose(fpd); fclose(fph);
            return;
        }
        if ((!aux.status && ppl == -1)) ppl = pos; //marca que ha uma posição livre antes do final do arquivo

        ant = pos;
        pos = aux.prox;

    }

    THAN elem; //inicializa novo elemento
    elem.status = 1;
    strncpy(elem.nome, nome, sizeof(elem.nome));
    
        
    if (ppl == -1){  //caso não haja posiçoes livre no meio do arquivo
        elem.prox = -1;
        fseek(fpd, 0L, SEEK_END);
        pos = ftell(fpd);
        //printf("[DEBUG ANO] pos novo escrito = %d (ftell)\n", pos);
        fwrite(&elem, sizeof(THAN), 1, fpd);

        if (ant == -1){ //primeiro elemento
            fseek(fph, h*sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fph);
        }
        
        else{
            aux.prox = pos;
            fseek(fpd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THAN), 1, fpd);
        }
        fclose(fpd);
        fclose(fph);
        return;
    }
    fseek(fpd, ppl, SEEK_SET);
    fread(&aux, sizeof(THAN), 1, fpd);
    elem.prox = aux.prox;
    fseek(fpd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THAN), 1, fpd);

    fclose(fpd);
    fclose(fph);
}

TLSET* THAN_busca(int h){ //pega ou os aposentados ou os ativos;
    
    FILE *fph = fopen("hashAposentado.bin", "rb");
    if (!fph) {
        printf("Erro ao abrir hashAposentado.bin");
        exit(1);
    }

    int pos;
    fseek(fph, (h*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);

    fclose(fph);

    FILE *fpd = fopen("dadosAposentado.bin", "rb"); //abrir para edição
    if(!fpd) exit(1);

    TLSET* aposentados = NULL;
    THAN elem;
    int cont = 0;
    while (pos != -1){
        cont++;
        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THAN), 1, fpd);
        //printf("[BUSCA %d] Lido: %s na pos %d → prox = %d\n", cont, elem.nome, pos, elem.prox);
        if(elem.status) TLSET_insere_final(&aposentados, elem);
        pos = elem.prox;
    }

    fclose(fpd);
    return aposentados;
}

void THAN_removeAtleta(char *nome, int idade){

    FILE *fph = fopen("hashAposentado.bin", "rb");
    if (!fph) {
        printf("Erro ao abrir hashAposentado.bin");
        exit(1);
    }
    int h = hash_Apo(idade);

    int pos;
    fseek(fph, (h*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);

    FILE *fpd = fopen("dadosAposentado.bin", "rb");
    if(!fpd) exit(1);

    THAN elem;

    while (pos != -1) {

        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THAN), 1, fpd);

        if (!strcmp(elem.nome, nome) && elem.status){
            elem.status = 0;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&elem, sizeof(THAN), 1, fpd);
            fclose(fph);
            fclose(fpd);
            return;
        }
        pos = elem.prox;
        
    }
    fclose(fph);
    fclose(fpd);

}



/*
===================================
FUNÇÕES HASH DE TORNEIOS POR ANO
===================================
*/

void THAT_inicializa(char *hashAnoTorneio, char *dadosAnoTorneio){
    FILE *fpd = fopen(dadosAnoTorneio, "wb"); //cria os dois arquivos e faz as verficações se abriram certinho
    if (!fpd) exit(1);
    fclose(fpd);

    FILE *fph = fopen(hashAnoTorneio, "wb");
    if (!fph) exit(1);

    long end = -1;
    for (int i = 0; i < THAT_tam; i++) { //preenche todo o arquivo da hash com -1(null)
        fwrite(&end, sizeof(long), 1, fph);
    }
    fclose(fph);
}

void THAT_insere(char *hashAnoTorneio, char *dadosAnoTorneio, char *nome, int ano,int cont){

    FILE *fph = fopen(hashAnoTorneio, "rb+");
    if (!fph) {
        printf("Erro ao abrir hashAnoTorneio.bin");
        exit(1);
    }
    
    long pos;
    int idade = 2024 - ano;
    
    fseek(fph, (idade*sizeof(long)), SEEK_SET);
    fread(&pos, sizeof(long), 1, fph);

    int ant, ppl;
    FILE *fpd = fopen(dadosAnoTorneio, "rb+"); //abrir para edição
    if(!fpd) exit(1);
    ant = -1;
    ppl = -1; //primeira posição livre
    THAT aux;
    while (pos != -1){ //caso em que houve colisões na hash 
        fseek(fpd, pos, SEEK_SET);
        if (fread(&aux, sizeof(THAT), 1, fpd) != 1) break;
        if (!strcmp(aux.nome, nome)){ //só edita informaçoes
            aux.status = 1;
            switch (cont){
            case 1: case 2: case 3: case 4: aux.gs++; break;
            case 5: aux.atp++; break;
            case 6: aux.olim++; break;
            case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: aux.atpM++; break;
            }
            fseek(fpd, pos, SEEK_SET);
            fwrite(&aux, sizeof(THAT), 1, fpd);
            fclose(fpd); fclose(fph);
            return;
        }
        if ((!aux.status && ppl == -1)) ppl = pos; //marca que ha uma posição livre antes do final do arquivo

        ant = pos;
        pos = aux.prox;
    }

    THAT elem = {0};
    elem.status = 1;
    strncpy(elem.nome, nome, sizeof(elem.nome) - 1);
    elem.nome[sizeof(elem.nome) - 1] = '\0';
    switch (cont){
        case 1: case 2: case 3: case 4: elem.gs++; break;
        case 5: elem.atp++; break;
        case 6: elem.olim++; break;
        case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: elem.atpM++; break;
    }

    if (ppl == -1){  //caso não haja posiçoes livre no meio do arquivo
        elem.prox = -1;
        fseek(fpd, 0L, SEEK_END);
        pos = ftell(fpd);
        fwrite(&elem, sizeof(THAT), 1, fpd);

        if (ant == -1){ //primeiro elemento
            fseek(fph, idade*sizeof(long), SEEK_SET);
            fwrite(&pos, sizeof(long), 1, fph);
        }
        
        else{
            aux.prox = pos;
            fseek(fpd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THAT), 1, fpd);
        }
        fclose(fpd);
        fclose(fph);
        return;
    }
    fseek(fpd, ppl, SEEK_SET);
    fread(&aux, sizeof(THAT), 1, fpd);
    elem.prox = aux.prox;
    fseek(fpd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THAT), 1, fpd);

    fclose(fpd);
    fclose(fph);
}

THAT *THAT_busca(int ano, char *id){
    if(!ano) return NULL;

    FILE *fp = fopen("hashAnoTorneio.bin", "rb");
    if(!fp){
        printf("Erro ao abrir arquivo hashAnoTorneio.bin na funcao THAT_grand_slam");
        exit(1); 
    }
    int idade = 2024 - ano;
    long pos;
    fseek(fp, idade*sizeof(long), SEEK_SET);
    fread(&pos, sizeof(long), 1, fp);
    fclose(fp);

    if(pos == -1) return NULL;

    fp = fopen("dadosAnoTorneio.bin", "rb");
    if(!fp){
        printf("Erro ao abrir arquivo dadosAnoTorneio.bin na funcao THAT_grand_slam");
        exit(1);
    }

    THAT aux = {0};
    while( pos != -1){
        fseek(fp, pos, SEEK_SET);
        if (fread(&aux, sizeof(THAT), 1, fp) != 1) {
            fclose(fp);
            return NULL;
        }

        if(!strcmp(aux.nome, id)){
            if(!aux.status){
                fclose(fp);
                return NULL;
            }

            //alocar o elemento
            THAT *elem = (THAT*)malloc(sizeof(THAT));
            strcpy(elem->nome, aux.nome);
            elem->atp = aux.atp;
            elem->atpM = aux.atpM;
            elem->gs = aux.gs;
            elem->olim = aux.olim;
            elem->status = aux.status;
            fclose(fp);
            return elem;
        }
        pos = aux.prox;
    }
    fclose(fp);
    return NULL;
}

void THAT_removeAtleta(char *dadosAnoTorneio, char *nome){
    FILE *fpd = fopen(dadosAnoTorneio, "rb+");
    if (!fpd) {
        printf("Erro ao abrir %s na THAT_remove", dadosAnoTorneio);
        exit(1);
    }

    THAT elem = {0};
    long pos;
    rewind(fpd);
    while (fread(&elem, sizeof(THAT), 1, fpd)) {

        pos = ftell(fpd) - sizeof(THAT);
        if (!strcmp(elem.nome, nome) && elem.status){
            elem.status = 0;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&elem, sizeof(THAT), 1, fpd);
        }
        
    }
    fclose(fpd);
}

void THAT_imp(char *dadosAnoTorneiro){
    FILE *fpd = fopen(dadosAnoTorneiro, "rb");
    if (!fpd) {
        printf("Erro ao abrir %s ", dadosAnoTorneiro);
        exit(1);
    }

    THAT elem = {0};
    int pos =0;
    while(fread(&elem, sizeof(THAT), 1, fpd)){
        printf("pos ano por torneio: %d\n", pos);
        if(elem.status) printf("Jogador: %s\n", elem.nome);
        pos++;
    }
    if (!feof(fpd)) {
        printf("⚠️ Erro ao ler o arquivo, leitura incompleta!\n");
    }

    printf("Total de registros lidos: %d\n", pos);
    fclose(fpd);
}

/*
===================================
FUNÇÕES HASH DE TITULOS
===================================
*/

void THT_atualiza_titulo(THT *atleta, int cont){
    if (!atleta) return;

    switch(cont){
                case 1: atleta->ao++; break;
                case 2: atleta->fo++; break;
                case 3: atleta->wib++; break;
                case 4: atleta->uso++; break;
                case 5: atleta->atpF++; break;
                case 6: atleta->olim++; break;
                case 7: atleta->atpI++; break;
                case 8: atleta->atpMi++; break;
                case 9: atleta->atpMo++; break;
                case 10: atleta->atpMa++; break;
                case 11: atleta->atpR++; break;
                case 12: atleta->atpCa++; break;
                case 13: atleta->atpCi++; break;
                case 14: atleta->atpS++; break;
                case 15: atleta->atpP++; break;
    }
    
}

void THT_inicializa(char *hashTitulos, char *dadosTitulos){
    FILE *fpd = fopen(dadosTitulos, "wb"); //cria os dois arquivos e faz as verficações se abriram certinho
    if (!fpd) exit(1);
    fclose(fpd);

    FILE *fph = fopen(hashTitulos, "wb");
    if (!fph) exit(1);

    int end = -1;
    for (int i = 0; i < THT_tam; i++) { //preenche todo o arquivo da hash com -1(null)
        fwrite(&end, sizeof(int), 1, fph);
    }
    fclose(fph);
}

void THT_insere(char *hashTitulos, char *dadosTitulos, char *nome, int ntitulos, int cont){
    FILE *fph = fopen(hashTitulos, "rb+");
    if (!fph) {
        printf("Erro ao abrir hashNasc.bin");
        exit(1);
    }
    
    int pos;
    fseek(fph, (ntitulos*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);
    int ant, ppl;

    FILE *fpd = fopen(dadosTitulos, "rb+"); //abrir para edição
    if(!fpd) exit(1);

    ant = -1;
    ppl = -1; //primeira posição livre

    THT aux = {0};

    while (pos != -1){ //caso em que houve colisões na hash 
        fseek(fpd, pos, SEEK_SET);
        fread(&aux, sizeof(THT), 1, fpd);
        if (!strcmp(aux.nome, nome)){ //só edita informaçoes
            aux.status = 1;
            THT_atualiza_titulo(&aux, cont);
            fseek(fpd, pos, SEEK_SET);
            fwrite(&aux, sizeof(THT), 1, fpd);
            fclose(fpd); fclose(fph);
            return;
        }
        if ((!aux.status && ppl == -1)) ppl = pos; //marca que ha uma posição livre antes do final do arquivo

        ant = pos;
        pos = aux.prox;
    }

    THT elem = {0}; //inicializa novo elemento
    elem.status = 1;
    strncpy(elem.nome, nome, sizeof(elem.nome));
    THT_atualiza_titulo(&elem, cont);

    if (ppl == -1){  //caso não haja posiçoes livre no meio do arquivo
        elem.prox = -1;
        fseek(fpd, 0L, SEEK_END);
        pos = ftell(fpd);
        fwrite(&elem, sizeof(THT), 1, fpd);

        if (ant == -1){ //primeiro elemento
            fseek(fph, ntitulos*sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fph);
        }
        
        else{
            aux.prox = pos;
            fseek(fpd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THT), 1, fpd);
        }
        fclose(fpd);
        fclose(fph);
        return;
    }
    fseek(fpd, ppl, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fpd);
    elem.prox = aux.prox;
    fseek(fpd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THT), 1, fpd);

    fclose(fpd);
    fclose(fph);
}

void THT_removeAtleta(char *hashTitulos, char *dadosTitulos, char *nome, int ntitulos){
    FILE *fph = fopen(hashTitulos, "rb+"), *fpd = fopen(dadosTitulos, "rb+");
    if (!fpd || !fph) {
        printf("Erro ao abrir %s ou %s na THT_remove", dadosTitulos, hashTitulos);
        exit(1);
    }

    THT elem = {0};
    int pos;
    fseek(fph, ntitulos*sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);
    while (pos != -1) {

        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THT), 1, fpd);

        if (!strcmp(elem.nome, nome) && elem.status){
            elem.status = 0;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&elem, sizeof(THT), 1, fpd);
            fclose(fph);
            fclose(fpd);
            return;
        }
        pos = elem.prox;
        
    }
    fclose(fph);
    fclose(fpd);
}


/*
===================================
FUNÇÕES HASH DE TORNEIOS
===================================
*/

void THTO_inicializa(char *hashTorneio, char *dadosTorneio){
    FILE *fpd = fopen(dadosTorneio, "wb"); //cria os dois arquivos e faz as verficações se abriram certinho
    if (!fpd) exit(1);
    fclose(fpd);

    FILE *fph = fopen(hashTorneio, "wb");
    if (!fph) exit(1);

    long end = -1;
    for (int i = 0; i < THTO_TAM; i++) { //preenche todo o arquivo da hash com -1(null)
        fwrite(&end, sizeof(long), 1, fph);
    }
    fclose(fph);
}

void THTO_insere(char *hashTorneio, char *dadosTorneio, char *nome, int torneio){
    FILE *fph = fopen(hashTorneio, "rb+");
    if (!fph) {
        printf("Erro ao abrir hashTorneio.bin");
        exit(1);
    }
    
    long pos;
    fseek(fph, (torneio*sizeof(long)), SEEK_SET);
    fread(&pos, sizeof(long), 1, fph);
    long ant, ppl;

    FILE *fpd = fopen(dadosTorneio, "rb+"); //abrir para edição
    if(!fpd) exit(1);

    ant = -1;
    ppl = -1; //primeira posição livre

    THTO aux = {0};

    while (pos != -1){ //caso em que houve colisões na hash 
        fseek(fpd, pos, SEEK_SET);
        fread(&aux, sizeof(THTO), 1, fpd);
        if (!strcmp(aux.id, nome)){ //só edita informaçoes
            aux.status = 1;
            aux.mult++;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&aux, sizeof(THTO), 1, fpd);
            fclose(fpd); fclose(fph);
            return;
        }
        if ((!aux.status && ppl == -1)) ppl = pos; //marca que ha uma posição livre antes do final do arquivo

        ant = pos;
        pos = aux.prox;
    }

    THTO elem = {0}; //inicializa novo elemento
    elem.status = 1;
    elem.mult = 1;
    strncpy(elem.id, nome, sizeof(elem.id));

    if (ppl == -1){  //caso não haja posiçoes livre no meio do arquivo
        elem.prox = -1;
        fseek(fpd, 0L, SEEK_END);
        pos = ftell(fpd);
        fwrite(&elem, sizeof(THTO), 1, fpd);

        if (ant == -1){ //primeiro elemento
            fseek(fph, torneio*sizeof(long), SEEK_SET);
            fwrite(&pos, sizeof(long), 1, fph);
        }
        
        else{
            aux.prox = pos;
            fseek(fpd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THTO), 1, fpd);
        }
        fclose(fpd);
        fclose(fph);
        return;
    }
    fseek(fpd, ppl, SEEK_SET);
    fread(&aux, sizeof(THTO), 1, fpd);
    elem.prox = aux.prox;
    fseek(fpd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THTO), 1, fpd);

    fclose(fpd);
    fclose(fph);
}

void THTO_removeAtleta(char *dadosTorneio, char *nome){
    FILE *fpd = fopen(dadosTorneio, "rb+");
    if (!fpd) {
        printf("Erro ao abrir %s na THOT_remove", dadosTorneio);
        exit(1);
    }

    THTO elem;
    long pos;
    rewind(fpd);
    while (fread(&elem, sizeof(THTO), 1, fpd)) {

        pos = ftell(fpd) - sizeof(THTO);
        if (!strcmp(elem.id, nome) && elem.status){
            elem.status = 0;
            fseek(fpd, pos, SEEK_SET);
            fwrite(&elem, sizeof(THTO), 1, fpd);
        }
    }
    fclose(fpd);
}