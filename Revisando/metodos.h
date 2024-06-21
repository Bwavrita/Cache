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
    int palavra = logDois(cache->config.larguraLinha);
    int index = logDois(cache->config.numeroConjuntos);   

    if (palavra == -1) {
        printf("ERRO: O tamanho do bloco da cache (%d) deve ser uma potência de dois.\n", cache->config.larguraLinha);
    } else {
        cache->end.palavra = palavra;
    }
    if (indice == -1) {
        printf("ERRO: O número de conjuntos na cache (%d) deve ser uma potência de dois.\n", cache->config.numeroConjuntos);
    } else {
        cache->end.index = index;
    }
}

// Realiza a escrita write-back na cache
void escritaWriteBack(Cache* c) {
	int i,j;
    for (i = 0; i < c->config.numeroConjuntos; i++) {
        for (j = 0; j < c->config.associatividade; j++) {
            if (c->v[i][j].sujo == 1) {
                c->est.escrita++;
                c->v[i][j].sujo = 0;
            }
        }
    }
}

void atualizaContadorUsos(Cache* cache, int indice, int coluna, int hit) {
    if (hit) {
        if (strcmp(cache->config.substituicao, "LRU") == 0) {
            cache->v[indice][coluna].contadorUsos = 0;
        } else if (strcmp(cache->config.substituicao, "LFU") == 0) {
            cache->v[indice][coluna].contadorUsos++;
        }
    } else {
        cache->v[indice][coluna].contadorUsos--;
    }
}

int buscaBlocoSubstituicao(Cache* cache, int indice) {
    int menosUtilizado = 0;
    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[indice][i].contadorUsos < cache->v[indice][menosUtilizado].contadorUsos) {
            menosUtilizado = i;
        }
    }
    return menosUtilizado;
}

void substitui(Cache* cache, int endereco) {
    int colunaSubstituicao = 0;
    int indice = obterIndice(endereco,cache);
    if (strcmp(cache->config.substituicao, "LRU") == 0 || strcmp(cache->config.substituicao, "LFU") == 0) {
        colunaSubstituicao = buscaBlocoSubstituicao(cache,indice);

        if (cache->config.escrita && cache->v[indice][colunaSubstituicao].sujo == 1) {
            cache->est.escrita++;
        }

        cache->v[indice][colunaSubstituicao].tag = obterTag(endereco, cache);
        cache->v[indice][colunaSubstituicao].contadorUsos = 0;
        cache->v[indice][colunaSubstituicao].sujo = 0;
    }
}

void acharCache(Cache* cache, int endereco) {
    int inserido = 0;
    int indice = obterIndice(endereco,cache);
    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[indice][i].tag == -1) {
            cache->v[indice][i].tag = obterTag(endereco, cache);
            cache->v[indice][i].contadorUsos = 0;
            inserido = 1;
            break;
        }
    }

    if (!inserido) {
        substitui(cache,endereco);
    }
}

int atualizarEscritaLeitura(Cache* cache, int endereco, int operation) {
    int hit = 0, coluna = 0;
    int indice = obterIndice(endereco, cache);

    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[indice][i].tag == obterTag(endereco, cache)) {
            hit = 1;
            coluna = i;
        }
        atualizaContadorUsos(cache,indice, i, hit);
    }

    if (!hit) {
        cache->est.leituras ++;
        acharCache(cache, endereco);
    }

    if (operation) {
        if (!cache->config.escrita) {
            cache->est.escrita ++;
        } else {
            cache->v[indice][coluna].sujo = 1;
        }
    }

    return hit;
}

#endif