#ifndef OPERACAO_H
#define OPERACAO_H

#include "struct.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Calcula log base 2 de um número n, retornando -1 se n não for potência de 2
int logDois(int n) {
    int log = 0;
    if (n == 0) return -1;
    while ((n & 1) == 0) {
        n >>= 1;
        log++;
    }
    return (n == 1) ? log : -1;
}

// Obtém a tag a partir de um endereço
int obterTag(int endereco, Cache* c) {
    int bits = c->end.linha + c->end.indice;
    return endereco >> bits;
}

// Obtém o índice a partir de um endereço
int obterIndice(int endereco, Cache* c) {
    int mask = (1 << c->end.indice) - 1;
    return (endereco >> c->end.linha) & mask;
}

// Calcula as larguras dos campos de endereço
void largurasEndereco(Cache* cache) {
    int largura = sizeof(void*) << 3;
    int linha = logDois(cache->config.tamanhoLinha);
    int indice = logDois(cache->config.numeroConjuntos);

    if (linha == -1) {
        printf("ERRO: O tamanho do bloco da cache (%d) deve ser uma potência de dois.\n", cache->config.tamanhoLinha);
    } else {
        cache->end.linha = linha;
    }
    if (indice == -1) {
        printf("ERRO: O número de conjuntos na cache (%d) deve ser uma potência de dois.\n", cache->config.numeroConjuntos);
    } else {
        cache->end.indice = indice;
    }
    if (linha != -1 && indice != -1) {
        cache->end.endereco = largura - (linha + indice);
    }
}

// Realiza a escrita write-back na cache
void escritaWriteBack(Cache* c) {
    for (int i = 0; i < c->config.numeroConjuntos; i++) {
        for (int j = 0; j < c->config.associatividade; j++) {
            if (c->v[i][j].sujo == 1) {
                c->est.escrita++;
                c->v[i][j].sujo = 0;
            }
        }
    }
}

void atualizaContadorUsos(Cache* cache, int linha, int coluna, int hit) {
    if (hit) {
        if (strcmp(cache->config.substituicao, "LRU") == 0) {
            cache->v[linha][coluna].contadorUsos = 0;
        } else if (strcmp(cache->config.substituicao, "LFU") == 0) {
            cache->v[linha][coluna].contadorUsos++;
        }
    } else {
        cache->v[linha][coluna].contadorUsos--;
    }
}

int buscaBlocoSubstituicao(Cache* cache, int linha) {
    int menosUtilizado = 0;
    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[linha][i].contadorUsos < cache->v[linha][menosUtilizado].contadorUsos) {
            menosUtilizado = i;
        }
    }
    return menosUtilizado;
}

void substitui(Cache* cache, int data, int linha) {
    int colunaSubstituicao = 0;

    if (strcmp(cache->config.substituicao, "LRU") == 0 || strcmp(cache->config.substituicao, "LFU") == 0) {
        colunaSubstituicao = buscaBlocoSubstituicao(cache, linha);

        if (cache->config.escrita && cache->v[linha][colunaSubstituicao].sujo == 1) {
            cache->est.escrita++;
        }

        cache->v[linha][colunaSubstituicao].tag = obterTag(data, cache);
        cache->v[linha][colunaSubstituicao].contadorUsos = 0;
        cache->v[linha][colunaSubstituicao].sujo = 0;
    }
}

void buscaBloco(Cache* cache, int data, int linha, int* escritas) {
    int inserido = 0;

    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[linha][i].tag == -1) {
            cache->v[linha][i].tag = obterTag(data, cache);
            cache->v[linha][i].contadorUsos = 0;
            inserido = 1;
            break;
        }
    }

    if (!inserido) {
        substitui(cache, data, linha);
    }
}

int readWrite(Cache* cache, int data, int operation, int* leituras, int* escritas) {
    int hit = 0, coluna = 0;
    int linha = obterIndice(data, cache);

    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[linha][i].tag == obterTag(data, cache)) {
            hit = 1;
            coluna = i;
        }
        atualizaContadorUsos(cache, linha, i, hit);
    }

    if (!hit) {
        *leituras += 1;
        buscaBloco(cache, data, linha, escritas);
    }

    if (operation) {
        if (!cache->config.escrita) {
            *escritas += 1;
        } else {
            cache->v[linha][coluna].sujo = 1;
        }
    }

    return hit;
}

#endif
