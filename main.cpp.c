#include <stdio.h>
#include <stdlib.h>
#include "estrutura.h"
#include "metodos.h"

Cache* inicializar(FILE* f) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    fscanf(f, "%d", &cache->config.larguraLinha);
    fscanf(f, "%d", &cache->config.numeroLinhas);
    fscanf(f, "%d", &cache->config.associatividade);
    fscanf(f, "%d", &cache->config.escrita);
    fscanf(f, "%s", cache->config.substituicao);
    
    cache->config.numeroConjuntos = cache->config.numeroLinhas / cache->config.associatividade;

    cache->v = (LinhaCache**)malloc(cache->config.numeroConjuntos * sizeof(LinhaCache*));
    for (int i = 0; i < cache->config.numeroConjuntos; i++) {
        cache->v[i] = (LinhaCache*)malloc(cache->config.associatividade * sizeof(LinhaCache));
        for (int j = 0; j < cache->config.associatividade; j++) {
            cache->v[i][j].tag = -1;
            cache->v[i][j].dados = (char*)malloc(cache->config.larguraLinha * sizeof(char));
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
void leituraEnd(int endereco, char operacao, Cache* cache) {
    int hit = 0;
    if (operacao == 'R') {
        hit = atualizarEscritaLeitura(cache, endereco, 0);
        cache->est.totalEnderecos++;
        if (hit) {
            cache->est.hitLeitura++;
        }
    } else if (operacao == 'W') {
        hit = atualizarEscritaLeitura(cache, endereco, 1);
        cache->est.totalEnderecos++;
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

    FILE* k = fopen("oficial.txt", "r");
    if (!k) {
        printf("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    char operacao;
    int endereco;
    while (fscanf(k, "%x %c", &endereco, &operacao) == 2) {
        leituraEnd(endereco, operacao, c);
    }
    fclose(k);

    escritaWriteBack(c);

    FILE* saida = fopen("resultados.txt", "w");
    if (!saida) {
        printf("Erro ao criar o arquivo de saída");
        return 1;
    }
    c->est.hitrate = ((c->est.hitLeitura + c->est.hitEscrita)*100)/c->est.totalEnderecos;
    float temp = 10 + (1 - c->est.hitrate/100) * 60;

    fprintf(saida, "Tamanho da cache: %d\n", c->config.numeroLinhas*c->config.larguraLinha);
    fprintf(saida, "Total de endereços acessados: %2.f\n", c->est.totalEnderecos);
    fprintf(saida, "Taxa de hit: %.2f%%\n",c->est.hitrate);
    fprintf(saida, "Tempo médio: %.2fns\n",temp);
    fprintf(saida, "Leituras na Mp: %d\n", c->est.leituras);
    fprintf(saida, "Escritas na Mp: %d\n", c->est.escrita);
    fprintf(saida, "Hits de leitura: %d\n", c->est.hitLeitura);
    fprintf(saida, "Hits de escrita: %d\n", c->est.hitEscrita);
    fclose(saida);

    liberarCache(c);
    return 0;
}
