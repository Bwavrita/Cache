# Simulador de Cache

Este projeto é um simulador de cache implementado em C. O simulador lê configurações de cache de um arquivo e realiza operações de leitura e escrita com base nos endereços fornecidos, aplicando políticas de substituição e escrita configuráveis.

## Estrutura do Projeto

- `estrutura.h`: Define as estruturas de dados utilizadas no projeto.
- `metodos.h`: Contém as funções auxiliares para manipulação das estruturas de dados.
- `main.c`: Contém a função principal e a lógica para inicialização e execução do simulador.

## Funcionalidades

- **Inicialização da cache**: A partir de um arquivo de configuração.
- **Operações de leitura e escrita**: Com políticas configuráveis de substituição e escrita.
- **Coleta de estatísticas**: Incluindo taxa de acertos (hit rate) e tempo médio de acesso.
- **Saída de resultados**: Exibe e salva as estatísticas em um arquivo de saída.

## Estruturas de Dados

- `CacheConfig`: Configurações da cache.
- `LinhaCache`: Representa uma linha na cache.
- `Estatisticas`: Mantém estatísticas da cache.
- `Endereco`: Detalhes sobre o endereço acessado.
- `Cache`: Estrutura principal que combina todas as informações.



## Arquivos de Entrada

### config.txt

Este arquivo deve conter as configurações da cache em formato específico:

```plaintext
32  // largura da linha
1024 // número de linhas
4 // associatividade
1 // política de escrita (0 - write through / 1 - write back)
LRU // política de substituição (LRU, LFU, ALEATORIA)
