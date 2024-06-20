#ifndef STRUCT_H
#define STRUCT_H

/*
Memória Cache
ok - Política de escrita: 0 - write-through e 1 - write-back;
Tamanho da linha: deve ser potência de 2, em bytes;
ok - Número de linhas: deve ser potência de 2;
ok - Associatividade (número de linhas) por conjunto: deve ser potência de 2 (mínimo 1 e máximo igual ao número de linhas);
Tempo de acesso quando encontra (hit time): em nanosegundos;
Política de Substituição: LFU (Least Frequently Used), LRU (Least Recently Used) ou Aleatória;
Memória Principal
Tempos de leitura/escrita: em nanosegundos.
*/



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