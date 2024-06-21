#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
    int larguraLinha;
    int numeroLinhas;
    int numeroConjuntos;
    int associatividade;
    int escrita;
    char substituicao[4];
}CacheConfig;

typedef struct{
    int tag;
    char* dados;
    int contadorUsos;
    int tempoAcesso;
    int sujo;
}LinhaCache;

typedef struct{
    float totalEnderecos;
    int leituras;
    int escrita;
    int hitLeitura;
    int hitEscrita;
    float hitrate;
}Estatisticas;

typedef struct{
    int endereco;
    int linha;
    int indice;
}Endereco;

typedef struct{
    CacheConfig config;
    LinhaCache** v;
    Estatisticas est;
    Endereco end;
}Cache;

#endif