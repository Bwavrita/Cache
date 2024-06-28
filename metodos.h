#ifndef OPERACAO_H
#define OPERACAO_H

#include "estrutura.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

int logDois(int n) {
    int log = 0;
    if (n == 0) return -1;
    while ((n & 1) == 0) {
        n >>= 1;
        log++;
    }
    return (n == 1) ? log : -1;
}

int obterTag(int endereco, Cache* c) {
    // A tag é obtida deslocando o endereço para a direita pelo número de bits do índice e da palavra
    int deslocamento = c->end.index + c->end.palavra;
    //printf("desolocamento %d\n",deslocamento);
    return endereco >> deslocamento;//deslocamento;
}
int obterIndice(int endereco, Cache* c) {
    int mask = (1 << c->end.index) - 1;
    return (endereco >> c->end.palavra) & mask;
}

void largurasEndereco(Cache* cache) {
    int palavra = logDois(cache->config.larguraLinha);
    int index = logDois(cache->config.numeroConjuntos);

    if (palavra == -1) {
        printf("ERRO: O tamanho do bloco da cache (%d) deve ser uma potência de dois.\n", cache->config.larguraLinha);
    } else {
        cache->end.palavra = palavra;
    }
    if (index == -1) {
        printf("ERRO: O número de conjuntos na cache (%d) deve ser uma potência de dois.\n", cache->config.numeroConjuntos);
    } else {
        cache->end.index = index;
    }
}

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
            cache->v[indice][coluna].tempoAcesso++;
        } else if (strcmp(cache->config.substituicao, "LFU") == 0) {
            cache->v[indice][coluna].contadorUsos++;
        }
    }
}


int buscaBlocoSubstituicao(Cache* cache, int indice) {
    int blocoSubstituicao = 1;
    if (strcmp(cache->config.substituicao, "LRU") == 0) {
        for (int i = 0; i < cache->config.associatividade; i++) {
            if (cache->v[indice][i].tempoAcesso > cache->v[indice][blocoSubstituicao].tempoAcesso) {
                blocoSubstituicao = i;
            }
        }
    } else if (strcmp(cache->config.substituicao, "LFU") == 0) {
        for (int i = 0; i < cache->config.associatividade; i++) {
            if (cache->v[indice][i].contadorUsos > cache->v[indice][blocoSubstituicao].contadorUsos) {
                blocoSubstituicao = i;
            }
        }
    } else if (strcmp(cache->config.substituicao, "ALEATORIA") == 0) {
        blocoSubstituicao = rand() % cache->config.associatividade;
    }
    return blocoSubstituicao;
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
        cache->v[indice][colunaSubstituicao].tempoAcesso = 0;
        cache->v[indice][colunaSubstituicao].sujo = 0;
        cache->v[indice][colunaSubstituicao].endereco = endereco;
    }
}

void acharCache(Cache* cache, int endereco) {
    int inserido = 0;
    int indice = obterIndice(endereco,cache);
    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[indice][i].tag == -1) {
            cache->v[indice][i].tag = obterTag(endereco, cache);
            cache->v[indice][i].contadorUsos = 0;
            cache->v[indice][i].tempoAcesso = 0;
            cache->v[indice][i].endereco = endereco;
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
    int tag = obterTag(endereco, cache);
    printf("---------------------------------------\n");
    for (int i = 0; i < cache->config.associatividade; i++) {
        if (cache->v[indice][i].tag == tag){
            hit = 1;
            coluna = i;
            printf("Dado: %x , Endereco: %x , Indice dado: %x , indice endereco: %x\n",
                   cache->v[indice][i].endereco,endereco, obterIndice(cache->v[indice][i].endereco, cache), indice);
        }
        atualizaContadorUsos(cache, indice, i, hit);
    }

    if (!hit) {
        if (!operation) {
            cache->est.leituras++;
        } else if (operation) {
            cache->est.leituras++;
            if (cache->config.escrita) {
                cache->v[indice][coluna].sujo = 1;
            } else {
                cache->est.escrita++;
            }
        }
        acharCache(cache, endereco);
    }

    return hit;
}

#endif
