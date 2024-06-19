#ifndef OPERACAO_H
#define OPERACAO_H

#include "struct.h"
#include <stdio.h>
#include <string.h>

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

// Verifica se há um miss na cache
int miss(int endereco, Cache* c, int* hitIndex) {
    int tag = obterTag(endereco, c);
    int indice = obterIndice(endereco, c);
    for (int i = 0; i < c->config.associatividade; i++) {
        LinhaCache temp = c->v[indice][i];
        if (temp.tag == tag) {
            *hitIndex = i;
            return 1; // hit
        }
    }
    return 0; // miss
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

// Busca o bloco para substituição
int buscaBlocoSubstituicao(Cache* c, int endereco) {
    int menosUtilizado = 0;
    int indice = obterIndice(endereco, c);

    for (int i = 0; i < c->config.associatividade; i++) {
        if (c->v[indice][i].contadorUsos < c->v[indice][menosUtilizado].contadorUsos) {
            menosUtilizado = i;
        }
    }
    return menosUtilizado;
}

void substitui(Cache* c, int endereco) {
    int indice = obterIndice(endereco, c);
    int coluna = buscaBlocoSubstituicao(c, endereco);

    // Verificar se o bloco que está sendo substituído está sujo
    if (c->v[indice][coluna].sujo == 1) {
        c->est.escrita++;
    }

    // Atualizar o bloco substituído
    c->v[indice][coluna].tag = obterTag(endereco, c);
    c->v[indice][coluna].tempoAcesso = 0;
    c->v[indice][coluna].sujo = 0;
    c->v[indice][coluna].tempoAcesso = 0;

    // Atualizar os blocos adjacentes
    for (int k = 0; k < c->config.associatividade; k++) {
        if (k != coluna) {
            c->v[indice][k].tempoAcesso++;
        }
    }
}

// Verifica se o dado está na cache e atualiza as estatísticas
void acharCache(int endereco, char operacao, Cache* c) {
    int hitIndex = -1;
    int hit = miss(endereco, c, &hitIndex);
    int indice = obterIndice(endereco, c);

    if (hit) {
        if (operacao == 'W') {
            c->est.hitEscrita++;
            if (c->config.escrita == 0) {
                c->est.escrita++;
            } else {
                c->v[indice][hitIndex].sujo = 1;
            }
        } else {
            c->est.leituras++;
            c->est.hitLeitura++;
        }
        c->v[indice][hitIndex].contadorUsos++;
        c->v[indice][hitIndex].tempoAcesso = 0;
        for (int k = 0; k < c->config.associatividade; k++) {
            if (k != hitIndex) {
                c->v[indice][k].tempoAcesso++;
            }
        }
    } else {
        if (operacao == 'W') {
            c->est.escrita++;
        } else {
            c->est.leituras++;
        }
        substitui(c, endereco);
    }

    c->est.totalEnderecos++;
}

#endif
