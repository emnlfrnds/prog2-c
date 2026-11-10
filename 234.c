#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_CHAVES 3
#define MAX_FILHOS 4

// Estrutura do Nó da Árvore 2-3-4
typedef struct No {
    int chaves[MAX_CHAVES];
    struct No *filhos[MAX_FILHOS];
    int num_chaves;
    bool is_folha;
} No;

// Protótipos das Funções
No *criar_no(bool is_folha);
void dividir_filho(No *pai, int i, No *filho);
No *inserir(No *raiz, int chave);
bool buscar(No *raiz, int chave);
void em_ordem(No *raiz);
void exibir_por_niveis(No *raiz);
void liberar_arvore(No *raiz);

// Funções Auxiliares de Remoção
void remover(No **raiz, int chave);
void remover_no(No *no, int chave);
int obter_predecessor(No *no, int idx);
int obter_sucessor(No *no, int idx);
void preencher(No *no, int idx);
void pegar_emprestado_anterior(No *no, int idx);
void pegar_emprestado_seguinte(No *no, int idx);
void fundir(No *no, int idx);

// CRIAÇÃO E ALOCAÇÃO

No *criar_no(bool is_folha) {
    No *novo = (No *)malloc(sizeof(No));
    if (!novo) {
        printf("Erro de alocacao de memoria!\n");
        exit(EXIT_FAILURE);
    }
    novo->num_chaves = 0;
    novo->is_folha = is_folha;
    for (int i = 0; i < MAX_FILHOS; i++) {
        novo->filhos[i] = NULL;
    }
    return novo;
}

// INSERÇÃO COM DIVISÃO ANTECIPADA (Top-Down Split)



void dividir_filho(No *pai, int i, No *filho) {
    // Cria o novo nó que armazenará as chaves do lado direito do nó cheio
    No *novo_no = criar_no(filho->is_folha);
    novo_no->num_chaves = 1;

    // A chave do meio (índice 1) sobe para o pai
    // A chave da direita (índice 2) passa para o novo nó
    novo_no->chaves[0] = filho->chaves[2];

    // Se não for folha, transfere também os 2 últimos filhos
    if (!filho->is_folha) {
        novo_no->filhos[0] = filho->filhos[2];
        novo_no->filhos[1] = filho->filhos[3];
        filho->filhos[2] = NULL;
        filho->filhos[3] = NULL;
    }

    filho->num_chaves = 1;

    // Abre espaço no pai para o novo filho
    for (int j = pai->num_chaves; j >= i + 1; j--) {
        pai->filhos[j + 1] = pai->filhos[j];
    }
    pai->filhos[i + 1] = novo_no;

    // Abre espaço no pai para a chave promovida
    for (int j = pai->num_chaves - 1; j >= i; j--) {
        pai->chaves[j + 1] = pai->chaves[j];
    }
    pai->chaves[i] = filho->chaves[1];
    pai->num_chaves++;
}

No *inserir(No *raiz, int chave) {
    if (raiz == NULL) {
        raiz = criar_no(true);
        raiz->chaves[0] = chave;
        raiz->num_chaves = 1;
        return raiz;
    }

    // Se a raiz está cheia (4-node), ela é dividida e uma nova raiz surge
    if (raiz->num_chaves == MAX_CHAVES) {
        No *nova_raiz = criar_no(false);
        nova_raiz->filhos[0] = raiz;
        dividir_filho(nova_raiz, 0, raiz);

        int i = 0;
        if (nova_raiz->chaves[0] < chave) {
            i++;
        }
        // Continua a inserção a partir da nova raiz
        No *atual = nova_raiz;
        while (!atual->is_folha) {
            if (atual->filhos[i]->num_chaves == MAX_CHAVES) {
                dividir_filho(atual, i, atual->filhos[i]);
                if (atual->chaves[i] < chave) {
                    i++;
                }
            }
            atual = atual->filhos[i];
            i = 0;
            while (i < atual->num_chaves && chave > atual->chaves[i]) {
                i++;
            }
        }

        // Insere na folha
        if (i < atual->num_chaves && atual->chaves[i] == chave) return nova_raiz; // Ignora duplicados
        for (int j = atual->num_chaves - 1; j >= i; j--) {
            atual->chaves[j + 1] = atual->chaves[j];
        }
        atual->chaves[i] = chave;
        atual->num_chaves++;
        return nova_raiz;
    }

    // Descida preemptiva dividindo nós cheios
    No *atual = raiz;
    while (!atual->is_folha) {
        int i = 0;
        while (i < atual->num_chaves && chave > atual->chaves[i]) {
            i++;
        }

        if (i < atual->num_chaves && atual->chaves[i] == chave) {
            return raiz; // Ignora duplicados
        }

        if (atual->filhos[i]->num_chaves == MAX_CHAVES) {
            dividir_filho(atual, i, atual->filhos[i]);
            if (chave > atual->chaves[i]) {
                i++;
            } else if (chave == atual->chaves[i]) {
                return raiz; // Ignora duplicados
            }
        }
        atual = atual->filhos[i];
    }

    // Chegou na folha: insere a chave na posição correta
    int i = 0;
    while (i < atual->num_chaves && chave > atual->chaves[i]) {
        i++;
    }

    if (i < atual->num_chaves && atual->chaves[i] == chave) {
        return raiz; // Ignora duplicados
    }

    for (int j = atual->num_chaves - 1; j >= i; j--) {
        atual->chaves[j + 1] = atual->chaves[j];
    }
    atual->chaves[i] = chave;
    atual->num_chaves++;

    return raiz;
}

// BUSCA

bool buscar(No *raiz, int chave) {
    if (raiz == NULL) return false;

    int i = 0;
    while (i < raiz->num_chaves && chave > raiz->chaves[i]) {
        i++;
    }

    if (i < raiz->num_chaves && chave == raiz->chaves[i]) {
        return true;
    }

    if (raiz->is_folha) {
        return false;
    }

    return buscar(raiz->filhos[i], chave);
}

// PERCURSO EM ORDEM

void em_ordem(No *raiz) {
    if (raiz != NULL) {
        int i;
        for (i = 0; i < raiz->num_chaves; i++) {
            if (!raiz->is_folha) {
                em_ordem(raiz->filhos[i]);
            }
            printf("%d ", raiz->chaves[i]);
        }
        if (!raiz->is_folha) {
            em_ordem(raiz->filhos[i]);
        }
    }
}

// EXIBIÇÃO POR NÍVEIS

void exibir_por_niveis(No *raiz) {
    if (raiz == NULL) {
        printf("Arvore vazia.\n");
        return;
    }

    // Fila simples para travessia em largura (BFS)
    No *fila[1000];
    int nivel[1000];
    int inicio = 0, fim = 0;

    fila[fim] = raiz;
    nivel[fim] = 0;
    fim++;

    int nivel_atual = 0;
    printf("\n--- Nivel %d ---\n", nivel_atual);

    while (inicio < fim) {
        No *no_atual = fila[inicio];
        int n_atual = nivel[inicio];
        inicio++;

        if (n_atual > nivel_atual) {
            nivel_atual = n_atual;
            printf("\n--- Nivel %d ---\n", nivel_atual);
        }

        printf("[ ");
        for (int i = 0; i < no_atual->num_chaves; i++) {
            printf("%d ", no_atual->chaves[i]);
        }
        printf("] ");

        if (!no_atual->is_folha) {
            for (int i = 0; i <= no_atual->num_chaves; i++) {
                if (no_atual->filhos[i] != NULL) {
                    fila[fim] = no_atual->filhos[i];
                    nivel[fim] = n_atual + 1;
                    fim++;
                }
            }
        }
    }
    printf("\n");
}

// REMOÇÃO

int obter_predecessor(No *no, int idx) {
    No *atual = no->filhos[idx];
    while (!atual->is_folha) {
        atual = atual->filhos[atual->num_chaves];
    }
    return atual->chaves[atual->num_chaves - 1];
}

int obter_sucessor(No *no, int idx) {
    No *atual = no->filhos[idx + 1];
    while (!atual->is_folha) {
        atual = atual->filhos[0];
    }
    return atual->chaves[0];
}

void pegar_emprestado_anterior(No *no, int idx) {
    No *filho = no->filhos[idx];
    No *irmao = no->filhos[idx - 1];

    for (int i = filho->num_chaves - 1; i >= 0; i--) {
        filho->chaves[i + 1] = filho->chaves[i];
    }

    if (!filho->is_folha) {
        for (int i = filho->num_chaves; i >= 0; i--) {
            filho->filhos[i + 1] = filho->filhos[i];
        }
    }

    filho->chaves[0] = no->chaves[idx - 1];

    if (!filho->is_folha) {
        filho->filhos[0] = irmao->filhos[irmao->num_chaves];
    }

    no->chaves[idx - 1] = irmao->chaves[irmao->num_chaves - 1];

    filho->num_chaves++;
    irmao->num_chaves--;
}

void pegar_emprestado_seguinte(No *no, int idx) {
    No *filho = no->filhos[idx];
    No *irmao = no->filhos[idx + 1];

    filho->chaves[filho->num_chaves] = no->chaves[idx];

    if (!filho->is_folha) {
        filho->filhos[filho->num_chaves + 1] = irmao->filhos[0];
    }

    no->chaves[idx] = irmao->chaves[0];

    for (int i = 1; i < irmao->num_chaves; i++) {
        irmao->chaves[i - 1] = irmao->chaves[i];
    }

    if (!irmao->is_folha) {
        for (int i = 1; i <= irmao->num_chaves; i++) {
            irmao->filhos[i - 1] = irmao->filhos[i];
        }
    }

    filho->num_chaves++;
    irmao->num_chaves--;
}

void fundir(No *no, int idx) {
    No *filho = no->filhos[idx];
    No *irmao = no->filhos[idx + 1];

    filho->chaves[1] = no->chaves[idx];

    for (int i = 0; i < irmao->num_chaves; i++) {
        filho->chaves[i + 2] = irmao->chaves[i];
    }

    if (!filho->is_folha) {
        for (int i = 0; i <= irmao->num_chaves; i++) {
            filho->filhos[i + 2] = irmao->filhos[i];
        }
    }

    for (int i = idx + 1; i < no->num_chaves; i++) {
        no->chaves[i - 1] = no->chaves[i];
    }

    for (int i = idx + 2; i <= no->num_chaves; i++) {
        no->filhos[i - 1] = no->filhos[i];
    }

    filho->num_chaves += irmao->num_chaves + 1;
    no->num_chaves--;

    free(irmao);
}

void preencher(No *no, int idx) {
    if (idx != 0 && no->filhos[idx - 1]->num_chaves > 1) {
        pegar_emprestado_anterior(no, idx);
    } else if (idx != no->num_chaves && no->filhos[idx + 1]->num_chaves > 1) {
        pegar_emprestado_seguinte(no, idx);
    } else {
        if (idx != no->num_chaves) {
            fundir(no, idx);
        } else {
            fundir(no, idx - 1);
        }
    }
}

void remover_no(No *no, int chave) {
    int idx = 0;
    while (idx < no->num_chaves && no->chaves[idx] < chave) {
        idx++;
    }

    if (idx < no->num_chaves && no->chaves[idx] == chave) {
        if (no->is_folha) {
            for (int i = idx + 1; i < no->num_chaves; i++) {
                no->chaves[i - 1] = no->chaves[i];
            }
            no->num_chaves--;
        } else {
            if (no->filhos[idx]->num_chaves > 1) {
                int pred = obter_predecessor(no, idx);
                no->chaves[idx] = pred;
                remover_no(no->filhos[idx], pred);
            } else if (no->filhos[idx + 1]->num_chaves > 1) {
                int suc = obter_sucessor(no, idx);
                no->chaves[idx] = suc;
                remover_no(no->filhos[idx + 1], suc);
            } else {
                fundir(no, idx);
                remover_no(no->filhos[idx], chave);
            }
        }
    } else {
        if (no->is_folha) {
            printf("Chave %d nao encontrada na arvore.\n", chave);
            return;
        }

        bool flag = (idx == no->num_chaves);

        if (no->filhos[idx]->num_chaves < 2) {
            preencher(no, idx);
        }

        if (flag && idx > no->num_chaves) {
            remover_no(no->filhos[idx - 1], chave);
        } else {
            remover_no(no->filhos[idx], chave);
        }
    }
}

void remover(No **raiz, int chave) {
    if (*raiz == NULL) {
        printf("Arvore vazia.\n");
        return;
    }

    remover_no(*raiz, chave);

    if ((*raiz)->num_chaves == 0) {
        No *tmp = *raiz;
        if ((*raiz)->is_folha) {
            *raiz = NULL;
        } else {
            *raiz = (*raiz)->filhos[0];
        }
        free(tmp);
    }
}

// LIBERAÇÃO DE MEMÓRIA

void liberar_arvore(No *raiz) {
    if (raiz != NULL) {
        if (!raiz->is_folha) {
            for (int i = 0; i <= raiz->num_chaves; i++) {
                liberar_arvore(raiz->filhos[i]);
            }
        }
        free(raiz);
    }
}

// MENU INTERATIVO

int main() {
    No *raiz = NULL;
    int opcao, valor;

    do {
        printf("\n=============================\n");
        printf("       ARVORE 2-3-4\n");
        printf("=============================\n");
        printf("1 - Inserir valor\n");
        printf("2 - Buscar valor\n");
        printf("3 - Remover valor\n");
        printf("4 - Percorrer em ordem\n");
        printf("5 - Exibir arvore por niveis\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida!\n");
            break;
        }

        switch (opcao) {
            case 1:
                printf("Digite o valor a ser inserido: ");
                scanf("%d", &valor);
                raiz = inserir(raiz, valor);
                printf("Valor %d inserido com sucesso.\n", valor);
                break;

            case 2:
                printf("Digite o valor a ser buscado: ");
                scanf("%d", &valor);
                if (buscar(raiz, valor)) {
                    printf("O valor %d ESTA presente na arvore.\n", valor);
                } else {
                    printf("O valor %d NAO esta presente na arvore.\n", valor);
                }
                break;

            case 3:
                printf("Digite o valor a ser removido: ");
                scanf("%d", &valor);
                remover(&raiz, valor);
                break;

            case 4:
                printf("Percurso em ordem: ");
                if (raiz == NULL) {
                    printf("Arvore vazia.");
                } else {
                    em_ordem(raiz);
                }
                printf("\n");
                break;

            case 5:
                printf("Exibicao da arvore por niveis:");
                exibir_por_niveis(raiz);
                break;

            case 0:
                printf("Encerrando o programa e liberando memoria...\n");
                liberar_arvore(raiz);
                raiz = NULL;
                printf("Memoria liberada com sucesso!\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}