#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "operacao.h"

Cache* inicializar(FILE* f) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    fscanf(f, "%d", &cache->config.tamanhoLinha);
    fscanf(f, "%d", &cache->config.numeroConjuntos);
    fscanf(f, "%d", &cache->config.associatividade);
    fscanf(f, "%d", &cache->config.escrita);
    fscanf(f, "%s", cache->config.substituicao);

    cache->v = (LinhaCache**)malloc(cache->config.numeroConjuntos * sizeof(LinhaCache*));
    for (int i = 0; i < cache->config.numeroConjuntos; i++) {
        cache->v[i] = (LinhaCache*)malloc(cache->config.associatividade * sizeof(LinhaCache));
        for (int j = 0; j < cache->config.associatividade; j++) {
            cache->v[i][j].tag = -1;
            cache->v[i][j].dados = (char*)malloc(cache->config.tamanhoLinha * sizeof(char));
            cache->v[i][j].contadorUsos = 0;
            cache->v[i][j].tempoAcesso = 0;
            cache->v[i][j].sujo = 0;
        }
    }

    cache->est.totalEnderecos = 0;
    cache->est.leituras = 0;
    cache->est.escrita = 0;
    cache->est.hitLeitura = 0;
    cache->est.hitEscrita = 0;

    largurasEndereco(cache);
    return cache;
}

void liberarCache(Cache* cache) {
    for (int i = 0; i < cache->config.numeroConjuntos; i++) {
        for (int j = 0; j < cache->config.associatividade; j++) {
            free(cache->v[i][j].dados);
        }
        free(cache->v[i]);
    }
    free(cache->v);
    free(cache);
}

void acharCache(int endereco, char operacao, Cache* cache) {
    int hit = 0;
    int leitura = 0, escrita = 0;

    if (operacao == 'R') {
        hit = readWrite(cache, endereco, 0, &leitura, &escrita);
        cache->est.totalEnderecos++;
        cache->est.leituras += leitura;
        if (hit) {
            cache->est.hitLeitura++;
        }
    } else if (operacao == 'W') {
        hit = readWrite(cache, endereco, 1, &leitura, &escrita);
        cache->est.totalEnderecos++;
        cache->est.leituras += leitura;
        cache->est.escrita += escrita;
        if (hit) {
            cache->est.hitEscrita++;
        }
    }
}

int main() {
    FILE* f = fopen("config.txt", "r");
    if (!f) {
        perror("Erro ao abrir o arquivo de configuração");
        return 1;
    }
    Cache* c = inicializar(f);
    fclose(f);

    FILE* k = fopen("teste.txt", "r");
    if (!k) {
        printf("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    char operacao;
    int endereco;
    while (fscanf(k, "%x %c", &endereco, &operacao) == 2) {
        acharCache(endereco, operacao, c);
    }
    fclose(k);

    escritaWriteBack(c);

    FILE* saida = fopen("resultados.txt", "w");
    if (!saida) {
        printf("Erro ao criar o arquivo de saída");
        return 1;
    }

    fprintf(saida, "Total de endereços acessados: %d\n", c->est.totalEnderecos);
    fprintf(saida, "Total de leituras: %d\n", c->est.leituras);
    fprintf(saida, "Total de escritas: %d\n", c->est.escrita);
    fprintf(saida, "Hits de leitura: %d\n", c->est.hitLeitura);
    fprintf(saida, "Hits de escrita: %d\n", c->est.hitEscrita);
    fclose(saida);

    liberarCache(c);
    return 0;
}
