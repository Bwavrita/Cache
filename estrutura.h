#ifndef estrutura_H
#define estrutura_H

typedef struct {
    int larguraLinha; //tamanho da palavra
    int numeroLinhas; //qntd de linhas na cache
    int numeroConjuntos; //quantidade de conjuntos
    int associatividade; //quantos por conjunto
    int escrita; //politica de escrita (0 - write through / 1 - write back)
    char substituicao[4]; //metodo de escrita
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
    int palavra;
    int index;
    int tag;
}Endereco;

typedef struct{
    CacheConfig config;
    LinhaCache** v;
    Estatisticas est;
    Endereco end;
}Cache;

#endif