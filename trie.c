#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define TAM_ALFABETO 26
#define MAX_PALAVRA 100

// Trie para palavras com letras de 'a' a 'z' (maiusculas sao convertidas)

typedef struct No {
    struct No *filhos[TAM_ALFABETO];
    bool fim_palavra;
} No;

// Protótipos
No *criar_no(void);
bool inserir(No *raiz, const char *palavra);
bool buscar(No *raiz, const char *palavra);
bool remover(No *raiz, const char *palavra);
void exibir(No *raiz);
void em_ordem(No *no, char *buffer, int nivel);
void pre_ordem(No *no, char *buffer, int nivel);
void pos_ordem(No *no, char *buffer, int nivel);
void liberar_trie(No *no);

// Funções auxiliares
static bool normalizar(char *palavra);
static bool no_vazio(No *no);
static bool remover_rec(No *no, const char *palavra, int nivel, bool *removido);
static void exibir_rec(No *no, int nivel);
static void imprimir_palavra(No *no, char *buffer, int nivel);

// CRIAÇÃO

No *criar_no(void) {
    No *novo = (No *)malloc(sizeof(No));
    if (!novo) {
        printf("Erro de alocacao de memoria!\n");
        exit(EXIT_FAILURE);
    }
    novo->fim_palavra = false;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        novo->filhos[i] = NULL;
    }
    return novo;
}

// Converte para minusculas e valida; retorna false se houver caractere invalido
static bool normalizar(char *palavra) {
    if (palavra[0] == '\0') return false;
    for (int i = 0; palavra[i] != '\0'; i++) {
        unsigned char c = (unsigned char)palavra[i];
        if (!isalpha(c) || c > 127) return false;
        palavra[i] = (char)tolower(c);
    }
    return true;
}

static bool no_vazio(No *no) {
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (no->filhos[i] != NULL) return false;
    }
    return true;
}

// INSERÇÃO

// Retorna false se a palavra ja existia
bool inserir(No *raiz, const char *palavra) {
    No *atual = raiz;
    for (int i = 0; palavra[i] != '\0'; i++) {
        int idx = palavra[i] - 'a';
        if (atual->filhos[idx] == NULL) {
            atual->filhos[idx] = criar_no();
        }
        atual = atual->filhos[idx];
    }
    if (atual->fim_palavra) {
        return false;
    }
    atual->fim_palavra = true;
    return true;
}

// BUSCA

bool buscar(No *raiz, const char *palavra) {
    No *atual = raiz;
    for (int i = 0; palavra[i] != '\0'; i++) {
        atual = atual->filhos[palavra[i] - 'a'];
        if (atual == NULL) return false;
    }
    return atual->fim_palavra;
}

// REMOÇÃO

// Desce ate o fim da palavra e, na volta da recursao, libera os nos que
// ficaram sem filhos e que nao marcam o fim de outra palavra.
// Retorna true se o no atual pode ser liberado pelo pai.
static bool remover_rec(No *no, const char *palavra, int nivel, bool *removido) {
    if (palavra[nivel] == '\0') {
        if (!no->fim_palavra) return false;
        no->fim_palavra = false;
        *removido = true;
        return no_vazio(no);
    }

    int idx = palavra[nivel] - 'a';
    No *filho = no->filhos[idx];
    if (filho == NULL) return false;

    if (remover_rec(filho, palavra, nivel + 1, removido)) {
        free(filho);
        no->filhos[idx] = NULL;
        return !no->fim_palavra && no_vazio(no);
    }
    return false;
}

bool remover(No *raiz, const char *palavra) {
    bool removido = false;
    remover_rec(raiz, palavra, 0, &removido); // a raiz nunca e liberada
    return removido;
}

// PERCURSOS
// O buffer guarda o prefixo (caminho da raiz ate o no); ao visitar um no que
// termina uma palavra, o prefixo completo e impresso.
//   Pre-ordem: visita o no e depois os filhos (a-z)
//   Pos-ordem: visita os filhos (a-z) e depois o no
//   Em ordem : visita o primeiro filho, depois o no, depois os demais filhos

static void imprimir_palavra(No *no, char *buffer, int nivel) {
    if (no->fim_palavra) {
        buffer[nivel] = '\0';
        printf("%s\n", buffer);
    }
}

void pre_ordem(No *no, char *buffer, int nivel) {
    imprimir_palavra(no, buffer, nivel);
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (no->filhos[i] != NULL) {
            buffer[nivel] = (char)('a' + i);
            pre_ordem(no->filhos[i], buffer, nivel + 1);
        }
    }
}

void pos_ordem(No *no, char *buffer, int nivel) {
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (no->filhos[i] != NULL) {
            buffer[nivel] = (char)('a' + i);
            pos_ordem(no->filhos[i], buffer, nivel + 1);
        }
    }
    imprimir_palavra(no, buffer, nivel);
}

void em_ordem(No *no, char *buffer, int nivel) {
    bool primeiro = true;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (no->filhos[i] != NULL) {
            buffer[nivel] = (char)('a' + i);
            em_ordem(no->filhos[i], buffer, nivel + 1);
            if (primeiro) {
                imprimir_palavra(no, buffer, nivel);
                primeiro = false;
            }
        }
    }
    if (primeiro) { // no folha: nao ha filhos, so visita o proprio no
        imprimir_palavra(no, buffer, nivel);
    }
}

// EXIBIÇÃO

// Mostra a trie indentada; '*' marca o fim de uma palavra
static void exibir_rec(No *no, int nivel) {
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (no->filhos[i] != NULL) {
            for (int j = 0; j < nivel; j++) {
                printf("  ");
            }
            printf("%c%s\n", 'a' + i, no->filhos[i]->fim_palavra ? "*" : "");
            exibir_rec(no->filhos[i], nivel + 1);
        }
    }
}

void exibir(No *raiz) {
    if (no_vazio(raiz)) {
        printf("Trie vazia.\n");
        return;
    }
    printf("(raiz)\n");
    exibir_rec(raiz, 1);
    printf("(* = fim de palavra)\n");
}

// LIBERAÇÃO DE MEMÓRIA

void liberar_trie(No *no) {
    if (no == NULL) return;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        liberar_trie(no->filhos[i]);
    }
    free(no);
}

// MENU

// Le uma palavra, valida e normaliza. Retorna false em EOF ou entrada invalida.
static bool ler_palavra(const char *mensagem, char *palavra) {
    printf("%s", mensagem);
    if (scanf("%99s", palavra) != 1) return false;
    if (!normalizar(palavra)) {
        printf("Palavra invalida! Use apenas letras de a a z.\n");
        return false;
    }
    return true;
}

static void percorrer(No *raiz, void (*percurso)(No *, char *, int)) {
    if (no_vazio(raiz)) {
        printf("Trie vazia.\n");
        return;
    }
    char buffer[MAX_PALAVRA + 1];
    percurso(raiz, buffer, 0);
}

int main() {
    No *raiz = criar_no();
    int opcao;
    char palavra[MAX_PALAVRA + 1];

    do {
        printf("\n===== TRIE =====\n");
        printf("1 - Inserir palavra\n");
        printf("2 - Buscar palavra\n");
        printf("3 - Remover palavra\n");
        printf("4 - Percorrer em ordem\n");
        printf("5 - Percorrer em pre-ordem\n");
        printf("6 - Percorrer em pos-ordem\n");
        printf("7 - Exibir trie\n");
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
                if (!ler_palavra("Palavra a inserir: ", palavra)) break;
                if (inserir(raiz, palavra)) {
                    printf("Palavra \"%s\" inserida.\n", palavra);
                } else {
                    printf("Palavra \"%s\" ja existe.\n", palavra);
                }
                break;
            case 2:
                if (!ler_palavra("Palavra a buscar: ", palavra)) break;
                if (buscar(raiz, palavra)) {
                    printf("Palavra \"%s\" encontrada.\n", palavra);
                } else {
                    printf("Palavra \"%s\" nao encontrada.\n", palavra);
                }
                break;
            case 3:
                if (!ler_palavra("Palavra a remover: ", palavra)) break;
                if (remover(raiz, palavra)) {
                    printf("Palavra \"%s\" removida.\n", palavra);
                } else {
                    printf("Palavra \"%s\" nao encontrada.\n", palavra);
                }
                break;
            case 4:
                printf("Em ordem:\n");
                percorrer(raiz, em_ordem);
                break;
            case 5:
                printf("Pre-ordem:\n");
                percorrer(raiz, pre_ordem);
                break;
            case 6:
                printf("Pos-ordem:\n");
                percorrer(raiz, pos_ordem);
                break;
            case 7:
                exibir(raiz);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    liberar_trie(raiz);
    return 0;
}
