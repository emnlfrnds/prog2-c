#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAMANHO_TABELA 7

// Tabela hash com encadeamento separado: cada posicao (balde) guarda a raiz
// de uma arvore binaria de busca, o que permite percorrer em ordem,
// pre-ordem e pos-ordem os valores de cada balde.

typedef struct No {
    int chave;
    struct No *esq;
    struct No *dir;
} No;

typedef struct {
    No *baldes[TAMANHO_TABELA];
} TabelaHash;

// Protótipos
int funcao_hash(int chave);
No *criar_no(int chave);
void inicializar(TabelaHash *tabela);
bool inserir(TabelaHash *tabela, int chave);
bool buscar(TabelaHash *tabela, int chave);
bool remover(TabelaHash *tabela, int chave);
void exibir(TabelaHash *tabela);
void percorrer(TabelaHash *tabela, void (*percurso)(No *));
void em_ordem(No *raiz);
void pre_ordem(No *raiz);
void pos_ordem(No *raiz);
void liberar_tabela(TabelaHash *tabela);

// Funções auxiliares da árvore de cada balde
static No *inserir_no(No *raiz, int chave, bool *inserido);
static No *remover_no(No *raiz, int chave, bool *removido);
static void exibir_arvore(No *raiz, int nivel);
static void liberar_arvore(No *raiz);

// CRIAÇÃO E FUNÇÃO HASH

// Método da divisão; trata chaves negativas para o índice nunca ser negativo
int funcao_hash(int chave) {
    int indice = chave % TAMANHO_TABELA;
    return indice < 0 ? indice + TAMANHO_TABELA : indice;
}

No *criar_no(int chave) {
    No *novo = (No *)malloc(sizeof(No));
    if (!novo) {
        printf("Erro de alocacao de memoria!\n");
        exit(EXIT_FAILURE);
    }
    novo->chave = chave;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

void inicializar(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabela->baldes[i] = NULL;
    }
}

// INSERÇÃO

static No *inserir_no(No *raiz, int chave, bool *inserido) {
    if (raiz == NULL) {
        *inserido = true;
        return criar_no(chave);
    }
    if (chave < raiz->chave) {
        raiz->esq = inserir_no(raiz->esq, chave, inserido);
    } else if (chave > raiz->chave) {
        raiz->dir = inserir_no(raiz->dir, chave, inserido);
    }
    // chave igual: duplicata, *inserido continua false
    return raiz;
}

bool inserir(TabelaHash *tabela, int chave) {
    int indice = funcao_hash(chave);
    bool inserido = false;
    tabela->baldes[indice] = inserir_no(tabela->baldes[indice], chave, &inserido);
    return inserido;
}

// BUSCA

bool buscar(TabelaHash *tabela, int chave) {
    No *atual = tabela->baldes[funcao_hash(chave)];
    while (atual != NULL) {
        if (chave == atual->chave) {
            return true;
        }
        atual = chave < atual->chave ? atual->esq : atual->dir;
    }
    return false;
}

// REMOÇÃO

static No *remover_no(No *raiz, int chave, bool *removido) {
    if (raiz == NULL) {
        return NULL;
    }
    if (chave < raiz->chave) {
        raiz->esq = remover_no(raiz->esq, chave, removido);
    } else if (chave > raiz->chave) {
        raiz->dir = remover_no(raiz->dir, chave, removido);
    } else {
        *removido = true;
        // Caso 1 e 2: zero ou um filho
        if (raiz->esq == NULL) {
            No *filho = raiz->dir;
            free(raiz);
            return filho;
        }
        if (raiz->dir == NULL) {
            No *filho = raiz->esq;
            free(raiz);
            return filho;
        }
        // Caso 3: dois filhos -> substitui pelo sucessor (menor da subarvore direita)
        No *sucessor = raiz->dir;
        while (sucessor->esq != NULL) {
            sucessor = sucessor->esq;
        }
        raiz->chave = sucessor->chave;
        bool ignorado = false;
        raiz->dir = remover_no(raiz->dir, sucessor->chave, &ignorado);
    }
    return raiz;
}

bool remover(TabelaHash *tabela, int chave) {
    int indice = funcao_hash(chave);
    bool removido = false;
    tabela->baldes[indice] = remover_no(tabela->baldes[indice], chave, &removido);
    return removido;
}

// PERCURSOS (aplicados a cada balde, do 0 ao ultimo)

void em_ordem(No *raiz) {
    if (raiz == NULL) return;
    em_ordem(raiz->esq);
    printf("%d ", raiz->chave);
    em_ordem(raiz->dir);
}

void pre_ordem(No *raiz) {
    if (raiz == NULL) return;
    printf("%d ", raiz->chave);
    pre_ordem(raiz->esq);
    pre_ordem(raiz->dir);
}

void pos_ordem(No *raiz) {
    if (raiz == NULL) return;
    pos_ordem(raiz->esq);
    pos_ordem(raiz->dir);
    printf("%d ", raiz->chave);
}

void percorrer(TabelaHash *tabela, void (*percurso)(No *)) {
    bool vazia = true;
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (tabela->baldes[i] != NULL) {
            vazia = false;
            printf("[%d]: ", i);
            percurso(tabela->baldes[i]);
            printf("\n");
        }
    }
    if (vazia) {
        printf("Tabela vazia.\n");
    }
}

// EXIBIÇÃO

// Imprime a arvore deitada: subarvore direita em cima, esquerda embaixo
static void exibir_arvore(No *raiz, int nivel) {
    if (raiz == NULL) return;
    exibir_arvore(raiz->dir, nivel + 1);
    for (int i = 0; i < nivel; i++) {
        printf("    ");
    }
    printf("%d\n", raiz->chave);
    exibir_arvore(raiz->esq, nivel + 1);
}

void exibir(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        printf("Balde [%d]:", i);
        if (tabela->baldes[i] == NULL) {
            printf(" (vazio)\n");
        } else {
            printf("\n");
            exibir_arvore(tabela->baldes[i], 1);
        }
    }
}

// LIBERAÇÃO DE MEMÓRIA

static void liberar_arvore(No *raiz) {
    if (raiz == NULL) return;
    liberar_arvore(raiz->esq);
    liberar_arvore(raiz->dir);
    free(raiz);
}

void liberar_tabela(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        liberar_arvore(tabela->baldes[i]);
        tabela->baldes[i] = NULL;
    }
}

// MENU

static bool ler_inteiro(const char *mensagem, int *valor) {
    printf("%s", mensagem);
    if (scanf("%d", valor) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        if (c == EOF) return false;
        printf("Entrada invalida!\n");
        return false;
    }
    return true;
}

int main() {
    TabelaHash tabela;
    inicializar(&tabela);
    int opcao, valor;

    do {
        printf("\n===== TABELA HASH (baldes com BST) =====\n");
        printf("1 - Inserir valor\n");
        printf("2 - Buscar valor\n");
        printf("3 - Remover valor\n");
        printf("4 - Percorrer em ordem\n");
        printf("5 - Percorrer em pre-ordem\n");
        printf("6 - Percorrer em pos-ordem\n");
        printf("7 - Exibir tabela\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            if (c == EOF) break;
            printf("Opcao invalida!\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                if (!ler_inteiro("Valor a inserir: ", &valor)) break;
                if (inserir(&tabela, valor)) {
                    printf("Valor %d inserido no balde %d.\n", valor, funcao_hash(valor));
                } else {
                    printf("Valor %d ja existe na tabela.\n", valor);
                }
                break;
            case 2:
                if (!ler_inteiro("Valor a buscar: ", &valor)) break;
                if (buscar(&tabela, valor)) {
                    printf("Valor %d encontrado no balde %d.\n", valor, funcao_hash(valor));
                } else {
                    printf("Valor %d nao encontrado.\n", valor);
                }
                break;
            case 3:
                if (!ler_inteiro("Valor a remover: ", &valor)) break;
                if (remover(&tabela, valor)) {
                    printf("Valor %d removido.\n", valor);
                } else {
                    printf("Valor %d nao encontrado.\n", valor);
                }
                break;
            case 4:
                printf("Em ordem (por balde):\n");
                percorrer(&tabela, em_ordem);
                break;
            case 5:
                printf("Pre-ordem (por balde):\n");
                percorrer(&tabela, pre_ordem);
                break;
            case 6:
                printf("Pos-ordem (por balde):\n");
                percorrer(&tabela, pos_ordem);
                break;
            case 7:
                exibir(&tabela);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    liberar_tabela(&tabela);
    return 0;
}
