/****************************************************************************************************

          394959 HORAS DEPOIS ESSE SPOILER DO INFERNO ESTÁ RODANDO COM SUCESSO !!!!!!!!

*****************************************************************************************************/

#include "insere_arv.h"

int cont_folhas = 0;
int cont_ni = 0;

int tam_bloco(int t){
    return sizeof(char) * TAM_ID + sizeof(int) + (sizeof(char) * TAM_NOME_NO * (2 * t - 1)) + (sizeof(char) * TAM_NO * (2 * t));
}

//REVISADA - OK 
long calc_offset_ni(char* nome_no, int t){
    //Função auxiliar para saber onde pegar as estruturas correspondentes aos NI que quermos no arwuivo de índice
    if(!strcmp(nome_no, "")) return -1;
    char aux[TAM_NOME_NO];
    for(int j=1; j<TAM_NOME_NO - 1; j++) aux[j-1] = nome_no[j];
    aux[TAM_NOME_NO-1] = '\0';
    int tam = tam_bloco(t);
    return tam * atoi(aux);
}

void salva_no(char*nome_arq, TARVBM_NI* no, int t){
    // Se nome_arq for "temp", abre esse arquivo. Caso contrário, abre "indice.bin".
    FILE *fp;
    if (strcmp(nome_arq, "temp.bin") == 0) {
        fp = fopen("temp.bin", "rb+");
    } else {
        fp = fopen("indice.bin", "rb+");
    }
    if(!fp){
        printf("Erro ao abrir o arquivo de índice em salvo nó");
        exit(1);
    }
    if (!no->id_no) {
        printf("Erro: id_no nulo em salva_no.\n");
        exit(1);
    }

    int offset = calc_offset_ni(no->id_no, t);
    fseek(fp, offset, SEEK_SET);

    fwrite(no->id_no, sizeof(char), TAM_ID, fp);

    fwrite(&no->nchaves, sizeof(int), 1, fp);

    for (int i = 0; i < (2 * t - 1); i++) {
        fwrite(no->chaves[i], sizeof(char), TAM_NOME_NO, fp);
    }

    for (int i = 0; i < (2 * t); i++) {
        fwrite(no->filhos[i], sizeof(char), TAM_NO, fp);
    }

    fclose(fp);
}

void libera_no(TARVBM_NI* no, int t){

    for(int i=0; i<(2*t)-1; i++) free(no->chaves[i]);
    for(int i=0; i<(2*t); i++) free(no->filhos[i]);
    free(no->chaves);
    free(no->filhos);
    free(no);
}

//REVISADA - OK
void TARVBM_F_cria(){ 
    //cria o nome do arquivo
    char id_folha[TAM_NO];
    sprintf(id_folha, "F%03d.bin", cont_folhas);
    cont_folhas++;

    //cria o arquivo em si
    FILE *fp = fopen(id_folha, "wb");
    if(!fp) exit(1);

    //informações "cabeçalho" da folha
    TARVBM_F* novo = (TARVBM_F*) calloc(1,sizeof(TARVBM_F));


    //cria a folha já com o cabeçalho, só esperando os atletas que ficarão guardados nela
    fwrite(novo, sizeof(TARVBM_F), 1, fp);
    fclose(fp);
    free(novo);
}

//REVISADA - OK
void TARVBM_NI_cria(char* arq_indice, int t, int tam_NI){
    //cria o nome do arquivo de nó interno
    char id_ni[TAM_NO];
    sprintf(id_ni, "N%3d", cont_ni);
    cont_ni++;

    //informações (zeradas/vazias) desse novo nó
    TARVBM_NI *novo_ni = (TARVBM_NI*) calloc(1, sizeof(TARVBM_NI));
    strcpy(novo_ni->id_no, id_ni);
    novo_ni->chaves = (char**) calloc((2 * t) - 1, sizeof(char*));
    for (int i = 0; i < (2 * t) - 1; i++) {
        novo_ni->chaves[i] = (char*) calloc(TAM_NOME_NO, sizeof(char));
    }

    novo_ni->filhos = (char**) calloc ((2*t), sizeof(char*));
    for(int i = 0; i < (2*t); i++){
        novo_ni->filhos[i] = (char*) calloc(TAM_NO, sizeof(char));
    }
    

    strcpy(novo_ni->id_no, id_ni);
    for(int i=0; i<(2*t)-1; i++) strcpy(novo_ni->chaves[i], "");
    for(int i=0; i<(2*t); i++) strcpy(novo_ni->filhos[i], "");
    novo_ni->nchaves=0;

    //vai no final do arquivo de índice e salva esse novo nó
    //em teoria não pode dar problema pq estamos fazendo de forma sequencial 
    salva_no("indice.bin", novo_ni, t);
    libera_no(novo_ni, t);
}

TARVBM_NI* TARVBM_NI_aux_cria(char* arq_indice, char* nome_no, int t) {
    FILE* fp = fopen(arq_indice, "rb");
    if (!fp) {
        perror("Erro ao abrir arquivo de índice");
        exit(1);
    }

    long offset = calc_offset_ni(nome_no, t);
    fseek(fp, offset, SEEK_SET);

    TARVBM_NI* no = (TARVBM_NI*) calloc(1, sizeof(TARVBM_NI));
    fread(no->id_no, sizeof(char), TAM_ID, fp);
    fread(&no->nchaves, sizeof(int), 1, fp);

    no->chaves = malloc(sizeof(char*) * (2 * t - 1));
    for (int i = 0; i < (2 * t - 1); i++) {
        no->chaves[i] = malloc(sizeof(char) * TAM_NOME_NO);
        fread(no->chaves[i], sizeof(char), TAM_NOME_NO, fp);
    }

    no->filhos = malloc(sizeof(char*) * (2 * t));
    for (int i = 0; i < (2 * t); i++) {
        no->filhos[i] = malloc(sizeof(char) * TAM_NO);
        fread(no->filhos[i], sizeof(char), TAM_NO, fp);
    }

    fclose(fp);
    return no;
}

//REVISADA - OK
void imprimir_folha(char* nome_no){
    FILE* fp = fopen(nome_no, "rb");
    if(!fp) printf("Erro ao abrir a folha %s na imprimir_folha", nome_no);
    TARVBM_F folha = {0};
    fread(&folha, sizeof(TARVBM_F), 1, fp);
    printf("\n-----------------------------------------------------");
    printf("\nFolha: %s", nome_no);
    printf("\nQuantidade de chaves: %d", folha.nchaves);
    printf("\nPonteiro prox: %s", folha.prox);

    int qntd_chaves = folha.nchaves;

    infos_atleta tenista = {0};
    while(qntd_chaves){
        fread(&tenista, sizeof(infos_atleta), 1, fp);
        printf("\n----------------------------------------------------------------- ");
        printf("\nID: %s", tenista.id);
        printf("\nNome: %s", tenista.nome);
        printf("\n Nascionalidade: %s", tenista.nasc);
        printf("\nIdade: %d", tenista.idade);
        printf("\nMelhor Rank: %d", tenista.rank);
        printf("\n Ano melhor rank: %d", tenista.anoMelRank);
        if(tenista.morte) printf("Morto");
        else printf("Vivo");
        printf("\nNúmero de vezes que ganhou cada Roland Garros ------------------- ");
        printf("\t\nAustralia open: %d", tenista.ao);
        printf("\t\nFrench Open: %d", tenista.fo);
        printf("\t\nWimbledom: %d", tenista.wib);
        printf("\t\nUs Open: %d", tenista.uso);
        printf("\nNúmero de vezes que ganhou cada ATP1000 ------------------------- ");
        printf("\t\nIndiam Wells: %d", tenista.iw);
        printf("\t\nMiami: %d", tenista.mi);
        printf("\t\nMonte Carlo: %d", tenista.mc);
        printf("\t\nMadrid: %d", tenista.md);
        printf("\t\nRome: %d", tenista.rm);
        printf("\t\nCanada: %d", tenista.ca);
        printf("\t\nCincinnati: %d", tenista.ci);
        printf("\t\nShangai: %d", tenista.sh);
        printf("\t\nParis: %d", tenista.pa);
        printf("\nNúmero de vezes que ganhou as competições sem pontuação --------- ");
        printf("\t\nATP Finals: %d", tenista.atpF);
        printf("\t\nOlympic Games: %d", tenista.olim);
        printf("\n----------------------------------------------------------------\n");
        qntd_chaves--;
    }

    fclose(fp);
}
//REVISADA - OK
void TARVBM_divide(char* arq_indice, char* nome_pai, char* nome_filho, int pos_filho, int t, int tam_NI) {
    char nome_novo[TAM_NO];

    // DVISÃO FOLHA
    if (nome_filho[0] == 'F') {
        // Cria nova folha e define nome
        sprintf(nome_novo, "F%03d.bin", cont_folhas);
        TARVBM_F_cria();  // incrementa cont_folhas lá dentro 

        //se y é folha z tbm vai ser
        FILE *fp_filho = fopen(nome_filho, "rb+");
        if(!fp_filho) exit(1);
        FILE *fp_novo = fopen(nome_novo, "rb+");
        if (!fp_novo) exit(1);

        //carrega o cabeçalho de ambos para a MP
        TARVBM_F folha_filho = {0}, folha_nova = {0};
        fread(&folha_filho, sizeof(TARVBM_F), 1, fp_filho);
        fread(&folha_nova, sizeof(TARVBM_F), 1, fp_novo);

        
        //Atualiza a quantidade de chaves
        folha_filho.nchaves = t - 1;
        folha_nova.nchaves = t; 

        // Passa as t últimas chaves para folha_novo (que agora é o nó da direita)
       fseek(fp_filho, sizeof(TARVBM_F) + sizeof(infos_atleta)*(t - 1), SEEK_SET);
        for (int j = 0; j < t; j++) {
            infos_atleta temp = {0};
            fread(&temp, sizeof(infos_atleta), 1, fp_filho);
            fwrite(&temp, sizeof(infos_atleta), 1, fp_novo); //o ponteiro já está dps do cabeçalho por conta do fread
        }
        //aqui poderia ter apagado os dados de folha_filho, mas como atualizo o número de chaves para de acessa-los e eventualmente serão
        //reescritos, então apagar seria adicionar trabalho e complexidade desnecessárias, mas podemos ver com a Ro7 terça (na MP ela não apagou tbm)
        
        // Atualiza ponteiros
        strcpy(folha_nova.prox, folha_filho.prox);
        strcpy(folha_filho.prox, nome_novo);

        // Regrava cabeçalhos
        fseek(fp_filho, 0, SEEK_SET); 
        fwrite(&folha_filho, sizeof(TARVBM_F), 1, fp_filho);
        fseek(fp_novo, 0, SEEK_SET);  
        fwrite(&folha_nova, sizeof(TARVBM_F), 1, fp_novo);
        fclose(fp_filho);
        fclose(fp_novo);

        // Subindo a menor chave da nova folha (pq é menor que todo o resto a direita e maior que todas a esquerda)
        //a referência ao pai está no arquivo de índice só deu ruim na divisão da 1° folha e da raiz (aí separei)

        //mesmo processo de sempre de carregar o struct de NI para a MP
        TARVBM_NI* pai = TARVBM_NI_aux_cria(arq_indice, nome_pai, t);

        //abre de novo o arquivo da folha criada para carregar a primeira chave
        infos_atleta promovido = {0};
        FILE* fp_le_novo = fopen(nome_novo, "rb");
        fseek(fp_le_novo, sizeof(TARVBM_F), SEEK_SET);
        fread(&promovido, sizeof(infos_atleta), 1, fp_le_novo);
        fclose(fp_le_novo);

        //Abrindo espaço e reorganizando o pai para a entrada da nova chave
        for (int j = pai->nchaves; j > pos_filho; j--) {
            strcpy(pai->chaves[j], pai->chaves[j - 1]);
            strcpy(pai->filhos[j + 1], pai->filhos[j]);
        }

        //Promove só o id (pq é NI) e coloca o novo_no como o filho mais a direita na chave promovida
        strcpy(pai->chaves[pos_filho], promovido.id);
        strcpy(pai->filhos[pos_filho + 1], nome_novo);
        pai->nchaves++;

        salva_no( "indice.bin", pai,t);
        libera_no(pai,t);
    }

    // DIVISÃO DE NÓ INTERNO
    else if (nome_filho[0] == 'N') {
        //cria um novo nó interno
        sprintf(nome_novo, "N%03d", cont_ni);
        TARVBM_NI_cria(arq_indice, t, tam_NI);  // incrementa cont_ni

        //Abre o arquivo de índice, calcula o offsets de x,y,z (pai, filho, novo) e carrega os três pra MP
        FILE *fp = fopen(arq_indice, "rb+");
        if (!fp) exit(1);

        long offset_filho = calc_offset_ni(nome_filho, t);
        long offset_pai = calc_offset_ni(nome_pai, t);
        long offset_novo = calc_offset_ni(nome_novo, t);

        TARVBM_NI* filho = TARVBM_NI_aux_cria(arq_indice, nome_filho, t);
        TARVBM_NI* novo = TARVBM_NI_aux_cria(arq_indice, nome_novo, t);
        TARVBM_NI* pai = TARVBM_NI_aux_cria(arq_indice, nome_pai, t);

        //Passa as chaves e filhos necessárias, aqui ela apaga os filhos que saiam de y, mas mantive a lógica de só mudar nchaves e permitir sobreescritas
        novo->nchaves = t - 1;
        for (int j = 0; j < t-1; j++) strcpy(novo->chaves[j], filho->chaves[j + t]);
        for (int j = 0; j < t; j++) strcpy(novo->filhos[j], filho->filhos[j + t]);
        for(int j=t; j<2*t; j++) strcpy(novo->filhos[j], "");

        filho->nchaves = t - 1;

        for (int j = pai->nchaves; j > pos_filho; j--) {
            strcpy(pai->chaves[j], pai->chaves[j - 1]);
            strcpy(pai->filhos[j + 1], pai->filhos[j]);
        }

        strcpy(pai->chaves[pos_filho], filho->chaves[t - 1]);
        strcpy(pai->filhos[pos_filho + 1], nome_novo);
        pai->nchaves++;

        //Volta com os structs atualizados para a mesma posição que estavam
        fseek(fp, offset_filho, SEEK_SET); 
        salva_no("indice.bin", filho,t);
        fseek(fp, offset_novo, SEEK_SET); 
        salva_no("indice.bin", novo,t);
        fseek(fp, offset_pai, SEEK_SET); 
        salva_no("indice.bin", pai,t);
        libera_no(filho,t);
        libera_no(pai,t);
        libera_no(novo,t);
        fclose(fp);
    }
}

//REVISADA - OK
void divide_primeira_folha(char* arq_indice, int t, int tam_NI) {
    FILE *fp_f0 = fopen("F000.bin", "rb");
    if (!fp_f0) exit(1);

    TARVBM_F folha = {0};
    fread(&folha, sizeof(TARVBM_F), 1, fp_f0);

    //carrego completamente a primeira folha na MP (são só 5 chaves acho que não tem problema)
    infos_atleta atletas[(2*t)-1];
    for (int i = 0; i < (2*t)-1; i++)
        fread(&atletas[i], sizeof(infos_atleta), 1, fp_f0);
    fclose(fp_f0);

    // Crio e F001 e abro as duas (mas abro F000 com wb+ pra ela vir vazia)
    TARVBM_F_cria(cont_folhas);
    FILE* fp_f0_novo = fopen("F000.bin", "wb+");
    FILE* fp_f1 = fopen("F001.bin", "rb+");

    //Montando F000
    //Como em qualquer divisão em folhas a "folha_filho" tem t-1 elementos e aponta para a nova folha criada
    folha.nchaves = t - 1;
    strcpy(folha.prox, "F001.bin");

    //escrevo o novo cabeçalho e copio os primeiros t-1 atletas para F000
    fwrite(&folha, sizeof(TARVBM_F), 1, fp_f0_novo);
    for (int i = 0; i < t - 1; i++)
        fwrite(&atletas[i], sizeof(infos_atleta), 1, fp_f0_novo);
    fclose(fp_f0_novo);

    // MONTANDO F001
    //atualizo o struct cabeçalho 
    TARVBM_F nova_folha = {0};
    nova_folha.nchaves = t;
    strcpy(nova_folha.prox, "");

    //escrevo o novo cabeçalho e copio os últimos t-1 atletas
    fwrite(&nova_folha, sizeof(TARVBM_F), 1, fp_f1);
    for (int i = t-1; i < (2*t)-1; i++)
        fwrite(&atletas[i], sizeof(infos_atleta), 1, fp_f1);
    fclose(fp_f1);

    // Como temos duas folhas precisamos de uma raiz e com isso criar o índice
    FILE* fp_indice = fopen(arq_indice, "wb+");
    if(!fp_indice){
        printf("Erro ao criar o índice");
        exit(1);
    }
    fclose(fp_indice);

    TARVBM_NI_cria(arq_indice, t, tam_NI);
    TARVBM_NI* raiz = TARVBM_NI_aux_cria(arq_indice, "N000", t);
    raiz->nchaves=1; 
    strcpy(raiz->id_no, "N000");
    strcpy(raiz->chaves[0], atletas[t - 1].id); //aproveita que já tá na MP para n precisar reabrir F001
    strcpy(raiz->filhos[0], "F000.bin"); //atualiza os seus dois filhos manualmente tbm
    strcpy(raiz->filhos[1], "F001.bin");
    
    //pro struct ir completo e não destruir nossa lógica de acesso por ofsset copia nada para o resto 
    for (int i = 1; i < (2*t)-1; i++) strcpy(raiz->chaves[i], "");
    for (int i = 2; i < (2*t); i++) strcpy(raiz->filhos[i], "");

    //escreve o novo NI/raiz no índice
    salva_no("indice.bin", raiz,t);
    libera_no(raiz,t);
}

//ok
void insere_incompleto(char* arq_indice, char* nome_no, infos_atleta* jogador, int t, int tam_NI) {
    // Caso folha 
    if (nome_no[0] == 'F') {

        //Abre a folha onde está a chave e um arquivo temporário que vai receber as infos atualizadas
        FILE *fp = fopen(nome_no, "rb+");
        if (!fp) exit(1);

        FILE *temp = fopen("temp.bin", "wb+");
        if (!temp) exit(1);

        //Carrega o cabeçalho da folha original aumenta mais uma chave (pq estamos fznd uma inserção) e já copia pra temp
        TARVBM_F folha = {0};
        fread(&folha, sizeof(TARVBM_F), 1, fp);
        folha.nchaves +=1;
        fwrite(&folha, sizeof(TARVBM_F), 1, temp);

        //controle
        int inserido = 0;
        infos_atleta lendo = {0};

        //lê todos os atletas que já estão na folha
        for (int i = 0; i < folha.nchaves - 1; i++) {
            fread(&lendo, sizeof(infos_atleta), 1, fp);

            //Se ainda não inseriu o jogador e ele é menor que o atleta que estamos lendo agr, salvamos ele em temp e só dps o atual
            //se não for menor pula esse if e só copia o atual
            if (!inserido && strcmp(jogador->id, lendo.id) < 0) {
                fwrite(jogador, sizeof(infos_atleta), 1, temp);
                inserido = 1;
            }

            fwrite(&lendo, sizeof(infos_atleta), 1, temp);
        }

        //se sair do for e não inserir é pq ele é maior que todos, aí insere no final
        if (!inserido)
            fwrite(jogador, sizeof(infos_atleta), 1, temp);

        fclose(fp);
        fclose(temp);

        remove(nome_no);
        if (rename("temp.bin", nome_no) != 0) {
            printf("Erro ao renomear arquivo temporário para %s", nome_no);
            exit(1);
        } 

    }

    // Caso nó interno
    else if (nome_no[0] == 'N') {
        //Abre o arquivo de índice e carrega para MP o nó em que estamos
        TARVBM_NI* no = TARVBM_NI_aux_cria(arq_indice, nome_no, t);

        //descobre o filho pra onde descer e vê se é folha ou NI
        int i = 0;
        while (i < no->nchaves && strcmp(jogador->id, no->chaves[i]) > 0) i++;
        char filho_descida[TAM_NO];
        strcpy(filho_descida, no->filhos[i]);

        char tipo = filho_descida[0];

        // Verifica se filho precisa ser dividido
        if (tipo == 'N') {
            TARVBM_NI* no_descida = TARVBM_NI_aux_cria(arq_indice, filho_descida, t);
            if (no_descida->nchaves == (2*t)-1) {
                TARVBM_divide(arq_indice, nome_no, filho_descida, i, t, tam_NI);
                libera_no(no,t);
                no = TARVBM_NI_aux_cria(arq_indice, nome_no, t);
                if (strcmp(jogador->id, no->chaves[i]) > 0) i++; //vê se vamos pro filho antigo ou atualizamos com a divisão
            }
            libera_no(no_descida, t);
        } else {
            //descendo para uma folha  
            FILE* fp_folha = fopen(filho_descida, "rb");
            if(!fp_folha){
                printf("Error ao abrir o filho %s", filho_descida);
                exit(1);
            }
            TARVBM_F f = {0};
            fread(&f, sizeof(TARVBM_F), 1, fp_folha);
            fclose(fp_folha);
            if (f.nchaves == (2*t)-1) {
                TARVBM_divide(arq_indice, nome_no, filho_descida, i,  t, tam_NI);
                libera_no(no, t);
                no = TARVBM_NI_aux_cria(arq_indice, nome_no, t);
                if (strcmp(jogador->id, no->chaves[i]) > 0) i++;
            }
        }

        char filho_proximo[TAM_NO];
        strcpy(filho_proximo, no->filhos[i]);
        libera_no(no, t);
        insere_incompleto(arq_indice, filho_proximo, jogador, t, tam_NI);
    }
}

//REVISADA - CREIO QUE FINALMENTE OK
void TARVBM_insere(char *arq_indice, infos_atleta* jogador, char* raiz_atual, int t, int tam_NI){
    char fantasia[TAM_NOME_NO];
    infos_atleta* busca;
    if(cont_folhas != 0 ){
        if(cont_ni==0) busca = TARVBM_F_busca("F000.bin", jogador->id, fantasia);
        else busca = TARVBM_NI_busca(arq_indice, jogador->id, raiz_atual,t,fantasia, "F000.bin");
        if(busca){
            //Adiciono um print aqui?
            free(busca);
            return;
        }
    }

    //Se ainda não existe nenhuma folha (nem nada)
    if(cont_folhas == 0){ 
        TARVBM_F_cria(); //crio F000.bin
        insere_incompleto(arq_indice, "F000.bin", jogador, t, tam_NI);
        return;
    }

    //se ainda não existe índice (c crtz só tem uma folha)
    if(cont_ni == 0){
        //verifca se a folha está cheia como é uma folha só já sei que é F000.bin
        FILE *fp = fopen("F000.bin", "rb");
        if(!fp) exit(1);

        TARVBM_F prim_folha = {0};
        fread(&prim_folha, sizeof(TARVBM_F), 1, fp);
        fclose(fp);

        //Quando a primeira folha enche tem uma função de divisão específica para ela porque tem que criar o índice e tals
        if(prim_folha.nchaves == (2*t)-1){
            divide_primeira_folha(arq_indice, t, tam_NI);
            insere_incompleto(arq_indice, "N000", jogador, t, tam_NI);
            return;
        }
        //só tem uma folha mas ela ainda tem espaço para elementos
        insere_incompleto(arq_indice, "F000.bin", jogador, t, tam_NI);
        return;
    }

    // Se já temos o arquivo de índice com pelo menos um nó interno
    TARVBM_NI* raiz = TARVBM_NI_aux_cria(arq_indice, raiz_atual, t);

    if (raiz->nchaves == (2*t)-1) {
        TARVBM_NI_cria(arq_indice, t, tam_NI);

        char nome_nova_raiz[TAM_NOME_NO];
        char nome_raiz_antiga[TAM_NOME_NO];
        int aux = cont_ni;
        sprintf(nome_nova_raiz, "N%03d", aux - 1); // -1 pois a função TARVBM_NI já incrementou

        strcpy(nome_raiz_antiga, raiz_atual);
        strcpy(raiz_atual, nome_nova_raiz); 

        // Atualizar a nova raiz com filhos[0] = antiga raiz
        TARVBM_NI* nova_raiz = TARVBM_NI_aux_cria(arq_indice, nome_nova_raiz, t);

        strcpy(nova_raiz->filhos[0], nome_raiz_antiga);

        // Salva a nova raiz
        salva_no("indice.bin", nova_raiz,t);

        // finalmente divide a antiga raiz
        TARVBM_divide(arq_indice, raiz_atual, nome_raiz_antiga, 0, t, tam_NI);
        libera_no(nova_raiz, t);  // libera a versão antiga
        nova_raiz = TARVBM_NI_aux_cria(arq_indice, raiz_atual, t);  // recarrega a nova raiz atualizada

        insere_incompleto(arq_indice, raiz_atual, jogador, t, tam_NI);
        libera_no(nova_raiz,t);
        libera_no(raiz,t);
        return;
    }

    // Se a raiz não está cheia
    libera_no(raiz, t);
    insere_incompleto(arq_indice, raiz_atual, jogador, t, tam_NI);
}

void TARBM_imprime_rec(char* arq_indice, char* raiz, int nivel, int t) {
    if(raiz[0] == 'N'){
        FILE *fp = fopen(arq_indice, "rb");
        if (!fp) {
            if (errno == ENOENT){
                printf("DEBUG: não existe arquivo indice, abrindo F000.bin\n");
                FILE *fp_folha = fopen("F000.bin", "rb+");  // usa outro nome!
                if (!fp_folha) {
                    printf("F000.bin inexistente ou erro ao abrí-lo (TARVBM_imprime)\n");
                    exit(1);
                }
                TARVBM_F cabecalho = {0};
                fread(&cabecalho, sizeof(TARVBM_F), 1, fp_folha);
                printf("nchaves da folha: %d\n", cabecalho.nchaves);
                // Imprime a folha com indentação
                for (int i = 0; i < nivel; i++) printf("\t");
                printf("Folha F000.bin: [");
                for (int i = 0; i < cabecalho.nchaves; i++) {
                    infos_atleta info = {0};
                    memset(&info, 0, sizeof(infos_atleta));
                    fread(&info, sizeof(infos_atleta), 1, fp_folha);
                    printf("%s", info.id);
                    if (i < cabecalho.nchaves - 1) printf(" | ");
                }
                printf("]\n");
                fclose(fp_folha);
                return; // já tratou o caso especial, termina aqui
            }else{
                printf("erro ao abrir %s\n", arq_indice);
                exit(1);
            }
        }
        TARVBM_NI* infos_raiz = TARVBM_NI_aux_cria(arq_indice, raiz, t);
        fclose(fp);

        // Imprime o nó interno com indentação
        for (int i = 0; i < nivel; i++) printf("\t");
        printf("Nó interno %s: [", raiz);
        for (int i = 0; i < infos_raiz->nchaves; i++) {
            printf("%s", infos_raiz->chaves[i]);
            if (i < infos_raiz->nchaves - 1) printf(" | ");
        }
        printf("]\n");

        // Recursivamente imprime os filhos
        for (int i = 0; i <= infos_raiz->nchaves; i++) {
            if (strcmp(infos_raiz->filhos[i], "") != 0) {
                TARBM_imprime_rec(arq_indice, infos_raiz->filhos[i], nivel + 1, t);
            }
        }
        libera_no(infos_raiz,t);
    }else{
        FILE* fp = fopen(raiz, "rb");
        if(!fp) exit(1);

        TARVBM_F cabecalho = {0};
        memset(&cabecalho, 0, sizeof(TARVBM_F));
        fread(&cabecalho, sizeof(TARVBM_F), 1, fp);

        for (int i = 0; i < nivel; i++) printf("\t");
        printf("Folha %s: [", raiz);

        for (int i = 0; i < cabecalho.nchaves; i++) {
            infos_atleta info = {0};
            fread(&info, sizeof(infos_atleta), 1, fp);
            printf("%s", info.id);
            if (i < cabecalho.nchaves - 1) printf(" | ");
        }
        printf("]\n");

        fclose(fp);
    }
}


infos_atleta *TARVBM_F_busca(char *nomeArqFol, char *id, char* copia_nome){
	FILE *fp = fopen(nomeArqFol, "rb");
	if (!fp){
		printf("Erro ao abrir %s (TARVBM_F_busca)", nomeArqFol);
		exit(1);
	}

	infos_atleta* jogador = (infos_atleta*) calloc (1,sizeof(infos_atleta));
	fseek(fp, sizeof(TARVBM_F), SEEK_SET);
	while (fread(jogador, sizeof(infos_atleta), 1, fp)){
		if (!strcmp(jogador->id, id)) {
            fclose(fp);
            strcpy(copia_nome, nomeArqFol);
			return jogador;
		}
	}
	fclose(fp);
    free(jogador);
    return NULL;
}

infos_atleta *TARVBM_NI_busca(char *nomeArqInd, char *id, char *raiz, int t, char* copia_nome, char* prim_folha){
    FILE *fp = fopen(nomeArqInd, "rb");
    infos_atleta *jogador = NULL;
	if(!fp){
        if (errno == ENOENT){
            jogador = TARVBM_F_busca(prim_folha, id, "");
        }
		else printf("Erro ao abrir %s (TARVBM_NI_busca)\n", nomeArqInd);
		return jogador;
	}
    fclose(fp);

	TARVBM_NI* aux = TARVBM_NI_aux_cria(nomeArqInd, raiz, t);
    char nomeArqFol[TAM_NO];
	while(1){
		int i = 0;
		while (i < aux->nchaves && strcmp(aux->chaves[i], id) < 0) i++;
		if (i < aux->nchaves && !strcmp(aux->chaves[i], id)){
            i++;
        }
		if (aux->filhos[i][0] == 'F'){
			strcpy(nomeArqFol, aux->filhos[i]);
			break;
		}
        char filho[TAM_NO];
        strcpy(filho, aux->filhos[i]);
        libera_no(aux,t);
        if(strcmp(filho, "") == 0) break;
        aux = TARVBM_NI_aux_cria(nomeArqInd, filho, t);

	}

    libera_no(aux,t);
    jogador = TARVBM_F_busca(nomeArqFol, id, copia_nome);
	return jogador;
}

void TARVBM_F_atualizarJogador(infos_atleta* jogador, char* raiz_atual, int t){
    char folha[TAM_NO];
    //só rodo para descobrir a folha
    infos_atleta* aux = TARVBM_NI_busca("indice.bin", jogador->id, raiz_atual, t, folha, "F000.bin");
    if(aux) free(aux);
    FILE* fp = fopen(folha, "rb+");
    if(!fp) printf("Erro ao abrir a folha para atualização");

    TARVBM_F cabecalho = {0};
    infos_atleta atleta_antes = {0};
    fread(&cabecalho, sizeof(TARVBM_F), 1, fp);
    int i= cabecalho.nchaves;
    while(i > 0){
        fread(&atleta_antes, sizeof(infos_atleta), 1, fp);
        if(strcmp(atleta_antes.id, jogador->id) == 0){
            atleta_antes.ao = jogador->ao;
            atleta_antes.fo = jogador->fo;
            atleta_antes.wib = jogador->wib;
            atleta_antes.uso = jogador->uso;
            atleta_antes.atpF = jogador->atpF;
            atleta_antes.olim = jogador->olim;
            atleta_antes.iw = jogador->iw;
            atleta_antes.mi = jogador->mi;
            atleta_antes.mc = jogador->mc;
            atleta_antes.md = jogador->md;
            atleta_antes.rm = jogador->rm;
            atleta_antes.ca = jogador->ca;
            atleta_antes.ci = jogador->ci;
            atleta_antes.sh = jogador->sh;
            atleta_antes.pa = jogador->pa;
            int pos = ftell(fp) - sizeof(infos_atleta);
            fseek(fp, pos, SEEK_SET);
            fwrite(&atleta_antes, sizeof(infos_atleta), 1, fp);
            break;
        }
        i--;
    }
    fclose(fp);
}

/****************************************************************************************************

                         obs: Isso não é um galho -  HABEMUS ÁRVORE!!!!!!!!

*****************************************************************************************************/

void decrementar_id_no(char *id_no){
    char s = id_no[0];
    strcpy(id_no, id_no + 1);
    int n = atoi(id_no);
    n -= 1;
    if (n < 1) {
        printf("ERRO: ID ficaria inválido ao decrementar (decrementar_id_no): %c%03d\n", s, n);
        exit(1);
    }
    sprintf(id_no, "%c%0*d", s, 3, n);
}

void compactar_posicao(char *nomeArqInd, long posVazia, long raizOffset, char *raiz_atual, int *posPai, int t){
    printf("compactar_posicao() iniciou, Vazio: %ld\n", posVazia/sizeof(TARVBM_NI));
    FILE *fp = fopen(nomeArqInd, "rb+");
    if (!fp){
        printf("Erro ao abrir %s (compactar_posicao)\n", nomeArqInd);
        exit(1);
    }

    FILE *fc = fopen("temp.bin", "wb+");
    if (!fc){
        printf("Erro ao criar temp.bin\n");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    long posFinal = ftell(fp);

    int cont = posVazia/tam_bloco(t);
    char aux[TAM_NOME_NO];
    sprintf(aux, "N%03d", cont);
    fclose(fp);
    TARVBM_NI *no = TARVBM_NI_aux_cria(nomeArqInd, aux, t);
    printf("\nid_no: %s, filho[0]: %s, chave[0]: %s\n\n", no->id_no, no->filhos[0], no->chaves[0]);
    libera_no(no, t);

    cont = 0;
    sprintf(aux, "N%03d", cont);
    no = TARVBM_NI_aux_cria(nomeArqInd, aux, t);
    
    long pos = 0;
    fclose(fc);
    while (no){
        printf("\nDEBUG: Lendo: %s\n", no->id_no);
        if (pos == posVazia) {
            // pula a posição vazia
            pos += tam_bloco(t);
            cont++;
            libera_no(no, t);
            if(cont == cont_ni) break;
            sprintf(aux, "N%03d", cont);
            no = TARVBM_NI_aux_cria(nomeArqInd, aux, t);
            continue;
        }
        
        if (pos == posFinal) break;
        // pula o último nó, que ficou duplicado após compactação

        if (pos > posVazia){
            printf("id antes de decrementar - %s\n", no->id_no);
            decrementar_id_no(no->id_no);
            printf("id depois de decrementar - %s\n", no->id_no);
            if (pos == raizOffset){
                printf("mudou o id da raiz\n");
                strcpy(raiz_atual, no->id_no);
            }
            if (pos/tam_bloco(t) == (*posPai)) (*posPai)--;
        }

        for (int i = 0; i <= no->nchaves; i++){
            printf("Verificando se o filho[%d]: %s do no %s vem depois da posVazia (%ld)\n", i, no->filhos[i], no->id_no, posVazia/tam_bloco(t));
            if (no->filhos[i][0] == 'N'){
                printf("E no interno\n");
                if (calc_offset_ni(no->filhos[i], t) > posVazia) {
                    printf("%s vem depois de posVazia\n", no->filhos[i]);  
                    decrementar_id_no(no->filhos[i]);
                    printf("Filho[%d] depois de decrementar: %s\n", i, no->filhos[i]);
                }
            }
        }

        printf("\nEscrevendo no %s no novo arquivo\n", no->id_no);
        salva_no("temp.bin", no, t);
        
        pos += tam_bloco(t);
        cont++;
        libera_no(no, t);
        if(cont == cont_ni) break;
        sprintf(aux, "N%03d", cont);
        no = TARVBM_NI_aux_cria(nomeArqInd, aux, t);
    }
    remove(nomeArqInd);
    rename("temp.bin", nomeArqInd);
    cont_ni--;
}

void TARVBM_remove(char *nomeArqInd, char *id, char *raiz_atual, int t, char *primFolha){
    // COLOCAR BUSCA AQUI
    FILE *fpi = fopen(nomeArqInd, "rb+");
    if (!fpi){ // ARQUIVO INDICE AINDA NÃO FOI CRIADO OU DEU ERRO AO ABRIR;
        if (errno == ENOENT){
            printf("DEBUG: Verificando existência do atleta com id='%s' em F000.bin\n", id);
            FILE *fpf = fopen(primFolha, "rb+"); // REVISAR F000 PODE NÃO SER A FOLHA A SOBRAR
            if (!fpf){
                printf("%s inexistente ou erro ao abrí-lo (TARVBM_F_remove)", primFolha);
                exit(1);
            }
            // NÃO EXISTE INDICE AINDA, OU SEJA, A ÁRVORE ESTÁ VAZIA OU A RAIZ É NÓ FOLHA
            TARVBM_F aux;
            fread(&aux, sizeof(TARVBM_F), 1, fpf); // LÊ O NÚMERO DE CHAVES DA FOLHA
            printf("DEBUG: %s possui %d chaves antes da remoção\n", primFolha, aux.nchaves);

            FILE *fc = fopen("temp.bin", "wb+"); // CRIANDO O ARQUIVO ONDE OS ELEMENTOS DO ARQUIVO ORIGINAL SERÃO REESCRITOS
            if (!fc){
                printf("Falha em criar temp.bin (TARVBM_F_remove dentro de if (!fpi))");
                exit(1);
            }
            aux.nchaves--; // ATUALIZA O NÚMERO DE CHAVES ANTES DA REESCRITA
            fwrite(&aux, sizeof(TARVBM_F), 1, fc); // ESCREVE O INÍCIO DO ARQUIVO COM O NÚMERO DE CHAVES DENTRO DA FOLHA

            long posAux = sizeof(TARVBM_F); // JA SOMA O INICIO DO ARQUIVO
            infos_atleta atleta;
            while (fread(&atleta, sizeof(infos_atleta), 1, fpf)){ // COPIA OS ELEMENTOS DO ARQUIVO ORIGINAL PARA O NOVO (EXCETO O QUE DEVE SER REMOVIDO)
                printf("DEBUG: Lendo atleta → nome='%s', id='%s' (comparando com '%s')\n", atleta.nome, atleta.id, id);
                if (strcmp(atleta.id, id) != 0){
                    printf("DEBUG: Mantendo atleta com id='%s'\n", atleta.id);
                    fwrite(&atleta, sizeof(infos_atleta), 1, fc);
                }
                posAux += sizeof(infos_atleta);
            }
            fclose(fpf);
            fclose(fc);
            remove(primFolha);
            printf("DEBUG: Substituindo F000.bin pelo novo arquivo com atleta removido\n");
            rename("temp.bin", primFolha);
            return;
        }
        else{
            printf("Erro ao abrir %s (TARVBM_F_remove)", nomeArqInd);
            exit(1);
        }
    }

    int i;
    TARVBM_NI *aux;

    fclose(fpi);
    aux = TARVBM_NI_aux_cria(nomeArqInd, raiz_atual, t);
    fpi = fopen(nomeArqInd, "rb+");
    if(!fpi){
        printf("Erro ao abrir %s (TARVBM_remove)", nomeArqInd);
        exit(1);    
    }

    char noPai[TAM_NO];
    char noFilho0[TAM_NO];
    char noFilho1[TAM_NO];
    char noFilho2[TAM_NO];
    long raizOffset = calc_offset_ni(raiz_atual, t);

    while(aux){
      

        for(i = 0; i < aux->nchaves && strcmp(aux->chaves[i], id) < 0; i++);
        if ((i < aux->nchaves) && !strcmp(aux->chaves[i], id)) i++;
        printf("DEBUG: Buscando id='%s', caiu na posição i=%d\n", id, i);
        if (i > aux->nchaves) {
            printf("DEBUG: Chave não está entre as chaves do nó\n");
            return;
        }
        if(i == 0) strcpy(noFilho0, "");
        else strcpy(noFilho0, aux->filhos[i-1]);
        strcpy(noFilho1, aux->filhos[i]);
        if(i == aux->nchaves) strcpy(noFilho2, "");
        else strcpy(noFilho2, aux->filhos[i+1]);
        
        strcpy(noPai, aux->id_no);
  

        FILE *fpf, *fpf2;
        TARVBM_NI *atletaFilho0 = NULL, *atletaFilho1 = NULL, *atletaFilho2 = NULL;
        TARVBM_F cabecalho = {0}, cabecalhoFolha0 = {0}, cabecalhoFolha1 = {0}, cabecalhoFolha2 = {0};
        long pos0, pos1, pos2, posPai;
        int k;
        char nomeArqFol0[9], nomeArqFol1[9], nomeArqFol2[9];

        posPai = calc_offset_ni(noPai, t);
        if (noFilho0[0] == 'N' || noFilho1[0] == 'N' || noFilho2[0] == 'N'){

            pos0 = calc_offset_ni(noFilho0, t);
            pos1 = calc_offset_ni(noFilho1, t);
            pos2 = calc_offset_ni(noFilho2, t);

            if (pos0 != -1){
                fclose(fpi);
                atletaFilho0 = TARVBM_NI_aux_cria(nomeArqInd, noFilho0, t);
                fpi = fopen(nomeArqInd, "rb+");
                if(!fpi){
                    printf("Erro ao reabrir %s (TARVBM_remove)", nomeArqInd);
                    exit(1);
                }
            }

            fclose(fpi);
            atletaFilho1 = TARVBM_NI_aux_cria(nomeArqInd, noFilho1, t);
            fpi = fopen(nomeArqInd, "rb+");
            if(!fpi){
                printf("Erro ao reabrir %s (TARVBM_remove)", nomeArqInd);
                exit(1);
            }

            if(pos2 != -1){
                fclose(fpi);
                atletaFilho2 = TARVBM_NI_aux_cria(nomeArqInd, noFilho2, t);
                fpi = fopen(nomeArqInd, "rb+");
                if(!fpi){
                    printf("Erro ao reabrir %s (TARVBM_remove)", nomeArqInd);
                    exit(1);
                }
            }
            if (atletaFilho0) printf("atletaFilho0.id_no='%s' - atletaFilho0.nchaves=%d\n", atletaFilho0->id_no, atletaFilho0->nchaves);
            if (atletaFilho1) printf("atletaFilho1.id_no='%s' - atletaFilho1.nchaves=%d\n", atletaFilho1->id_no, atletaFilho1->nchaves);
            if (atletaFilho2) printf("atletaFilho2.id_no='%s' - atletaFilho2.nchaves=%d\n", atletaFilho2->id_no, atletaFilho2->nchaves);
        }
        else{
            strcpy(nomeArqFol0, noFilho0);
            strcpy(nomeArqFol1, noFilho1);
            strcpy(nomeArqFol2, noFilho2);

            printf("DEBUG: Abrindo folhas: %s, %s, %s\n", nomeArqFol0, nomeArqFol1, nomeArqFol2);
            
            if (strlen(nomeArqFol0) > 0) {
                fpf = fopen(nomeArqFol0, "rb");
                if (!fpf) {
                    printf("Erro ao abrir '%s' (nomeArqFol0)\n", nomeArqFol0);
                    exit(1);
                }
                fread(&cabecalhoFolha0, sizeof(TARVBM_F), 1, fpf);
                fclose(fpf);
            } else {
                printf("DEBUG: nomeArqFol0 está vazio, ignorando leitura do primeiro filho\n");
            }
            
            fpf = fopen(nomeArqFol1, "rb");
            if(!fpf){
                printf("Erro ao abrir a folha %s (TARVBM_remove)\n", nomeArqFol1);
                exit(1);
            }
            fread(&cabecalhoFolha1, sizeof(TARVBM_F), 1, fpf);
            fclose(fpf);

            if (strlen(nomeArqFol2) > 0) {
                fpf = fopen(nomeArqFol2, "rb");
                if (!fpf) {
                    printf("Erro ao abrir '%s' (nomeArqFol2)\n", nomeArqFol2);
                    exit(1);
                }
                fread(&cabecalhoFolha2, sizeof(TARVBM_F), 1, fpf);
                fclose(fpf);
            } else {
                printf("DEBUG: nomeArqFol2 está vazio, ignorando leitura do terceiro filho\n");
            }
            
            printf("DEBUG: Cabeçalhos → Folha0.nchaves=%d | Folha1.nchaves=%d | Folha2.nchaves=%d\n", cabecalhoFolha0.nchaves, cabecalhoFolha1.nchaves, cabecalhoFolha2.nchaves);
        }

        if (cabecalhoFolha1.nchaves == t - 1 || ((atletaFilho1) && (atletaFilho1->nchaves == t - 1))){ // CASOS 3A E 3B
            if ((i < aux->nchaves) && ((atletaFilho2 && (atletaFilho2->nchaves >= t)) || (cabecalhoFolha2.nchaves >= t))){ // CASO 3A
                printf("DEBUG: CASO 3A ativado: filho[%d] está com nchaves = t-1 e direito >= t\n", i);
                if (noFilho1[0] == 'F') {
                    fpf = fopen(nomeArqFol2, "rb+");
                    if(!fpf){
                        printf("Erro ao abrir %s (CASO 3A TARVBM_F_remove)", nomeArqFol2);
                        exit(1);
                    }
                    printf("DEBUG: Abrindo %s para redistribuição\n", nomeArqFol2);
                    printf("DEBUG: Antes da redistribuição → cabecalho.nchaves (folha2) = %d\n", cabecalhoFolha2.nchaves);
                    
                    fread(&cabecalho, sizeof(TARVBM_F), 1, fpf);

                    FILE *fc = fopen("temp.bin", "wb+");
                    if (!fc){
                        printf("Erro ao criar temp.bin (CASO 3A)");
                        exit(1);
                    }

                    cabecalho.nchaves--;
                    fwrite(&cabecalho, sizeof(TARVBM_F), 1, fc);

                    infos_atleta atletaTransferir, atleta, atletaSubir;
                    fread(&atletaTransferir, sizeof(infos_atleta), 1, fpf);
                    long posIgnorar = ftell(fpf) - sizeof(infos_atleta);
                    fread(&atletaSubir, sizeof(infos_atleta), 1, fpf);
                    printf("Transferido pro pai: %s", atletaSubir.id);
                    printf("DEBUG: Atleta transferido → id='%s', nome='%s'\n", atletaTransferir.id, atletaTransferir.nome);

                    strcpy(aux->chaves[i], atletaSubir.id);
                    fclose(fpi);
                    salva_no(nomeArqInd, aux, t);
                    fpi = fopen(nomeArqInd, "rb+");
                    if (!fpi){
                        printf("Erro ao abrir %s (TARVBM_remove CASO 3A NORMAL F)", nomeArqInd);
                        exit(1);
                    }
                    printf("DEBUG: Atualizando chave[%d] no nó pai para '%s'\n", i, aux->chaves[i]);

                    fseek(fpf, sizeof(TARVBM_F), SEEK_SET);
                    long posAux = sizeof(TARVBM_F);
                    while(fread(&atleta, sizeof(infos_atleta), 1, fpf)){
                        if (posAux != posIgnorar){
                            printf("Mantendo: %s\n", atleta.id);
                            fwrite(&atleta, sizeof(infos_atleta), 1, fc);
                        }
                        else printf("Removendo: %s\n", atleta.id);
                        posAux += sizeof(infos_atleta);
                    }
                    fclose(fpf);
                    fclose(fc);
                    remove(nomeArqFol2);
                    printf("DEBUG: Removido atleta da frente de %s, novo nchaves=%d\n", nomeArqFol2, cabecalho.nchaves);
                    rename("temp.bin", nomeArqFol2);

                    fpf = fopen(nomeArqFol1, "rb+");
                    if (!fpf){
                        printf("Erro ao abrir arquivo %s (CASO 3A TARVBM_F_remove)", nomeArqFol1);
                        exit(1);
                    }

                    fread(&cabecalho, sizeof(TARVBM_F), 1, fpf);
                    cabecalho.nchaves++;
                    printf("DEBUG: Adicionando atleta transferido ao final de %s → novo nchaves=%d\n", nomeArqFol1, cabecalho.nchaves);
                    fseek(fpf, 0L, SEEK_SET);
                    fwrite(&cabecalho, sizeof(TARVBM_F), 1, fpf);

                    fseek(fpf, 0L, SEEK_END);
                    fwrite(&atletaTransferir, sizeof(infos_atleta), 1, fpf);
                    printf("DEBUG: Atleta '%s' inserido ao final da folha1\n", atletaTransferir.id);
                    fclose(fpf);

                    break;
                }
                else{
                    printf("DEBUG: Redistribuição entre nós internos (i = %d)\n", i);
                    printf("DEBUG: atletaFilho1.nchaves antes = %d | atletaFilho2.nchaves antes = %d\n", atletaFilho1->nchaves, atletaFilho2->nchaves);

                    strcpy(atletaFilho1->chaves[t-1], aux->chaves[i]);
                    strcpy(aux->chaves[i], atletaFilho2->chaves[0]);
                    printf("DEBUG: Movendo chave '%s' do pai para atletaFilho1.chaves[%d]\n", aux->chaves[i], t-1);
                    printf("DEBUG: Substituindo chave do pai por '%s' (de atletaFilho2.chaves[0])\n", atletaFilho2->chaves[0]);

                    int j;
                    atletaFilho1->nchaves++;
                    for (j = 0; j < atletaFilho2->nchaves - 1; j++) strcpy(atletaFilho2->chaves[j], atletaFilho2->chaves[j+1]);
                    strcpy(atletaFilho1->filhos[atletaFilho1->nchaves], atletaFilho2->filhos[0]);
                    for (j = 0; j < atletaFilho2->nchaves; j++) strcpy(atletaFilho2->filhos[j], atletaFilho2->filhos[j+1]);
                    printf("DEBUG: atletaFilho2.chaves e filhos deslocados à esquerda\n");
                    printf("DEBUG: Movendo ponteiro '%s' de atletaFilho2.filhos[0] para atletaFilho1.filhos[%d]\n", atletaFilho2->filhos[0], atletaFilho1->nchaves);
                    
                    atletaFilho2->nchaves--;

                    fclose(fpi);
                    salva_no(nomeArqInd, atletaFilho1, t);
                    salva_no(nomeArqInd,atletaFilho2, t);
                    salva_no(nomeArqInd, aux, t);
        
                    libera_no(aux, t);
                    aux = TARVBM_NI_aux_cria(nomeArqInd, noFilho1, t);

                    fpi = fopen(nomeArqInd, "rb+");
                    if (!fpi){
                        printf("Erro ao reabrir %s (TARVBM_remove CASO 3A NORMAL N)", nomeArqInd);
                        exit(1);
                    }

                    printf("DEBUG: Após redistribuição → atletaFilho1.nchaves = %d | atletaFilho2.nchaves = %d\n", atletaFilho1->nchaves, atletaFilho2->nchaves);
                    printf("DEBUG: Escrita concluída nos nós filho1, filho2 e pai\n");
                    continue;
                }
            }
            if ((i > 0) && ((atletaFilho0 && atletaFilho0->nchaves >= t)|| cabecalhoFolha0.nchaves >= t)){ // CASO 3A
                printf("DEBUG: CASO 3A simétrico ativado\n");
                if (noFilho1[0] == 'F'){
                    fpf = fopen(nomeArqFol0, "rb+");
                    if(!fpf){
                        printf("Erro ao abrir %s (CASO 3A TARVBM_F_remove)", nomeArqFol0);
                        exit(1);
                    }
                    fread(&cabecalho, sizeof(TARVBM_F), 1, fpf);

                    FILE *fc = fopen("temp.bin", "wb+");
                    if (!fc){
                        printf("Erro ao criar temp.bin (CASO 3A)");
                        exit(1);
                    }

                    cabecalho.nchaves--;
                    fwrite(&cabecalho, sizeof(TARVBM_F), 1, fc);

                    fseek(fpf, 0L, SEEK_END);
                    long posFim = ftell(fpf) - sizeof(infos_atleta), posAux = sizeof(TARVBM_F);
                    fseek(fpf, posFim, SEEK_SET);

                    infos_atleta atletaTransferir, atleta, atletaSubir;
                    fread(&atletaTransferir, sizeof(infos_atleta), 1, fpf);
                    long posPen =  posFim - sizeof(infos_atleta);
                    fseek(fpf, posPen, SEEK_SET);
                    fread(&atletaSubir, sizeof(infos_atleta), 1, fpf);
                    printf("DEBUG: Atleta transferido da folha esquerda → id='%s', nome='%s'\n", atletaTransferir.id, atletaTransferir.nome);

                    strcpy(aux->chaves[i-1], atletaSubir.id);
                    fclose(fpi);
                    salva_no(nomeArqInd, aux, t);
                    fpi = fopen(nomeArqInd, "rb+");
                    if (!fpi){
                        printf("Erro ao abrir %s (TARVBM_remove CASO 3A simetrico F)", nomeArqInd);
                        exit(1);
                    }
                    printf("DEBUG: Atualizando chave[%d] do nó pai com '%s'\n", i-1, atletaSubir.id);

                    fseek(fpf, sizeof(TARVBM_F), SEEK_SET);
                    while(fread(&atleta, sizeof(infos_atleta), 1, fpf)){
                        if (posAux == posFim) continue;
                        fwrite(&atleta, sizeof(infos_atleta), 1, fc);
                        posAux += sizeof(infos_atleta);
                    }

                    fclose(fpf);
                    fclose(fc);
                    remove(nomeArqFol0);
                    printf("DEBUG: Folha '%s' atualizada → nova nchaves=%d\n", nomeArqFol0, cabecalho.nchaves);
                    rename("temp.bin", nomeArqFol0);

                    fc = fopen("temp.bin", "wb+");
                    if (!fc){
                        printf("Erro ao criar temp.bin (CASO 3A)");
                        exit(1);
                    }

                    fpf = fopen(nomeArqFol1, "rb+");
                    if (!fpf){
                        printf("Erro ao abrir arquivo %s (CASO 3A SIMETRICO TARVBM_F_remove)", nomeArqFol1);
                        exit(1);
                    }
                    
                    fread(&cabecalho, sizeof(TARVBM_F), 1, fpf);
                    cabecalho.nchaves++;
                    fwrite(&cabecalho, sizeof(TARVBM_F), 1, fc);

                    fwrite(&atletaTransferir, sizeof(infos_atleta), 1, fc);
                    printf("DEBUG: Atleta '%s' inserido no início de folha '%s'\n", atletaTransferir.id, nomeArqFol1);

                    while(fread(&atleta, sizeof(infos_atleta), 1, fpf)) fwrite(&atleta, sizeof(infos_atleta), 1, fc);
                    printf("DEBUG: Folha '%s' atualizada → novo nchaves=%d\n", nomeArqFol1, cabecalho.nchaves);

                    fclose(fc);
                    fclose(fpf);

                    remove(nomeArqFol1);
                    rename("temp.bin", nomeArqFol1);

                    break;
                }
                else{
                    printf("DEBUG: CASO 3A interno (rotação à direita): puxando chave de filho esquerdo\n");
		            printf("atletaFilho0 nchaves: %d, atletaFilho1 nchaves: %d", atletaFilho0->nchaves, atletaFilho1->nchaves);
                    int j;

                    printf("DEBUG: atletaFilho1.nchaves antes = %d\n", atletaFilho1->nchaves);
                    printf("DEBUG: Deslocando chaves de atletaFilho1 para abrir espaço na posição 0\n");
                    for (j = atletaFilho1->nchaves; j > 0; j--) strcpy(atletaFilho1->chaves[j], atletaFilho1->chaves[j-1]);
                    
                    printf("DEBUG: Deslocando ponteiros de atletaFilho1 para abrir espaço em filhos[0]\n");
                    for (j = atletaFilho1->nchaves + 1; j > 0; j--) strcpy(atletaFilho1->filhos[j], atletaFilho1->filhos[j-1]);
                    
                    printf("DEBUG: Inserindo chave do pai '%s' em atletaFilho1.chaves[0]\n", aux->chaves[i-1]);
                    strcpy(atletaFilho1->chaves[0], aux->chaves[i-1]);

                    printf("DEBUG: Atualizando chave do pai com última chave do filho esquerdo '%s'\n", atletaFilho0->chaves[atletaFilho0->nchaves - 1]);
                    strcpy(aux->chaves[i-1], atletaFilho0->chaves[atletaFilho0->nchaves - 1]);

                    printf("DEBUG: Atualizando filhos do atletaFilho1 com ultimos filhos do filho esquerdo '%s'\n", atletaFilho0->chaves[atletaFilho0->nchaves]);
                    strcpy(atletaFilho1->filhos[0], atletaFilho0->filhos[atletaFilho0->nchaves]);
                    strcpy(atletaFilho0->filhos[atletaFilho0->nchaves], "");


                    atletaFilho0->nchaves--;
                    atletaFilho1->nchaves++;
                    printf("DEBUG: Após redistribuição → atletaFilho0.nchaves=%d | atletaFilho1.nchaves=%d\n", atletaFilho0->nchaves, atletaFilho1->nchaves);

                    fclose(fpi);
                    salva_no(nomeArqInd,atletaFilho0, t);
                    salva_no(nomeArqInd,atletaFilho1, t);
                    salva_no(nomeArqInd, aux, t);
                    libera_no(aux, t);
                    printf("DEBUG: Nó pai e filhos atualizados no disco\n");
                    aux = TARVBM_NI_aux_cria(nomeArqInd, noFilho1, t);
                    fpi = fopen(nomeArqInd, "rb+");
                    if (!fpi){
                        printf("Erro ao reabrir %s (CASO 3A SIMETRICO INT)", nomeArqInd);
                        exit(1);
                    }

                    continue;
                }
            }
            if ((i < aux->nchaves) && ((atletaFilho2 && (atletaFilho2->nchaves == t - 1)) || (cabecalhoFolha2.nchaves == t - 1))){
                printf("DEBUG: CASO 3B (fusão entre filho[%d] e filho[%d])\n", i, i+1);
                if (noFilho1[0] == 'N'){
                    strcpy(atletaFilho1->chaves[t-1], aux->chaves[i]);
                    printf("DEBUG: Inserindo chave do pai '%s' em atletaFilho1.chaves[%d]\n", aux->chaves[i], t-1);

                    atletaFilho1->nchaves++;
                    int j = 0;
                    while (j < t - 1) {
                        printf("DEBUG: Transferindo chave '%s' de atletaFilho2 → atletaFilho1.chaves[%d]\n", atletaFilho2->chaves[j], t + j);
                        strcpy(atletaFilho1->chaves[t+j], atletaFilho2->chaves[j]);
                        atletaFilho1->nchaves++;
                        j++;
                    }
                    for (j = 0; j < t; j++){
                        printf("DEBUG: Transferindo ponteiro '%s' de atletaFilho2 → atletaFilho1.filhos[%d]\n", atletaFilho2->filhos[j], t + j);
                        strcpy(atletaFilho1->filhos[t+j], atletaFilho2->filhos[j]);
                        strcpy(atletaFilho2->filhos[j], "");
                    }
                    printf("acabou de transferir ponteiros\n");
                    long posVazia;
                    if(strcmp(noFilho1, noFilho2) > 0){
                        printf("DEBUG: Atualizando ID do nó fundido para '%s'\n", noFilho2);
                        printf("DEBUG: Atualizando ponteiro do pai: aux.filhos[%d] = '%s'\n", i, noFilho2);
                        strcpy(atletaFilho1->id_no, noFilho2);
                        strcpy(aux->filhos[i], noFilho2);
                        posVazia = pos1;
                        pos1 = pos2;
                        fclose(fpi);
                        salva_no(nomeArqInd, aux, t);
                        printf("id filho1: %s, noFilho2: %s\n", atletaFilho1->id_no, noFilho2);
                    }
                    else posVazia = pos2;
                    if (fpi) fclose(fpi);
                    salva_no(nomeArqInd, atletaFilho1, t);

                    printf("Vai começar a compactacao a partir do %ld\n no que sará removido: %s (chaves[0]: %s - filhos[0]: %s)\n", posVazia/tam_bloco(t), atletaFilho2->id_no, atletaFilho2->chaves[0], atletaFilho2->filhos[0]);
                    k = posPai/tam_bloco(t);
                    long posaf = calc_offset_ni(aux->filhos[0], t);
                    printf("Antes de compactar - atletaFilho1: %s - pos: %ld\n", aux->filhos[0], posaf/tam_bloco(t));
                    printf("\nk antes de compactar: %d\n", k);
                    compactar_posicao(nomeArqInd, posVazia, raizOffset, raiz_atual, &k, t);
                    printf("k depois de compactar: %d\n\n", k);

                    raizOffset = calc_offset_ni(raiz_atual, t);
                    posPai = k*tam_bloco(t);
                    char pai[TAM_NO];
                    sprintf(pai, "N%03d", k);
                    libera_no(aux, t);
                    aux = TARVBM_NI_aux_cria(nomeArqInd, pai, t);
                    posaf = calc_offset_ni(aux->filhos[0], t); // DEBUG

                    printf("atletaFilho1: %s - pos: %ld\n", aux->filhos[0], posaf/tam_bloco(t));
                    printf("DEBUG: Fusão concluída. Novo atletaFilho1.nchaves = %d\n", atletaFilho1->nchaves);
                    printf("DEBUG: atletaFilho2 liberado do índice\n");
                }
                else{
                    printf("DEBUG: CASO 3B (fusão de folhas): '%s' absorvendo '%s'\n", nomeArqFol1, nomeArqFol2);
                    fpf = fopen(nomeArqFol1, "rb+");
                    if (!fpf){
                        printf("Erro ao abrir %s (TARVBM_F_remove CASO 3B)", nomeArqFol1);
                        exit(1);
                    }

                    fpf2 = fopen(nomeArqFol2, "rb+");
                    if (!fpf2){
                        printf("Erro ao abrir %s (TARVBM_F_remove CASO 3B)", nomeArqFol2);
                        exit(1);
                    }

                    imprimir_folha(nomeArqFol1);
                    printf("\n");
                    imprimir_folha(nomeArqFol2);
                    printf("\n");

                    fread(&cabecalho, sizeof(TARVBM_F), 1, fpf2);
                    fread(&cabecalhoFolha1, sizeof(TARVBM_F), 1, fpf);
                    int f = cabecalhoFolha1.nchaves;
                    cabecalhoFolha1.nchaves += cabecalho.nchaves;
                    strcpy(cabecalhoFolha1.prox, cabecalho.prox);
                    printf("DEBUG: Cabeçalho folha1 → nchaves=%d | Próxima='%s'\n", cabecalhoFolha1.nchaves, cabecalhoFolha1.prox);
                    printf("DEBUG: Cabeçalho folha2 → nchaves=%d | Próxima='%s'\n", cabecalho.nchaves, cabecalho.prox);
                    fseek(fpf, 0L, SEEK_SET);
                    fwrite(&cabecalhoFolha1, sizeof(TARVBM_F), 1, fpf);
                    fseek(fpf, f*sizeof(infos_atleta) + sizeof(TARVBM_F), SEEK_SET);

                    infos_atleta atleta;
                    while(fread(&atleta, sizeof(infos_atleta), 1, fpf2)){
                        printf("DEBUG: Transferindo atleta id='%s' nome='%s' da '%s' para '%s'\n", atleta.id, atleta.nome, nomeArqFol2, nomeArqFol1);
                        fwrite(&atleta, sizeof(infos_atleta), 1, fpf);
                        printf("id do atleta escrito em %s: %s", nomeArqFol1, atleta.id);
                    }

                    imprimir_folha(nomeArqFol1);
                    printf("\n");
                    imprimir_folha(nomeArqFol2);
                    printf("\n");

                    fclose(fpf);
                    fclose(fpf2);
                    remove(nomeArqFol2);
                    printf("DEBUG: Fusão concluída: folha '%s' removida do arquivo\n", nomeArqFol2);
                }

                int j;
                for (j = i; j < aux->nchaves - 1; j++){
                    strcpy(aux->chaves[j], aux->chaves[j+1]);
                    strcpy(aux->filhos[j+1], aux->filhos[j+2]);
                }

                strcpy(aux->filhos[aux->nchaves], "");
                aux->nchaves--;
                printf("DEBUG: Removida chave[%d] do nó pai '%s'\n", i, aux->id_no);
                printf("DEBUG: Nova quantidade de chaves no pai = %d\n", aux->nchaves);
                fclose(fpi);
                salva_no(nomeArqInd, aux, t);
		        printf("Passou aqui\n");
                fpi = fopen(nomeArqInd, "rb+");
                if (!fpi){
                    printf("Erro ao reabrir %s (TARVBM_remove CASO 3B NORMAL INT)", nomeArqInd);
                    exit(1);
                }

                if (!aux->nchaves){
                    printf("DEBUG: Nó pai '%s' ficou vazio. Verificando tipo do filho para promover...\n", aux->id_no);
                    if (noFilho1[0] == 'N'){
                        printf("DEBUG: Promovendo atletaFilho2 como nova raiz. Novo id_no: '%s'\n", aux->id_no);
                        //strcpy(atletaFilho1->id_no, aux->id_no);
                        long posVazia;
                        if (strcmp(noFilho1, aux->id_no) < 0){
                            posVazia = posPai; 
                            printf("A posicao da raiz tera que ser removida de indice\n");
                            strcpy(raiz_atual, atletaFilho1->id_no);
                            raizOffset = calc_offset_ni(raiz_atual, t);
                        }
                        else{
                            FILE *fc = fopen("temp.bin", "wb+");
                            if (!fc){
                                printf("Erro ao criar temp.bin (CASO 3B simetrico !aux.nchaves)\n");
                                exit(1);
                            }
                            strcpy(atletaFilho1->id_no, aux->id_no);
                            posVazia = pos1;
                            pos1 = posPai;
                            long posAux = 0;
                            int cont = 0;
                            char no[TAM_NOME_NO];
                            sprintf(no, "N%03d", cont);

                            fclose(fpi);
                            TARVBM_NI *aux2 = TARVBM_NI_aux_cria(nomeArqInd, no, t);
                            fclose(fc);
                            while(aux2) {
                                if (posAux == raizOffset) salva_no("temp.bin", atletaFilho1, t);
                                else salva_no("temp.bin", aux2, t);
                                posAux += tam_bloco(t);
                                libera_no(aux2, t);
                                cont++;
                                if(cont == cont_ni) break;
                                sprintf(no, "N%03d", cont);
                                aux2 = TARVBM_NI_aux_cria(nomeArqInd, no, t);
                            }
                            remove(nomeArqInd);
                            rename("temp.bin", nomeArqInd);
                        }
                        if (pos1 == posVazia) printf("Vai começar a compactacao a partir do %ld\n no que sará removido: %s (chaves[0]: %s - filhos[0]: %s)\n", posVazia/tam_bloco(t), atletaFilho1->id_no, atletaFilho1->chaves[0], atletaFilho1->filhos[0]);
                        else printf("Vai começar a compactacao a partir do %ld\n no que sará removido: %s (chaves[0]: %s - filhos[0]: %s)\n", posVazia/tam_bloco(t), aux->id_no, aux->chaves[0], aux->filhos[0]);

                        k = posPai/tam_bloco(t);
                        if (fpi) fclose(fpi);
                        compactar_posicao(nomeArqInd, posVazia, raizOffset, raiz_atual, &k, t);
                        posPai = k*tam_bloco(t);

                        raizOffset = calc_offset_ni(raiz_atual, t);
                        libera_no(aux, t);
                        aux = TARVBM_NI_aux_cria(nomeArqInd, raiz_atual, t);
                        fpi = fopen(nomeArqInd, "rb+");
                        if (!fpi){
                            printf("Erro ao reabrir %s (CASO 3B NORMAL !aux.nchaves fim)", nomeArqInd);
                            exit(1);
                        }
                        continue;
                    }

                    fclose(fpi);
                    remove(nomeArqInd);
                    printf("DEBUG: Removido índice porque nó raiz ficou vazio e apontava para folha\n");
                    printf("DEBUG: Chamada de TARVBM_F_remove para continuar remoção na folha\n");
                    TARVBM_remove(nomeArqInd, id, raiz_atual, t, noFilho1);
                    return;
                }
                if (noFilho1[0] == 'N'){
                    printf("pos1: %ld\n", pos1/tam_bloco(t));
                    fclose(fpi);
                    libera_no(aux, t);
                    char filho1[TAM_NO];
                    sprintf(filho1, "N%03ld", pos1/tam_bloco(t));
                    aux = TARVBM_NI_aux_cria(nomeArqInd, filho1, t);
                    printf("DEBUG: Voltando à posição do nó modificado para continuar remoção\n");
                    continue;
                }
                else break;   
            }
            if ((i > 0) && ((atletaFilho0 && (atletaFilho0->nchaves == t - 1)) || (cabecalhoFolha0.nchaves == t - 1))){
                printf("DEBUG: CASO 3B interno (fusão à esquerda): atletaFilho0 absorvendo atletaFilho1\n");
                if (noFilho1[0] == 'N'){
                    if (i == aux->nchaves) strcpy(atletaFilho0->chaves[t-1], aux->chaves[i-1]);
                    else strcpy(atletaFilho0->chaves[t-1], aux->chaves[i]);
                    atletaFilho0->nchaves++;
                    int j = 0;
                    while (j < t - 1) {
                        printf("DEBUG: Transferindo chave '%s' de atletaFilho1 → atletaFilho0.chaves[%d]\n", atletaFilho1->chaves[j], t + j);
                        strcpy(atletaFilho0->chaves[t+j], atletaFilho1->chaves[j]);
                        atletaFilho0->nchaves++;
                        j++;
                    }
                    for (j = 0; j < t; j++){
                        printf("DEBUG: Transferindo ponteiro '%s' de atletaFilho1 → atletaFilho0.filhos[%d]\n", atletaFilho1->filhos[j], t + j);
                        strcpy(atletaFilho0->filhos[t+j], atletaFilho1->filhos[j]);
                        strcpy(atletaFilho1->filhos[j], "");
                    }
                    printf("acabou de transferir ponteiros\n");
                    long posVazia;
                    fclose(fpi);
                    if(strcmp(noFilho0, noFilho1) > 0){
                        printf("DEBUG: Atualizando id_no do nó fundido para '%s'\n", noFilho1);
                        printf("DEBUG: Atualizando ponteiro do pai: aux.filhos[%d] = '%s'\n", i - 1, noFilho1);
                        strcpy(atletaFilho0->id_no, noFilho1);
                        strcpy(aux->filhos[i-1], noFilho1);
                        salva_no(nomeArqInd, aux, t);
                        posVazia = pos0;
                        pos0 = pos1;
                    }
                    else posVazia = pos1;
                    salva_no(nomeArqInd, atletaFilho0, t);

                    printf("Vai começar a compactacao a partir do %ld\n no que sará removido: %s (chaves[0]: %s - filhos[0]: %s)\n", posVazia/tam_bloco(t), atletaFilho1->id_no, atletaFilho1->chaves[0], atletaFilho1->filhos[0]);
                    k = posPai/tam_bloco(t);
                    printf("\nk antes de compactar: %d\n", k);
                    compactar_posicao(nomeArqInd, posVazia, raizOffset, raiz_atual, &k, t);
                    printf("k depois de compactar: %d\n\n", k);                   

                    raizOffset = calc_offset_ni(raiz_atual, t);
                    posPai = k*tam_bloco(t);

                    char pai[TAM_NO];
                    sprintf(pai, "N%03ld", posPai/tam_bloco(t));
                    libera_no(aux, t);
                    aux = TARVBM_NI_aux_cria(nomeArqInd, pai, t);                    
                    fpi = fopen(nomeArqInd, "rb+");
                    if (!fpi){
                        printf("Erro ao reabrir %s (CASO 3B simetrico INT", nomeArqInd);
                        exit(1);
                    }

                    printf("DEBUG: Fusão concluída → atletaFilho0.nchaves = %d\n", atletaFilho0->nchaves);
                    printf("DEBUG: atletaFilho1 removido do índice\n");
                }
                else{
                    printf("DEBUG: CASO 3B (fusão de folhas): '%s' vai absorver '%s'\n", nomeArqFol0, nomeArqFol1);
                    fpf = fopen(nomeArqFol0, "rb+");
                    if (!fpf){
                        printf("Erro ao abrir %s (TARVBM_F_remove CASO 3B)", nomeArqFol0);
                        exit(1);
                    }

                    fpf2 = fopen(nomeArqFol1, "rb+");
                    if (!fpf2){
                        printf("Erro ao abrir %s (TARVBM_F_remove CASO 3B)", nomeArqFol1);
                        exit(1);
                    }

                    imprimir_folha(nomeArqFol0);
                    printf("\n");
                    imprimir_folha(nomeArqFol1);
                    printf("\n");

                    fread(&cabecalho, sizeof(TARVBM_F), 1, fpf2);
                    fread(&cabecalhoFolha0, sizeof(TARVBM_F), 1, fpf);
                    int f = cabecalhoFolha0.nchaves;
                    cabecalhoFolha0.nchaves += cabecalho.nchaves;
                    strcpy(cabecalhoFolha0.prox, cabecalho.prox);
                    printf("DEBUG: Cabeçalho folha0 → nchaves=%d | prox='%s'\n", cabecalhoFolha0.nchaves, cabecalhoFolha0.prox);
                    printf("DEBUG: Cabeçalho folha1 → nchaves=%d | prox='%s'\n", cabecalho.nchaves, cabecalho.prox);
                    fseek(fpf, 0L, SEEK_SET);
                    fwrite(&cabecalhoFolha0, sizeof(TARVBM_F), 1, fpf);
                    fseek(fpf, f*sizeof(infos_atleta) + sizeof(TARVBM_F), SEEK_SET);

                    infos_atleta atleta;
                    while(fread(&atleta, sizeof(infos_atleta), 1, fpf2)){
                        printf("DEBUG: Transferindo atleta id='%s', nome='%s' de '%s' para '%s'\n", atleta.id, atleta.nome, nomeArqFol1, nomeArqFol0);
                        fwrite(&atleta, sizeof(infos_atleta), 1, fpf);
                        printf("id do atleta escrito em %s: %s", nomeArqFol0, atleta.id);
                    }

                    imprimir_folha(nomeArqFol0);
                    printf("\n");
                    imprimir_folha(nomeArqFol1);
                    printf("\n");

                    fclose(fpf);
                    fclose(fpf2);
                    remove(nomeArqFol1);
                    printf("DEBUG: Fusão concluída. Folha '%s' foi removida\n", nomeArqFol1);
                }

                int j;
                for (j = i - 1; j < aux->nchaves - 1; j++){
                    strcpy(aux->chaves[j], aux->chaves[j+1]);
                    strcpy(aux->filhos[j+1], aux->filhos[j+2]);
                }
                printf("DEBUG: Removendo chave[%d] do nó pai '%s'\n", i - 1, aux->id_no);
                printf("DEBUG: Novo número de chaves no pai: %d\n", aux->nchaves);
                strcpy(aux->filhos[aux->nchaves], "");
                aux->nchaves--;
                fclose(fpi);
                salva_no(nomeArqInd, aux, t);
                fpi = fopen(nomeArqInd, "rb+");
                if (!fpi){
                    printf("Erro ao reabrir %s (CASO 3B simetrico)", nomeArqInd);
                    exit(1);
                }

                if (!aux->nchaves){
                    printf("DEBUG: Nó pai '%s' ficou vazio. Verificando tipo do filho para promover...\n", aux->id_no);
                    if (noFilho1[0] == 'N'){
                        printf("DEBUG: Promovendo atletaFilho0 como nova raiz. Novo id_no: '%s'\n", noFilho0);                        
                        long posVazia;
                        if (strcmp(noFilho0, aux->id_no) < 0){
                            posVazia = posPai;
                            printf("A posicao da raiz tera que ser removida de indice\n");
                            strcpy(raiz_atual, atletaFilho0->id_no);
                            raizOffset = calc_offset_ni(raiz_atual, t);                           
                        }
                        else{
                            FILE *fc = fopen("temp.bin", "wb+");
                            if (!fc){
                                printf("Erro ao criar temp.bin (CASO 3A)");
                                exit(1);
                            }

                            strcpy(atletaFilho0->id_no, aux->id_no);
                            posVazia = pos0;
                            pos0 = posPai;
                            long posAux = 0;
                            int cont = 0;
                            char no[TAM_NOME_NO];
                            sprintf(no, "N%03d", cont);

                            fclose(fpi);
                            TARVBM_NI *aux2 = TARVBM_NI_aux_cria(nomeArqInd, no, t);
                            fclose(fc);
                            while(aux2) {
                                if (posAux == raizOffset) salva_no("temp.bin", atletaFilho0, t);
                                else salva_no("temp.bin", aux2, t);
                                posAux += tam_bloco(t);
                                libera_no(aux2, t);
                                cont++;
                                if(cont == cont_ni) break;
                                sprintf(no, "N%03d", cont);
                                aux2 = TARVBM_NI_aux_cria(nomeArqInd, no, t);
                            }
                            remove(nomeArqInd);
                            rename("temp.bin", nomeArqInd);
                        }
                        
                        if (pos1 == posVazia) printf("Vai começar a compactacao a partir do %ld\n no que sará removido: %s (chaves[0]: %s - filhos[0]: %s)\n", posVazia/tam_bloco(t), atletaFilho0->id_no, atletaFilho0->chaves[0], atletaFilho0->filhos[0]);
                        else printf("Vai começar a compactacao a partir do %ld\n no que sará removido: %s (chaves[0]: %s - filhos[0]: %s)\n", posVazia/tam_bloco(t), aux->id_no, aux->chaves[0], aux->filhos[0]);

                        k = posPai/tam_bloco(t);
                        if (fpi) fclose(fpi);
                        compactar_posicao(nomeArqInd, posVazia, raizOffset, raiz_atual, &k, t);
                        
                        posPai = k*tam_bloco(t);
                        raizOffset = calc_offset_ni(raiz_atual, t);

                        libera_no(aux, t);
                        aux = TARVBM_NI_aux_cria(nomeArqInd, raiz_atual, t);
                        fpi = fopen(nomeArqInd, "rb+");
                        if (!fpi){
                            printf("Erro ao reabrir %s (CASO 3B NORMAL !aux.nchaves fim)", nomeArqInd);
                            exit(1);
                        }

                        continue;
                    }

                    fclose(fpi);
                    remove(nomeArqInd);
                    printf("DEBUG: Índice removido completamente. Chamando remoção direta na folha\n");
                    TARVBM_remove(nomeArqInd, id, raiz_atual, t,noFilho0);
                    return;
                }
                if (noFilho0[0] == 'N'){
                    printf("DEBUG: Voltando ao nó modificado na posição %ld para continuar verificação\n", pos0);
                    printf("pos0: %ld", pos0/tam_bloco(t));
                    fclose(fpi);
                    char filho0[TAM_NO];
                    sprintf(filho0, "N%03ld", pos0/tam_bloco(t));
                    libera_no(aux, t);
                    aux = TARVBM_NI_aux_cria(nomeArqInd, filho0, t);
                    fpi = fopen(nomeArqInd, "rb+");
                    if (!fpi){
                        printf("Erro ao reabrir %s (CASO 3B simetrico INT !aux.nchaves fim)", nomeArqInd);
                        exit(1);
                    }
                    continue;
                }
                else break;
            }
        }

        printf("Descida: %s, Atual: %s\n", nomeArqFol1, aux->id_no);
        if (cabecalhoFolha1.nchaves > 0){
            printf("Chegou na folha %s\n", nomeArqFol1);
            FILE *ff = fopen(nomeArqFol1, "rb+"), *fc = fopen("temp.bin", "wb+");
            if (!ff || !fc){
                printf("Erro ao abrir %s ou ao criar temp.bin (TARVBM_remove CASO 1 existe indice)", atletaFilho1->filhos[i]);
                exit(1);
            }
            TARVBM_F cabecalho;
            fread(&cabecalho, sizeof(TARVBM_F), 1, ff);
            cabecalho.nchaves--;
            fwrite(&cabecalho, sizeof(TARVBM_F), 1, fc);

            long posAux = sizeof(TARVBM_F); // JA SOMA O INICIO DO ARQUIVO
            infos_atleta atleta;
            while (fread(&atleta, sizeof(infos_atleta), 1, ff)){ // COPIA OS ELEMENTOS DO ARQUIVO ORIGINAL PARA O NOVO (EXCETO O QUE DEVE SER REMOVIDO)
                printf("DEBUG: Lendo atleta → nome='%s', id='%s' (comparando com '%s')\n", atleta.nome, atleta.id, id);
                if (strcmp(atleta.id, id) != 0){
                    printf("DEBUG: Mantendo atleta com id='%s'\n", atleta.id);
                    fwrite(&atleta, sizeof(infos_atleta), 1, fc);
                }
                posAux += sizeof(infos_atleta);
            }

            fclose(ff);
            fclose(fc);
            fclose(fpi);
            remove(nomeArqFol1);
            rename("temp.bin", nomeArqFol1);
            printf("Atleta removido com sucesso\n");
            return;
        }
        fclose(fpi);
        libera_no(aux, t);
        char filho1[TAM_NO];
        sprintf(filho1, "N%03ld", pos1/tam_bloco(t));
        aux = TARVBM_NI_aux_cria(nomeArqInd, filho1, t);
        fpi = fopen(nomeArqInd, "rb+");
        if(!fpi){
            printf("Erro ao reabrir %s fim do while\n", nomeArqInd);
            exit(1);
        }
    }
    TARBM_imprime_rec(nomeArqInd, raiz_atual, 0, t);
    printf("\n");

    printf("DEBUG: Reiniciando leitura do índice para localizar folha do id='%s'\n", id);
    fclose(fpi);
    libera_no(aux, t);
    aux = TARVBM_NI_aux_cria(nomeArqInd, raiz_atual, t);
    char nomeArqFol[TAM_NO];
    i = 0;
    while (aux){
        printf("Lendo no %s\n", aux->id_no);
        i = 0;
        while (i < aux->nchaves && strcmp(aux->chaves[i], id) < 0) i++;
        printf("aux->nchaves: %d, i: %d", aux->nchaves, i);
        if ((i < aux->nchaves) && !strcmp(id, aux->chaves[i])) i++;
        printf("DEBUG: Descendo para filho[%d] = %s\n", i, aux->filhos[i]);
        if (aux->filhos[i][0] == 'F'){
            strcpy(nomeArqFol, aux->filhos[i]);
            printf("DEBUG: Encontrada folha '%s' que pode conter id='%s'\n", nomeArqFol, id);
            libera_no(aux, t);
            break;
        }
        libera_no(aux, t);
        aux = TARVBM_NI_aux_cria(nomeArqInd, aux->filhos[i], t);
        printf("aux->id_no: %s\n", aux->id_no);
    }
    
    fpi = fopen(nomeArqFol, "rb+");
    if (!fpi){
        printf("Erro ao abrir %s (TARVBM_remove pós tratamento dos casos)", nomeArqFol);
        exit(1);
    }

    printf("DEBUG: Abrindo folha '%s' para remover atleta\n", nomeArqFol);
    TARVBM_F aux1;
    fread(&aux1, sizeof(TARVBM_F), 1, fpi); // LÊ O NÚMERO DE CHAVES DA FOLHA
    printf("DEBUG: Folha lida → nchaves=%d. Atualizando após remoção\n", aux->nchaves);

    FILE *fc = fopen("temp.bin", "wb+"); // CRIANDO O ARQUIVO ONDE OS ELEMENTOS DO ARQUIVO ORIGINAL SERÃO REESCRITOS
    if (!fc){
        printf("Falha em criar temp.bin (TARVBM_remove dentro de if (!fpi))");
        exit(1);
    }
    aux1.nchaves--; // ATUALIZA O NÚMERO DE CHAVES ANTES DA REESCRITA
    fwrite(&aux1, sizeof(TARVBM_F), 1, fc); // ESCREVE O INÍCIO DO ARQUIVO COM O NÚMERO DE CHAVES DENTRO DA FOLHA

    long posAux = sizeof(TARVBM_F); // JA SOMA O INICIO DO ARQUIVO
    infos_atleta atleta;
    while (fread(&atleta, sizeof(infos_atleta), 1, fpi)){ // COPIA OS ELEMENTOS DO ARQUIVO ORIGINAL PARA O NOVO (EXCETO O QUE DEVE SER REMOVIDO)
        if (strcmp(atleta.id, id) != 0){
            printf("DEBUG: Mantendo atleta '%s' na folha\n", atleta.id);
            fwrite(&atleta, sizeof(infos_atleta), 1, fc);
        }
        posAux += sizeof(infos_atleta);
    }

    fclose(fpi);
    fclose(fc);
    remove(nomeArqFol);
    rename("temp.bin", nomeArqFol);
    printf("DEBUG: Remoção concluída. Folha '%s' atualizada sem atleta id='%s'\n", nomeArqFol, id);
}


void listar_folhas(char *nomeArqInd, int* num_folhas, char** folhas, char *primFolha, char *raiz, int t){
    FILE *fp = fopen(nomeArqInd, "rb");
    if (!fp){
        if (errno == ENOENT){
            strcpy(folhas[(*num_folhas)++], primFolha);
            return;
        }
        printf("Erro ao abrir %s (listar_folhas)", nomeArqInd);
        exit(1);
    }
    fclose(fp);

    TARVBM_NI *aux = TARVBM_NI_aux_cria(nomeArqInd, raiz, t);
    if(!aux) return ;
    while (aux){
        if (aux->filhos[0][0] == 'F') break;
        char prox_filho[TAM_NO];
        strcpy(prox_filho, aux->filhos[0]);

        libera_no(aux, t);
        aux = TARVBM_NI_aux_cria(nomeArqInd, prox_filho, t);
	if(!aux){
            printf("Erro ao carregar filho %s\n", prox_filho);
	    return;
	}
    }
    
    strcpy(folhas[(*num_folhas)++], aux->filhos[0]);
    
    fp = fopen(aux->filhos[0], "rb");
    if (!fp){
        printf("Erro ao abrir %s (listar_folhas)", aux->filhos[0]);
	libera_no(aux,t);
        exit(1);
    }
    libera_no(aux, t);
    TARVBM_F cabecalho;
    fread(&cabecalho, sizeof(TARVBM_F), 1, fp);
    fclose(fp);
    
    while (cabecalho.prox[0] != '\0') {
        strcpy(folhas[(*num_folhas)++], cabecalho.prox);
        fp = fopen(cabecalho.prox, "rb");
        if (!fp){
            printf("Erro ao abrir %s (listar_folhas)", cabecalho.prox);
            exit(1);
        }

        if (fread(&cabecalho, sizeof(TARVBM_F), 1, fp) != 1) {
            perror("Erro na leitura do cabeçalho");
            fclose(fp);
            break;
        }
        fclose(fp);
    }

}

/****************************************************************************************************

                         obs: Isso não é um galho -  HABEMUS ÁRVORE!!!!!!!!

*****************************************************************************************************/