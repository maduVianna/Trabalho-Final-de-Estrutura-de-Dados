#include "mapeando_nascionalidade.h"


//função auxiliar para o qsort
int compara_paises(const void *p1, const void* p2){
    const char *cmp1 = *(const char **)p1;
    const char *cmp2 = *(const char **)p2;
    return strcmp(cmp1, cmp2);
}

int busca_vet(char ** vet_paises, char* pais, int tam){
    if(!vet_paises) return 0;
    int i=0;
    while(i<tam)
        if(!strcmp(vet_paises[i++], pais)) return 1;
    return 0;
}

void ler_nacionalidades(char** vet_paises) {
    char linha[65];
    
    FILE *fp = fopen("tennis_players.txt", "r"); //POSSÍVEL MODIFICAÇÕES
    if (!fp) {
        perror("Erro ao abrir tennis para escrita");
        exit(1);
    }

    char* result = fgets(linha, sizeof(linha), fp); //lê e ignora a primeira linha (porque é aquela q identifica os campos só)
    result = fgets(linha, sizeof(linha), fp); //lê a primeira linha com iformações de vdd
    int qntd_elem = 0;

    while (result != NULL) {
        int cont = 0;
        int inicio = -1, fim = -1;

        for (size_t i = 0; i < strlen(result); i++) {
            if (result[i] == '\\') { //a cada \ encontrada soma um (já que todas as informações estão entre \)
                cont++;
                if (cont == 3) inicio = i + 1; //quando achou a terceira barra já sabe q o próximo caracter é o primeiro do pais
                else if (cont == 4) {
                    fim = i; //se achou a 4 barra já leu tudo do pais
                    break;
                }
            }
        }

        if (inicio != -1 && fim != -1 && fim > inicio) {
            int tam = fim - inicio; //calcula o tamanho da string (qnts caracteres tem o país)
            char* pais = (char*) malloc(tam_pais); //aloca uma string com a qntd de caracteres do pais+1 (o \0)
            strncpy(pais, &result[inicio], tam_pais); //copia para pais a nacionalidade encontrada
            pais[tam] = '\0';

            //se a nacionalidade ainda não está no vetor países coloca
            if(!busca_vet(vet_paises, pais, qntd_elem))
                vet_paises[qntd_elem++] = pais;
            else
                free(pais);
        }

        result = fgets(linha, sizeof(linha), fp);
    }

    //fecha o arquivo .txt
    fclose(fp);

   //ordenando o vetor de países
    qsort(vet_paises, qntd_elem, sizeof(char*), compara_paises);

   
    //gravando os dados em arquivo binário com tamanho da string(qntd de caracteres do país), nome do país, índice
    FILE *fpbin = fopen("nacionalities_table.bin", "wb+");
    if (!fpbin) {
        perror("Erro ao abrir nacionalities_table.bin para escrita");
        exit(1);
    }

    for(int i=0; i<qntd_elem; i++){
        fwrite(vet_paises[i], sizeof(char),tam_pais, fpbin);
        fwrite(&i, sizeof(int), 1,fpbin);
    }
   
    fclose(fpbin);
}