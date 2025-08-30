#include "respostas.h"
#include "mapeando_id.h" 
#include "insere_arv.h"

//QUESTÃO 1

int max_titulos(THT *atleta, int *gs, int *atpm){ //soma o numero de titulos
    if(!atleta) return -1;

    *gs =  atleta->ao+ atleta->fo +atleta->wib + atleta->uso; //soma grands slans
    (*atpm) =  atleta->atpI + atleta->atpMi + atleta->atpMo +  atleta->atpMa + atleta->atpR;
    (*atpm) += atleta->atpCa + atleta->atpCi + atleta->atpS+ atleta->atpP; //Atps MIL
    return (*gs) + (*atpm) + atleta->olim + atleta->atpF;
}

void imp_titulos(FILE *fp){ //formatação

    int ntitulos, gs, atpm;
    THT elem;
    printf("TENISTA:   | TOTAIS | GRAND SLAM | AUSTRALIA OPEN | FRENCH OPEN | WIMBLEDON | ATP MIL | INDIAN WELLS | MIAMI | MONTE CARLOS | MADRI | ROME | CANADA | CINCINNATI | SHANGAI | PARIS | ATP FINALS | OLIMPIC \n");
    while (fread(&elem, sizeof(THT), 1, fp)) {
        ntitulos = max_titulos(&elem, &gs, &atpm);
        if(ntitulos == -1) continue;

        char nome[TAM_NOME] = "0";
        if(!busca_sob(elem.nome, nome)) continue;
       
        printf("%-51s| %2d | %2d | %2d | %2d | %d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d \n", 
            nome, ntitulos, gs, elem.ao, elem.fo, elem.wib, elem.uso, atpm, elem.atpI, elem.atpMi, elem.atpMo,
            elem.atpMa, elem.atpR, elem.atpCa,elem.atpCi, elem.atpS, elem.atpP, elem.atpF, elem.olim);
    }
}

void Resp_titulos(){ //organiza os titulos de maneira decrescente
    FILE *fph = fopen("hashTitulos.bin", "rb"), *fpd= fopen("dadosTitulos.bin", "rb");;
    if(!fph || !fpd){
        perror("erro ao abrir hashTitulos.bin ou dadosTitulos.bin ");
        exit(1); 
    }

    FILE *fApo = fopen("titulosApos.bin", "wb+"); 
    if(!fApo) {
        perror("erro ao abrir titulosApos.bin");
        exit(1);
    }

    FILE *fA = fopen("titulosAtivos.bin", "wb+"); 
    if(!fA){
        perror("erro ao criar titulosAtivos.bin");
        exit(1);
    }
    
    TLSET *ativos = THAN_busca(1); //busca os ativos
    //TLSET_imp(ativos);
    
    int posD;
    int i;

    fseek(fph, 0L, SEEK_END);
    long total = ftell(fph);
    int posH = total / sizeof(int);


    for(i = posH - 1; i >= 0; i--){
        fseek(fph, i*sizeof(int), SEEK_SET);
        fread(&posD, sizeof(int), 1, fph);
        while(posD != -1){
            THT aux;
            fseek(fpd, posD, SEEK_SET);
            fread(&aux, sizeof(THT), 1,fpd);
            if(aux.status) {
                if(!TLSET_busca(ativos, aux.nome)){
                    fwrite(&aux, sizeof(THT), 1, fApo);
                }else{
                    fwrite(&aux, sizeof(THT), 1, fA);
                }
            }
            posD = aux.prox;
        }
    }
    fclose(fph);
    fclose(fpd);

    fseek(fApo, 0L, SEEK_END);
    int contApo = ftell(fApo);
    contApo = contApo/ sizeof(THT);
    rewind(fApo);

    fseek(fA, 0L, SEEK_END);
    int contA = ftell(fA);
    contA = contA/ sizeof(THT);
    rewind(fA);

    printf("======================================TITULOS========================================\n");

    printf("NUMERO DE JOGADORES APOSENTADOS COM TITULO: %d\n", contApo);
    printf("NUMERO DE JOGADORES ATIVOS COM TITULO: %d\n", contA);

    printf("\n================================= APOSENTADOS ======================================\n");
    imp_titulos(fApo);

    printf("\n=================================== ATIVOS ==========================================\n");
    imp_titulos(fA);

    fclose(fApo);
    fclose(fA);

    TLSET_libera(ativos);
}

//QUESTÃO 2

int ler_ano(char *id){
    char ano[5];

    for(int i= 0; i < 4; i++) ano[i] = id[i + 3];
    ano[4] = '\0';

    return atoi(ano);
}

int ler_rank(char *id){
    char rank[3] = {0};
    for(int i= 0; i < 2; i++) rank[i] = id[i + 7];
    rank[2] = '\0';

    return atoi(rank);
}

int compatriota(){

    FILE *fph = fopen("hashPais.bin", "rb");
    if(!fph){
        perror("Erro ao abrir o arquivo hashPais.bin na funçao compatriota\n");
        exit(1);
    }

    FILE *fpd = fopen("dadosPais.bin", "rb");
    if(!fpd){
        perror("Erro ao abrir o arquivo dadosPais.bin na funçao compatriota\n");
        exit(1);
    }

    int posd;
    

    while(fread(&posd, sizeof(int), 1, fph)){ //pega a posição na hashPais
        fseek(fpd, posd, SEEK_SET);
        THN tenista;
        fread(&tenista, sizeof(THN), 1, fpd); //pega o primeiro atleta

        TLSEN *atletas = THN_buscaPorPais(fpd, tenista.prox); //pega todos os conterraneos do atleta;
        TLSEN_insere_final(&atletas, tenista); //insere o primeiro atleta

        TLSEN *temp = atletas;
        
        while(temp){
            int ano = ler_ano(temp->atleta.nome);
            if (ano < 1990) { //pq so temos dados de torneios a partir de 1990
                temp = temp->prox;
                continue;
            }
            TLSEN *aux = atletas;
            while(aux){
                if(strcmp(aux->atleta.nome, temp->atleta.nome) != 0){
                    THAT *resp = THAT_busca(ano, aux->atleta.nome);
                    if(resp && resp->gs != 0){
                        int rank = ler_rank(resp->nome);
                        if(rank != 0){
                            char nomeA[51], nomeB[51];
                            if(busca_sob(aux->atleta.nome, nomeA) &&  busca_sob(temp->atleta.nome, nomeB))
                            	printf("Tenistas: %s e %s", nomeA, nomeB);
                            TLSEN_libera(atletas);
                            free(resp);
                            fclose(fpd);
                            fclose(fph);
                            return 1;
                        }
                        
                    }
                    free(resp);
                }
                aux = aux->prox;
            }
            temp = temp->prox;
        }
        TLSEN_libera(atletas);
    }
    fclose(fpd);
    fclose(fph);
    return 0;
}

//QUESTÃO 3


void mapear_pontuacoes(char *raiz, char *primFolha, int t){
    // INICIALIZA VETOR QUE VAI ARMAZENAR NOME DAS FOLHAS
    char **folhas = (char**)malloc(sizeof(char*)*cont_folhas);
    for (int i = 0; i < cont_folhas; i++){
        folhas[i] = (char*)malloc(TAM_NO * sizeof(char));
    }

    int num_folhas = 0; // indice dentro do vetor

    //percorre a arvore e adiciona o nome de cada folha ao vetor
    listar_folhas("indice.bin", &num_folhas, folhas, primFolha, raiz, t);
    FILE *fp, *fc;

    fc = fopen("arqPontuacao.bin", "wb+");
    if (!fc){
        printf("Erro ao criar arqPontuacao.bin (mapear_pontuacao)");
        exit(1);
    }

    infos_atleta jogador;
    for (int i = 0; i < num_folhas; i++){
        fp = fopen(folhas[i], "rb");
        if (!fp){
            printf("Erro ao abrir %s (mapear_pontuacao)", folhas[i]);
            exit(1);
        }

        fseek(fp, sizeof(TARVBM_F), SEEK_SET); // PULA O CABEÇALHO
        // PARA CADA JOGADOR, SOMA A PONTUACAO ACUMULADO E SALVA NO arqPontuacao.bin
        while (fread(&jogador, sizeof(infos_atleta), 1, fp)){
            TP jogP = {0};
            jogP.pontuacao =
                2000 * (jogador.ao + jogador.fo + jogador.wib + jogador.uso) +
                1000 * (jogador.iw + jogador.mi + jogador.mc + jogador.md + jogador.rm +
                        jogador.ca + jogador.ci + jogador.sh + jogador.pa);
            strcpy(jogP.id, jogador.id);
            fwrite(&jogP, sizeof(jogP), 1, fc);
        }
        fclose(fp);
    }
    fclose(fc);
    for (int i = 0; i < cont_folhas; i++){
        free(folhas[i]);
    }
    free(folhas);
}

int cmp_tp_desc(const void *a, const void *b) {
    return ((TP*)b)->pontuacao - ((TP*)a)->pontuacao;
}

void ordenar_3(char *arquivo) {
    FILE *fp = fopen(arquivo, "rb+");
    if (!fp) {
        printf("Erro ao abrir arqPontuacao.bin para ordenação");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    if (tamanho == 0) {
        printf("Arquivo %s está vazio. Nada para ordenar.\n", arquivo);
        fclose(fp);
        return;
    }
    rewind(fp);

    int qtd = tamanho / sizeof(TP);
    TP *vet = (TP*)calloc(1, tamanho);

    fread(vet, sizeof(TP), qtd, fp);
    qsort(vet, qtd, sizeof(TP), cmp_tp_desc);
    rewind(fp);
    fwrite(vet, sizeof(TP), qtd, fp);

    fclose(fp);
    free(vet);

}

void pontuacao_acum(char *arqPontuacao){
    FILE *fp = fopen(arqPontuacao, "rb");
    if(!fp){
        printf("Erro ao abrir o arquivo %s em pontuacao_acum\n", arqPontuacao);
        exit(1);
    }

    TP aux = {0};

    printf("================= JOGADORES POR PONTUAÇÃO ACUMULADA DECRESCENTE ===========\n");
    while(fread(&aux, sizeof(TP), 1, fp)){
        char nome[51];
        busca_sob(aux.id, nome);
        printf("%-51s %6d\n", nome, aux.pontuacao);
    }
     fclose(fp);
    
}

//QUESTÃO 4

int compara(const void *p1, const void* p2){
    TP *x = *(TP**)p1;
    TP *y = *(TP**)p2;
    if(x->pontuacao != y ->pontuacao) return y->pontuacao - x->pontuacao;

    return strcmp(x->id, y->id);
}

int pont(THAT atleta){
    return (atleta.gs * 2000) + (atleta.atpM * 1000);
}

void ranking_25(char *hashAnoTorneio, char *dadosAnoTorneio){
    FILE *fph = fopen(hashAnoTorneio, "rb");
    if(!fph){
        printf("Erro ao abrir %s na funcao ranking_25\n", hashAnoTorneio);
        exit(1);
    }

    FILE *fpd = fopen(dadosAnoTorneio, "rb");
    if(!fpd){
        printf("Erro ao abrir %s na funcao ranking_25\n", dadosAnoTorneio);
        exit(1);
    }

    long pos;
    while(fread(&pos, sizeof(long), 1, fph)){

        TP **vet =(TP**) malloc(sizeof(TP*) * 26);
        int i ;
        for(i = 0; i < 26; i++) vet[i] = NULL;
        int posV = 0;

        THAT elem = {0};
        while(pos != -1){

            fseek(fpd, pos, SEEK_SET);
            fread(&elem, sizeof(THAT), 1, fpd);
            if(elem.status){
                TP *aux = (TP*)malloc(sizeof(TP));
                strcpy(aux->id, elem.nome);
                aux->pontuacao = pont(elem);
                if(aux->pontuacao != 0) vet[posV++] = aux;
                else free(aux);
            }
            pos = elem.prox;
        }
        qsort(vet, posV, sizeof(TP*), compara);
        long posH = (ftell(fph) - sizeof(long)) / sizeof(long);
        
        printf("RANKING ANO %ld\n", 2024 - posH);

        for(int j = 0; j < posV; j++){
            char nome[TAM_NOME];
            if(vet[j] &&  busca_sob(vet[j]->id, nome)) {
                printf("%-51s (%2d) %10d\n", nome, j + 1, vet[j]->pontuacao);
            }
        }

        printf("\n");
        
        for(int j = 0; j < posV; j++) {
            if (vet[j]) free(vet[j]);
        }
        free(vet);
        
        posV = 0; // importante também reiniciar o contador
    }
    fclose(fpd);
    fclose(fph);
}

//QUESTÃO 5
void all_gs(char *hashAnoTorneio, char *dadosAnoTorneio){

    FILE *fph = fopen(hashAnoTorneio, "rb"), *fpd = fopen(dadosAnoTorneio, "rb");
    if (!fph || !fpd) {
        printf("Erro ao abrir %s ou %s ",hashAnoTorneio, dadosAnoTorneio );
        exit(1);
    }

    long pos;
    TLSEAT *ganhadores = TLSEAT_inicializa();

    while(fread(&pos, sizeof(long), 1, fph)){

        long posH = ftell(fph) - sizeof(long);
        posH = posH/sizeof(long);

        THAT aux;
        while(pos != -1){
            fseek(fpd, pos, SEEK_SET);
            fread(&aux, sizeof(THAT), 1, fpd);

            if(aux.gs == 4) {
                char nome[TAM_NOME];
                int i = busca_sob(aux.nome, nome);
                TLSEAT_insere_final(&ganhadores, nome, 2024 - posH);
            }

            pos = aux.prox;
        }
    }

    if(ganhadores){
        printf("Jogadores que ganharam os 4 Grand Slam por ano\n");
        TLSEAT_imp(ganhadores);
        TLSEAT_libera(ganhadores);
    } else printf("Não existem jogadores que ganharam os 4 Grand Slam por ano\n");

    fclose(fph);
    fclose(fpd);
}

//QUESTÃO 6
int furar_rank(char *hashTitulos, char *dadosTitulos, int torneio){
    FILE *fph = fopen(hashTitulos, "rb");
    if (!fph) {
        perror("Erro ao abrir hashTitulos.bin");
        exit(1);
    }
    FILE *fpd = fopen(dadosTitulos, "rb"); 
    if(!fpd){
        perror("Erro ao abrir dadosTitulos.bin");
        exit(1);
    }
    
    int pos;
    while(fread(&pos, sizeof(int), 1, fph)){

        THT aux;
        while(pos != -1){
            fseek(fpd, pos, SEEK_SET);
            fread(&aux, sizeof(THT), 1, fpd);

            if(aux.status){
                int rank = ler_rank(aux.nome);
                if(!rank){
                    char nome[TAM_NOME];
                    busca_sob(aux.nome, nome);
                    switch (torneio){
                    case 1:
                        if(aux.ao != 0 || aux.fo != 0 || aux.uso != 0 || aux.wib != 0){
                            printf("O jogador %s furou o ranking\n", nome);
                            fclose(fpd);
                            fclose(fph);
                            return 1;
                        }
                        break;
                    case 2:
                        if(aux.atpI != 0 || aux.atpMi != 0 || aux.atpMo != 0 
                            || aux.atpMa != 0 || aux.atpR != 0 || aux.atpCa != 0
                            || aux.atpCi != 0 || aux.atpS != 0 || aux.atpP != 0){
                            printf("O jogador %s furou o ranking\n", nome);
                            fclose(fpd);
                            fclose(fph);
                            return 1;
                        }
                        break;
                    case 3:
                        if (aux.atpF != 0){
                            printf("O jogador %s furou o ranking\n", nome);
                            fclose(fpd);
                            fclose(fph);
                            return 1;
                        }
                        break;
                    case 4:
                        if (aux.olim != 0){
                            printf("O jogador %s furou o ranking\n", nome);
                            fclose(fpd);
                            fclose(fph);
                            return 1;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            pos = aux.prox;
        }   
    }

    fclose(fpd);
    fclose(fph);
    printf("Nenhum jogador furou o ranking\n");
    return 0;
}

//QUESTÃO 7

int max_titulosM(infos_atleta *jogador){ //soma o numero de titulos
    if(!jogador) return -1;

    return jogador->ao + jogador->fo +jogador->wib + 
    jogador->uso + jogador->iw +jogador->mi +jogador->mc +jogador->md + jogador->rm + 
    jogador->ca + jogador->ci+jogador->sh + jogador->pa; 
}

void remover_atletas(char *nacionalidades, char *raiz, int t, char *primFolha){
    FILE *fph = fopen("hashPais.bin", "rb+");
    if (!fph){
        perror("Erro ao abrir hashPais.bin em THN_removeAtleta");
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
        perror("Erro ao abrir dadosPais.bin");
        exit(1);
    }

    THN elem;
    char copia_nome[TAM_NO];
    while (pos != -1) {
        fseek(fpd, pos, SEEK_SET);
        fread(&elem, sizeof(THN), 1, fpd);

        elem.status = 0;
        fseek(fpd, pos, SEEK_SET);
        fwrite(&elem, sizeof(THN), 1, fpd); //matei ele na hash de nacionalidade
        
        infos_atleta *aux = TARVBM_NI_busca("indice.bin", elem.nome , raiz, t, copia_nome, primFolha); //busca na arvore
        if(!aux) {
           pos = elem.prox;
	   continue;
	}
	int ntitulos = max_titulosM(aux);
        THAN_removeAtleta(aux->id, aux->idade - 20); //matei ele na hash de aposentados
        THT_removeAtleta("hashTitulos.bin", "dadosTitulos.bin", aux->id, ntitulos); //matar ele na hash de titulo
        THTO_removeAtleta("dadosTorneio.bin", aux->id); //mata ele na hash Torneios
        THAT_removeAtleta("dadosAnoTorneio.bin", aux->id); //matei ele na hash de torneio por ano
        TARVBM_remove("indice.bin", aux->id, raiz, t, primFolha);
        free(aux);
        pos = elem.prox;
    }

    fclose(fpd); 
    return;

}

void ordem_aux(int cont){
    switch (cont){
    case 0: printf("VENCEDORES AUSTRALIA OPEN:\n"); break;
    case 1: printf("VENCEDORES FRENCH OPEN:\n"); break;
    case 2: printf("VENCEDORES WIMBLEDON OPEN:\n"); break;
    case 3: printf("VENCEDORES US OPEN OPEN:\n"); break;
    case 4: printf("VENCEDORES ATP FINALS:\n"); break;
    case 5: printf("VENCEDORES OLIMPIADAS:\n"); break;
    case 6: printf("VENCEDORES INDIAN WELLS: \n"); break;
    case 7: printf("VENCEDORES MIAMI: \n"); break;
    case 8: printf("VENCEDORES MONTE: CARLO: \n"); break;
    case 9: printf("VENCEDORES MADRI: \n"); break;
    case 10: printf("VENCEDORES ROME: \n"); break;
    case 11: printf("VENCEDORES CANADA: \n"); break;
    case 12: printf("VENCEDORES CINCINNATI: \n"); break;
    case 13: printf("VENCEDORES SHANGUAI \n"); break;
    case 14: printf("VENCEDORES PARIS\n"); break;
    }
}


//QUESTÃO 8

int pont_anterior(T8M* atleta){
    int gs = 0, atpM = 0;
    THAT* jogador = THAT_busca((atleta->ano)-1, atleta->id);
    if(!jogador) return 0;
    if(jogador->gs) gs = jogador->gs;
    if(jogador->atpM) atpM = jogador->atpM;
    int pont = gs*2000 + atpM*1000;
    free(jogador);
    return pont;
}

int compara_completa(const void *p1, const void* p2){
    T8M *x = *(T8M**)p1;
    T8M *y = *(T8M**)p2;

    //Critério 1: Pontuação
    if(x->pontuacao != y ->pontuacao) return y->pontuacao - x->pontuacao;

    //Critério 2: GS
    if(x->gs != y->gs) return y->gs - x->gs;

    //Critério 3: APT 100
    if(x->atp != y->atp) return y->atp - x->atp;  
    
    //Critério 4: pontuação do ano anterior
    int pont_x = pont_anterior(x);
    int pont_y = pont_anterior(y);
    if(pont_x != pont_y) return pont_y - pont_x;

    //Critério 5: "Default"
    return strcmp(x->id, y->id); 

}

void participaram_atp(char *hashAnoTorneio, char *dadosAnoTorneio) {
    FILE *fph = fopen(hashAnoTorneio, "rb");
    if (!fph) { 
        printf("Erro ao abrir %s\n", hashAnoTorneio); 
        exit(1); }

    FILE *fpd = fopen(dadosAnoTorneio, "rb");
    if (!fpd) { 
        printf("Erro ao abrir %s\n", dadosAnoTorneio); 
        exit(1); }

    int total_anos = 35;

    for (int i = 0; i < total_anos - 1; i++) {
        long pos;
        fseek(fph, i * sizeof(long), SEEK_SET);
        fread(&pos, sizeof(long), 1, fph);

        if (pos == -1) continue;

        T8M **vet = malloc(sizeof(T8M*) * 15);
        for (int j = 0; j < 15; j++) vet[j] = NULL;
        int posV = 0;

        THAT elem = {0};
        int ano = 2024 - i;

        // PRIMEIRO TENTA PELOS CRITÉRIOS DO ANO ATUAL
        while (pos != -1) {
            fseek(fpd, pos, SEEK_SET);
            fread(&elem, sizeof(THAT), 1, fpd);

            if (elem.status) {
                //não tem elemento repetido porque estamos fazendo por multiplicador
                T8M *aux = (T8M*) calloc(1, sizeof(T8M));
                strcpy(aux->id, elem.nome);
                aux->pontuacao = pont(elem);
                aux->gs = elem.gs;
                aux->atp = elem.atp;
                aux->ano = ano;
                if (aux->pontuacao != 0) vet[posV++] = aux;
                else free(aux);
            }
            pos = elem.prox;
        }

        qsort(vet, posV, sizeof(T8M*), compara_completa);

        //SE NÃO CHEGAR EM 8 PARTICIPANTES PEGA OS MELHORES DO ANTERIOR
        if (posV < 8) {
            int faltam = 8 - posV;

            //tá de 2024 para 1990 ent é +1
            long pos_ant;
            fseek(fph, (i + 1) * sizeof(long), SEEK_SET);
            fread(&pos_ant, sizeof(long), 1, fph);

            TP **ranking = malloc(sizeof(TP*) * 15);
            int qntd = 0;
            THAT elem = {0};

            //cópia da função da ranking_25 (se der tempo voltar para adaptar
            //aquela função para talvez receber direto o ano que a gnt quer (dividir)
            //e n precisar ter dois trechos iguais no código, modularizar isso
            while(pos_ant != -1){
                fseek(fpd, pos_ant, SEEK_SET);
                fread(&elem, sizeof(THAT), 1, fpd);
                if(elem.status){
                    TP *aux = (TP*)malloc(sizeof(TP));
                    strcpy(aux->id, elem.nome);
                    aux->pontuacao = pont(elem);
                    if(aux->pontuacao != 0) ranking[qntd++] = aux;
                    else free(aux);
            }
            pos_ant = elem.prox;
            }

            qsort(ranking, qntd, sizeof(TP*), compara);

            // Adiciona no final do vetor os jogadores restantes
            //confere se o jogador já não pegou a vaga direto pelo ano corrente
            for (int k = 0; k < qntd && posV < 8; k++) {
                int duplicado = 0;
                for (int j = 0; j < posV; j++) {
                    if (strcmp(vet[j]->id, ranking[k]->id) == 0) {
                        duplicado = 1;
                        break;
                    }
                }
                // se não tá insere
                if (!duplicado) {
                    T8M *novo = (T8M*) calloc(1, sizeof(T8M));
                    strcpy(novo->id, ranking[k]->id);
                    novo->pontuacao = ranking[k]->pontuacao;
                    novo->ano = ano - 1;
                    novo->gs = 0;
                    novo->atp = 0;
                    vet[posV++] = novo;
                }
            }

            for (int k = 0; k < qntd; k++) free(ranking[k]); // libera ranking anterior
            free(ranking);
        }

        printf("\nRANKING DOS JOGADORES DO ATP FINALS %d\n", ano);
        for (int j = 0; j < posV && j < 8; j++) {
            char nome[TAM_NOME];
            if (vet[j] && busca_sob(vet[j]->id, nome)) {
                printf("%-51s\n", nome);
            }
        }
        printf("\n");

        for (int j = 0; j < posV; j++) if (vet[j]) free(vet[j]);
        free(vet);
    }

    fclose(fpd);
    fclose(fph);
}

//QUESTÃO 9
void ordem_torneio(char *hashTorneio, char *dadosTorneio){
    FILE *fph = fopen(hashTorneio, "rb");
    if(!fph) {
        printf("Erro ao abrir %s na ordem_torneio", hashTorneio);
        exit(1);
    }

    FILE *fpd = fopen(dadosTorneio, "rb");
    if(!fpd) {
        printf("Erro ao abrir %s na ordem_torneio", dadosTorneio);
        exit(1);
    }

    int posH = ftell(fph)/ sizeof(long);
    long pos;

    for(int i = posH; i < THTO_TAM; i++) {
        ordem_aux(i);

        fseek(fph, i*sizeof(long), SEEK_SET);
        fread(&pos, sizeof(long), 1, fph);

        TP **vet =(TP**) malloc(sizeof(TP*) * 50);
        int i ;
        for(i = 0; i < 50; i++) vet[i] = NULL;
        int posV = 0;
        THTO aux;

        while(pos != -1){ //pega as informaçoes de cada torneio e coloca em um vetor caso o status seja 1

            fseek(fpd, pos, SEEK_SET);
            fread(&aux, sizeof(THTO), 1, fpd);
            if(aux.status){
                TP *elem = (TP*)malloc(sizeof(TP));
                int k = busca_sob(aux.id, elem->id);
                elem->pontuacao = aux.mult;
                vet[posV++] = elem;
            }
            pos = aux.prox;
        }
        qsort(vet, posV, sizeof(TP*), compara); //ordena o vetor baseado no numero de titulos/
        ordem_aux(i);

        for(int j = 0; j < posV; j++){
            if(vet[j]) printf("%-51s (%2d) %10d\n", vet[j]->id, j + 1, vet[j]->pontuacao);
        }
        printf("\n");

        for(int j = 0; j < posV; j++)
            if (vet[j]) free(vet[j]);
        free(vet);
        posV = 0; // importante também reiniciar o contador
    }
}