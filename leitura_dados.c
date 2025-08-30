#include "leitura_dados.h"
#include "respostas.h"

//Função para descobrir o "h" (para que bloco vai) cada jogador de acordo c a idade
int cal_idade(char *ano){
    int idade = atoi(ano);
    idade = 2004 - idade;
    return idade;
}

//inicializar todas as hashs
void hash_inicializar(){

    THN_inicializa("hashPais.bin", "dadosPais.bin");
    THAN_inicializa("hashAposentado.bin", "dadosAposentado.bin");
    THAT_inicializa("hashAnoTorneio.bin", "dadosAnoTorneio.bin");
    THT_inicializa("hashTitulos.bin", "dadosTitulos.bin");
    THTO_inicializa("hashTorneio.bin", "dadosTorneio.bin");

}

//leitura do arquivo de campeos
void ler_campeaos(char* raiz_atual, int t){
    char linha[350];
    FILE *fp = fopen("champions.txt", "r"); //POSSÍVEL MODIFICAÇÃO 
    if(!fp){
        perror("Erro ao abrir o arquivo champions.txt\n");
        exit(1);
    }

    fgets(linha, sizeof(linha), fp);
    char *result = fgets(linha, sizeof(linha), fp);
    while(result){
        char *token = strtok(result, "\\");

        char anoc[5];
        strncpy(anoc, token, 4);
        anoc[4] = '\0';

        int ano = atoi(anoc);

        int cont = 1;

        token = strtok(NULL, "\\");

        while(token){

            if(!strcmp(token, "-")){
                token = strtok(NULL, "\\");
                cont++;
                continue;
            }

            char sob_temp[TAM_NOME];
            int ganho, ntitulos;

            if(sscanf(token, "%[^(](%d/%d)", sob_temp, &ganho, &ntitulos) != 3){
                printf("Erro ao ler token: %s\n", token);
                token = strtok(NULL, "\\");
                cont++;
                continue;
            }
            char sobrenome[TAM_NOME];
            ultimo_sobrenome(sob_temp, sobrenome); //verificar se tem mais de um sobrenome

            char id[TAM_ID];
    
            if(!busca_id(id, sobrenome, sob_temp)){
                cont ++;
                token = strtok(NULL, "\\");
                //printf("não achou o id %s\n", sobrenome);
                continue;
            }  // não achou o id;

            //Inserindo nas hashs pelo número de títulos, ganhadores por ano e ?
            THT_insere("hashTitulos.bin", "dadosTitulos.bin", id, ntitulos, cont);
            THAT_insere("hashAnoTorneio.bin", "dadosAnoTorneio.bin", id, ano, cont);
            THTO_insere("hashTorneio.bin", "dadosTorneio.bin", id, cont - 1);

            char fantasia[TAM_NO];
            infos_atleta *atleta = TARVBM_NI_busca("indice.bin", id, raiz_atual, t, fantasia, "F000.bin"); //busca o jogador na arvore

            if(!atleta){
                cont ++;
                token = strtok(NULL, "\\");
                printf("Atleta %s não está na arvore\n", id);
                continue;
            } 

            switch(cont){
                case 1: atleta->ao++; break;
                case 2: atleta->fo++; break;
                case 3: atleta->wib++; break;
                case 4: atleta->uso++; break;
                case 5: atleta->atpF++; break;
                case 6: atleta->olim++; break;
                case 7: atleta->iw++; break;
                case 8: atleta->mi++; break;
                case 9: atleta->mc++; break;
                case 10: atleta->md++; break;
                case 11: atleta->rm++; break;
                case 12: atleta->ca++; break;
                case 13: atleta->ci++; break;
                case 14: atleta->sh++; break;
                case 15: atleta->pa++; break;
            }
            
            TARVBM_F_atualizarJogador(atleta, raiz_atual, t);
            free(atleta);
            
            cont ++; //conta os torneios
            token = strtok(NULL, "\\");
        }
        result = fgets(linha, sizeof(linha), fp);
    }
    fclose(fp);
}

void ler_tenistas(int t, int tam_NI, char* raiz_atual){
    char linha[65];
    FILE *fp = fopen("tennis_players.txt", "r"); //POSSÍVEL MODIFICAÇÃO
    if (!fp){
        perror("Erro ao abrir o arquivo tennis_players.txt");
        exit(1);
    } 

    char* result = fgets(linha, sizeof(linha), fp); //lê e ignora a primeira linha (porque é aquela q identifica os campos só)
    result = fgets(linha, sizeof(linha), fp); //lê a primeira linha com informações de vdd
    char* id = malloc(sizeof(char) * TAM_ID); //aloca a posição para o id
    TID **vet_ids= (TID**) malloc (sizeof(TID*) * 400);
    for (int i = 0; i < 400; i++) vet_ids[i] = NULL;
    int pos = 0 ;//mapeando_id

    char nome[TAM_NOME], pais[TAM_PAIS], anoNasc[5], anoM[5], rank[3], anoRank[5];

    while(result){

        //printf("\nLENDO LINHA %s",  result);

        //Inicializando a preparação do struct de jogador que vai para a árvore
        infos_atleta* jogador = (infos_atleta*) calloc (1,sizeof(infos_atleta));

        sscanf(linha,"%[^\\]\\%[^\\]\\%[^\\]\\%[^\\]\\%[^\\]\\%s", nome, anoNasc, anoM, pais, rank, anoRank);

        if(!strcmp(anoM, "-")) strcpy(anoM, "0000");
        if(!strcmp(rank, "-") || !strcmp(anoRank, "-")){
            strcpy(rank, "00");
            strcpy(anoRank, "0000");
        }
        
        if(strlen(rank) == 1){
            char temp[3];
            temp[0] = '0';
            temp[1] = rank[0];
            temp[2] = '\0';
            strcpy(rank, temp);
        }
        TID *id = montar_id(nome, anoNasc, rank);
        preenche_vetId(vet_ids, id, &pos); //preenche o vetor com os ids
        //preparando dados para hash e árvore
        int ano = atoi(anoM);
        int idade = cal_idade(anoNasc);

        int rank_in_arv, melhor_rank;
        if(strcmp(rank, "00") == 0){
            rank_in_arv = -1;
            melhor_rank = -1;
        }else {
            rank_in_arv = atoi(rank);
            melhor_rank = atoi(anoRank);
        }
    
        //Preparando struct para árvore
        strcpy(jogador->id, id->id);
        strcpy(jogador->nome, nome);
        jogador->rank = rank_in_arv;
        jogador->morte = ano;
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
        TARVBM_insere("indice.bin", jogador, raiz_atual, t, tam_NI);

        //Inserindo nas hash de aposentados e na de jogador por país
        THAN_insere("hashAposentado.bin", "dadosAposentado.bin", id->id, idade, ano); 
        THN_insere("hashPais.bin", "dadosPais.bin", id->id,  pais);

        free(jogador);
        result = fgets(linha, sizeof(linha), fp); //passa para a proxima linha

    }

    //Tabela de ID por sobrenome
    ordenar_id(vet_ids, pos);

    //Tabela de sobrenome por ID
    ordenar_sob(vet_ids, pos);

    libera_id(vet_ids); 
    free(id); // libera a posição do id
    fclose(fp);

}

void iniciar(int t, char *raiz_atual, char *primfolha){

    int tam_NI = tam_bloco(t);

    //inicializar todas as hashs
    hash_inicializar();

    /********* MAPEANDO NASCIONALIDADES  *************/
    //declarando um ponteiro para um vetor de ponteiros
    
    char **vet_paises = (char**) malloc(sizeof(char*) * 200);
    for (int i = 0; i < 200; i++) {
        vet_paises[i] = NULL; //tentativa de resolver um único erro eterno no querido valgrind
    }

    ler_nacionalidades(vet_paises);

    for (int i = 0; i < 200; i++){
        if (vet_paises[i]) free(vet_paises[i]); //libera a memória alocada para cada país
        else break; //se não tiver mais países, sai do loop
    }
    free(vet_paises);

    /****************************************************/

    //ler arquivos
    ler_tenistas(t, tam_NI, raiz_atual);
    ler_campeaos(raiz_atual, t);
    mapear_pontuacoes(raiz_atual, primfolha, t);
    ordenar_3("arqPontuacao.bin");
    

}

