#include <stdio.h>
#include "leitura_dados.h"
#include "insere_arv.h"
#include "hash.h"
#include "mapeando_id.h"
#include "respostas.h"
#include "mapeando_nascionalidade.h"


void imprimir_atleta(infos_atleta *tenista){
    printf("\n----------------------------------------------------------------- ");
    printf("\nID: %s", tenista->id);
    printf("\nNome: %s", tenista->nome);
    printf("\nNascionalidade: %s", tenista->nasc);
    printf("\nIdade: %d", tenista->idade);
    printf("\nMelhor Rank: %d", tenista->rank);
    printf("\nAno melhor rank: %d", tenista->anoMelRank);
    if(tenista->morte) printf("\nMorto");
    else printf("\nVivo");
    printf("\nNúmero de vezes que ganhou cada Roland Garros ------------------- ");
    printf("\t\nAustralia open: %d", tenista->ao);
    printf("\t\nFrench Open: %d", tenista->fo);
    printf("\t\nWimbledom: %d", tenista->wib);
    printf("\t\nUs Open: %d", tenista->uso);
    printf("\nNúmero de vezes que ganhou cada ATP1000 ------------------------- ");
    printf("\t\nIndiam Wells: %d", tenista->iw);
    printf("\t\nMiami: %d", tenista->mi);
    printf("\t\nMonte Carlo: %d", tenista->mc);
    printf("\t\nMadrid: %d", tenista->md);
    printf("\t\nRome: %d", tenista->rm);
    printf("\t\nCanada: %d", tenista->ca);
    printf("\t\nCincinnati: %d", tenista->ci);
    printf("\t\nShangai: %d", tenista->sh);
    printf("\t\nParis: %d", tenista->pa);
    printf("\nNúmero de vezes que ganhou as competições sem pontuação --------- ");
    printf("\t\nATP Finals: %d", tenista->atpF);
    printf("\t\nOlympic Games: %d", tenista->olim);
    printf("\n----------------------------------------------------------------\n");
}

void opcoes_perguntas(){

    printf("\n");
    printf("\n======================================\n");
    printf("======================================\n");
    printf("\n            MENU PERGUNTAS          \n");
    printf("\n======================================\n");
    printf("======================================\n");
    printf("\n");
    printf("Q%2d. %-50s\n", 0, "Voltar para o menu principal: \n");
    printf("Q%2d. %-50s\n", 1, "Listar jogadores por numero titulos [ativos e aposentados]: \n");
    printf("Q%2d. %-50s\n", 2, "Jogadores compatriotas: \n");
    printf("Q%2d. %-50s\n", 3, "Pontuação acumulada dos jogadores: \n");
    printf("Q%2d. %-50s\n", 4, "Ranking até 25 por ano: \n");
    printf("Q%2d. %-50s\n", 5, "Jogadores que ja ganharam 4 Grand Slam no mesmo ano: \n");
    printf("Q%2d. %-50s\n", 6, "Algum jogador que furou o ranking: \n");
    printf("Q%2d. %-50s\n", 7, "Remover todos os jogadores de um pais [em atividade ou não]: \n");
    printf("Q%2d. %-50s\n", 8, "Listar os 8 melhores jogadores do ano: \n");
    printf("Q%2d. %-50s\n", 9, "Listar vencedores em ordem decrescente por torneio: \n");
    printf("%-50s\n","Escolha uma opção:  \n");
}

void menu_perguntas(char *raiz, char *primFolha, int t){
    
    int op, c;
    do{
        opcoes_perguntas();
        scanf("%d",&op);
        while ((c = getchar()) != '\n' && c != EOF);  // limpa o buffer após scanf

        switch (op){
        case 1:
            Resp_titulos();
            break;
        case 2: 
            if(!compatriota()) printf("Não há atletas compatriotas que cumprem os requisitos\n");
            break;
        case 3: 
            pontuacao_acum("arqPontuacao.bin");
            break;
        case 4:
            ranking_25("hashAnoTorneio.bin", "dadosAnoTorneio.bin");
            break;
        case 5:
            all_gs("hashAnoTorneio.bin", "dadosAnoTorneio.bin");
            break;
        case 6:{
            printf("Qual torneio?\n1. Grand Slam\n2. ATP MIL\n3. ATP Finals\n4. Olimpiadas: ");
            int temp;
            scanf("%d", &temp);
            furar_rank("hashTitulos.bin", "dadosTitulos.bin", temp);
            break;
        }
        case 7:{
            char pais[TAM_PAIS];
            printf("Insira a nacionalidade dos jogadores: ");
            fgets(pais, sizeof(pais), stdin);
            pais[strcspn(pais, "\n")] = '\0';
            remover_atletas(pais, raiz, t, primFolha);
            break;
        }
        case 8:
            participaram_atp("hashAnoTorneio.bin", "dadosAnoTorneio.bin");
            break;
        case 9:
            ordem_torneio("hashTorneio.bin", "dadosTorneio.bin");
            break;
        }
    }while(op != 0);

}

void inserir_tabela(char *nomeArq, TID *elem, int x){
    FILE *fp = fopen(nomeArq, "rb+");
    if(!fp){
        printf("Erro ao abrir %s em inserir_tabela\n", nomeArq);
        exit(1);
    }
    FILE *fc = fopen("temp.bin", "wb+");
    if(!fc){
        printf("Erro ao abrir %s em inserir_tabela\n", "temp.bin");
        exit(1);
    }

    TID aux = {0};
    int inscrito = 0;
    while(fread(&aux, sizeof(TID), 1, fp)){
        if(x == 1){  //tabela de ids
            if(strcmp(aux.id, elem->id) < 0){
                fwrite(&aux, sizeof(TID), 1, fc);
                continue;
            } 
            else if(!inscrito) {
                fwrite(elem, sizeof(TID), 1, fc);
                inscrito = 1;
            }
            fwrite(&aux, sizeof(TID), 1, fc);
        }else{ //tabela de sobrenomes
            if(strcmp(aux.sobrenome, elem->sobrenome) < 0){
                fwrite(&aux, sizeof(TID), 1, fc);
                continue;
            } 
            else if(!inscrito) {
                fwrite(elem, sizeof(TID), 1, fc);
                inscrito = 1;
            }
            fwrite(&aux, sizeof(TID), 1, fc);
        }
    }
    if(!inscrito) {
        fwrite(elem, sizeof(TID), 1, fc);
    }
    fclose(fp);
    fclose(fc);

    remove(nomeArq);
    rename("temp.bin", nomeArq);
}

void opcoesArv(){

    printf("\n");
    printf("\n======================================\n");
    printf("======================================\n");
    printf("\n            MENU TENISTAS           \n");
    printf("\n======================================\n");
    printf("======================================\n");
    printf("\n");
    printf("%2d. %-50s\n", 1, "Inserir tenista: \n");
    printf("%2d. %-50s\n", 2, "Remover tenista: \n");
    printf("%2d. %-50s\n", 3, "Imprimir árvore: \n");
    printf("%2d. %-50s\n", 4, "Buscar tenista: \n");
    printf("%2d. %-50s\n", 5, "Buscar todos os tenistas em atividade por pais: \n");
    printf("%2d. %-50s\n", 6, "Perguntas: \n");
    printf("%2d. %-50s\n", 7, "Remover tenista pelo ID: \n");
    printf("%2d. %-50s\n", 8, "Sair do programa: \n");
    printf("%-50s\n","Escolha uma opção:  \n");

}

void opcao_1(char *nome, char *raiz, int t){
    char pais[TAM_PAIS], anoNasc[5], rank[3], anoRank[5];
    int anoM;

    printf("Insira a nacionalidade do jogador: ");
    scanf("%s", pais);
    printf("Insera o ano de nascimento: ");
    scanf("%s", anoNasc);
    printf("Ano de morte [caso não tenha morrido, inserir 0000]: ");
    scanf("%d", &anoM);
    printf("Insira o melhor rank do tenista [caso não seja maior que 25, inserir 00]: ");
    scanf("%s", rank);
    printf("Insira o ano de melhor Rank [ caso não haja, insira 0000]: ");
    scanf("%s", anoRank);
    
    if(strlen(rank) == 1){
        char temp[3];
        temp[0] = '0';
        temp[1] = rank[0];
        temp[2] = '\0';
        strcpy(rank, temp);
    }
    
    int idade = cal_idade(anoNasc);

    TID *id = montar_id(nome, anoNasc, rank);
    inserir_tabela("ids_table.bin", id, 1);
    inserir_tabela("sobrenome_table.bin", id, 2);
    THAN_insere("hashAposentado.bin", "dadosAposentado.bin", id->id, idade, anoM); 
    THN_insere("hashPais.bin", "dadosPais.bin", id->id,  pais);
    //INSERIR NA HASH DE TITULOS, TORNEIO E ANO POR TORNEIO

    infos_atleta* jogador = (infos_atleta*) calloc (1,sizeof(infos_atleta));

    int rank_in_arv, melhor_rank;
    if(strcmp(rank, "00") == 0){
        rank_in_arv = -1;
        melhor_rank = -1;
    }else {
        rank_in_arv = atoi(rank);
        melhor_rank = atoi(anoRank);
    }
    
    strcpy(jogador->id, id->id);
    strcpy(jogador->nome, nome);
    jogador->rank = rank_in_arv;
    jogador->morte = anoM;
    jogador->anoMelRank = melhor_rank;
    jogador->idade = idade + 20;
    strcpy(jogador->nasc, pais);
    jogador->ao = 0;
    jogador->fo = 0;
    jogador->wib = 0;
    jogador->uso = 0;
    jogador->atpF = 0;
    jogador->olim = 0;
    jogador->iw = 0;
    jogador->mi = 0;
    jogador->mc = 0;
    jogador->md = 0;
    jogador->rm = 0;
    jogador->ca = 0;
    jogador->ci = 0;
    jogador->sh = 0;
    jogador->pa = 0;

    //Inserindo na árvore 
    int tam_NI = tam_bloco(t);
    TARVBM_insere("indice.bin", jogador, raiz, t, tam_NI);
    free(jogador);
    free(id);
}

void opcao_2(char *nome, char *raiz_atual, int t, char *primFolha){
    char id[TAM_ID], copia_nome[TAM_NO], sobrenome[TAM_NOME];
    ultimo_sobrenome(nome, sobrenome); //verificar se tem mais de um sobrenome
    if(busca_id(id, sobrenome, nome)){
        infos_atleta *aux = TARVBM_NI_busca("indice.bin", id , raiz_atual, t, copia_nome, primFolha); //busca na arvore
        if(!aux) {
            printf("Jogador não está na árvore 2\n");
            free(aux);
            return;
        }
        int ntitulos = max_titulosM(aux);
        THAN_removeAtleta(id, aux->idade - 20); //matei ele na hash de aposentados
        THT_removeAtleta("hashTitulos.bin", "dadosTitulos.bin", id, ntitulos); //matar ele na hash de titulos
        THN_removeAtleta(id, aux->nasc); //mata ele na hash pais
        THTO_removeAtleta("dadosTorneio.bin", id); //mata ele na hash Torneios
        THAT_removeAtleta("dadosAnoTorneio.bin", id); //matei ele na hash de torneio por ano
        TARVBM_remove("indice.bin", id, raiz_atual, t, primFolha);
        free(aux);
    }else printf("Jogador não está na árvore\n");
}

void opcao_5(char *nacionalidade, char *raiz, int t, char *primFolha){
    FILE *fph = fopen("hashPais.bin", "rb");
    if(!fph){
        perror("Erro ao abrir o arquivo hashPais.bin na funçao main\n");
        exit(1);
    }

    FILE *fpd = fopen("dadosPais.bin", "rb");
    if(!fpd){
        perror("Erro ao abrir o arquivo dadosPais.bin na funçao main\n");
        exit(1);
    }
    int h = THN_hash(nacionalidade), pos; // busca binaria para saber a posição onde pais tem que ir na hash
    if(h == -1) {
        fclose(fph);
        printf("Não existe atletas nesse pais");
        return;
    }
    fseek(fph, (h*sizeof(int)), SEEK_SET);
    fread(&pos, sizeof(int), 1, fph);
    TLSEN *atletas = THN_buscaPorPais(fpd, pos); //pega todos os atletas daquele pais;
    int ano;
    char copia_nome[TAM_NO];
    while(atletas){
        ano = ler_ano(atletas->atleta.nome);
        if(2024 - ano < 39){ //verificar se esta ativo
            infos_atleta *aux = TARVBM_NI_busca("indice.bin", atletas->atleta.nome , raiz, t, copia_nome, primFolha); //busca na arvore
            if(aux && !aux->morte) imprimir_atleta(aux); 
            free(aux);
        }
        atletas = atletas->prox;
    }
    TLSEN_libera(atletas);
    fclose(fph);
    fclose(fpd);
}

void opcao_4(char *nome, char *raiz_atual, int t, char *primFolha){

    char id[TAM_ID], copia_nome[TAM_NO], sobrenome[TAM_NOME];
    ultimo_sobrenome(nome, sobrenome); //verificar se tem mais de um sobrenome

    if(busca_id(id, sobrenome, nome)){
        infos_atleta *aux = TARVBM_NI_busca("indice.bin", id , raiz_atual, t, copia_nome, primFolha); //busca na arvore
        if(!aux) {
            printf("Jogador não está na árvore\n");
            free(aux);
            return;
        }
        if(!strcmp(aux->id, "Nov198701Dj")) printf("Péssima escolha\n");
        imprimir_atleta(aux); //morto não ta ativo
        free(aux);
    }else printf("Jogador %s não está na árvore\n", nome);

}


void opcao_7(char *id, char *raiz_atual, int t, char *primfolha){
    char copia_nome[TAM_NO];
    infos_atleta *aux = TARVBM_NI_busca("indice.bin", id , raiz_atual, t, copia_nome, primfolha); //busca na arvore
    if(!aux) {
        printf("Jogador não está na árvore\n");
        free(aux);
        return;
    }
    int ntitulos = max_titulosM(aux);
    THAN_removeAtleta(id, aux->idade - 20); //matei ele na hash de aposentados
    THT_removeAtleta("hashTitulos.bin", "dadosTitulos.bin", id, ntitulos); //matar ele na hash de titulos
    THN_removeAtleta(id, aux->nasc); //mata ele na hash pais
    THTO_removeAtleta("dadosTorneio.bin", id); //mata ele na hash Torneios
    THAT_removeAtleta("dadosAnoTorneio.bin", id); //matei ele na hash de torneio por ano
    TARVBM_remove("indice.bin", id, raiz_atual, t, primfolha);
    free(aux);
}

int main(){
    char* raiz_atual = (char*) calloc(TAM_NOME_NO, sizeof(char));
    strcpy(raiz_atual, "N000");

    char* primfolha = (char*) calloc(TAM_NOME_NO, sizeof(char));
    strcpy(primfolha, "F000");

    int t;
    printf("Insira um valor de t maior que 2: ");
    scanf("%d", &t);
    while(t < 2){
        printf("Insira um valor de t maior que 2: ");
        scanf("%d", &t);
    }

    iniciar(t, raiz_atual, primfolha);

    int op;
    int c;
    do{
        opcoesArv();
        scanf("%d",&op);
        while ((c = getchar()) != '\n' && c != EOF);  // limpa o buffer após scanf
        switch (op){
        case 1: {
            char nome[TAM_NOME];
            printf("Insira o nome e sobrenome do jogador: ");
            fgets(nome, sizeof(nome), stdin);

            nome[strcspn(nome, "\n")] = '\0';
            opcao_1(nome, raiz_atual, t);

            printf("Jogador: %s inserido na árvore\n", nome);
            break;
        }
        case 2: {
            char nome[TAM_NOME];
            printf("Insira nome e o sobrenome do jogador: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            opcao_2(nome, raiz_atual, t, primfolha);
            break;
        }
        case 3:
            TARBM_imprime_rec("indice.bin", raiz_atual, 0, t); 
            break;
        case 4:{
            char nome[TAM_NOME];
            printf("Insira o nome e sobrenome do jogador: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
            opcao_4(nome, raiz_atual, t, primfolha);
            break;
        }
        case 5:{
            char pais[TAM_PAIS];
            printf("Insira a nacionalidade dos jogadores: ");
            fgets(pais, sizeof(pais), stdin);
            pais[strcspn(pais, "\n")] = '\0';

            opcao_5(pais, raiz_atual, t, primfolha);
            break;
        }
        case 6:
            menu_perguntas(raiz_atual, primfolha, t);
            break;
        case 7:{
            char nome[TAM_NOME];
            printf("Insira o ID do jogador: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
            opcao_7(nome, raiz_atual, t, primfolha);
            break;
        }
    }

    }while(op != 8);

    free(raiz_atual);
    free(primfolha);

    return 0;

}