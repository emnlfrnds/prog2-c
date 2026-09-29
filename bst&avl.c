#include <stdio.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------
 * Arvore AVL (BST auto-balanceada) para armazenar numeros inteiros.
 * Implementa insercao, busca, remocao (folha / um filho / dois filhos),
 * os tres percursos classicos (pre-ordem, em ordem e pos-ordem) e a
 * visualizacao do balanceamento da arvore.
 *
 * Alem da propriedade da BST, a AVL garante que, para todo no, a
 * diferenca de altura entre as subarvores esquerda e direita (fator de
 * balanceamento) seja -1, 0 ou +1. Quando uma insercao ou remocao quebra
 * essa regra, a arvore e rebalanceada com rotacoes simples ou duplas.
 *
 * Valores repetidos: se o valor inserido ja existir na arvore, insec()
 * apenas avisa o usuario e nao cria um novo no (duplicados sao ignorados).
 * --------------------------------------------------------------------- */

typedef struct No {
    int valor;
    int altura; /* altura do no, contada em nos (NULL = 0, folha = 1) */
    struct No *esquerda, *direita;
} Arvore;

/* ------------------------- funcoes auxiliares AVL ------------------------- */

/* Retorna a altura de um no. Arvore vazia (NULL) tem altura 0. */
int altura(Arvore *no) {
    return no ? no->altura : 0;
}

int maior(int a, int b) {
    return (a > b) ? a : b;
}

/* Recalcula a altura do no a partir da altura dos filhos. */
void atualizarAltura(Arvore *no) {
    no->altura = 1 + maior(altura(no->esquerda), altura(no->direita));
}

/* Fator de balanceamento = altura(esquerda) - altura(direita).
 * Positivo: pesada a esquerda. Negativo: pesada a direita.
 * Em uma AVL valida, o valor fica sempre entre -1 e +1. */
int fatorBalanceamento(Arvore *no) {
    return no ? altura(no->esquerda) - altura(no->direita) : 0;
}

/* Rotacao simples a direita (caso Esquerda-Esquerda):
 *        y                x
 *       / \              / \
 *      x   C    ==>     A   y
 *     / \                  / \
 *    A   B                B   C
 * Retorna a nova raiz da subarvore (x). */
Arvore* rotacaoDireita(Arvore *y) {
    Arvore *x = y->esquerda;
    Arvore *b = x->direita;

    x->direita = y;
    y->esquerda = b;

    /* y agora e filho de x, entao a altura de y deve ser atualizada primeiro */
    atualizarAltura(y);
    atualizarAltura(x);
    return x;
}

/* Rotacao simples a esquerda (caso Direita-Direita): espelho da anterior.
 * Retorna a nova raiz da subarvore (y). */
Arvore* rotacaoEsquerda(Arvore *x) {
    Arvore *y = x->direita;
    Arvore *b = y->esquerda;

    y->esquerda = x;
    x->direita = b;

    atualizarAltura(x);
    atualizarAltura(y);
    return y;
}

/* Atualiza a altura do no e, se ele estiver desbalanceado, aplica a
 * rotacao adequada. Retorna a raiz (possivelmente nova) da subarvore.
 *   fb >  1 e filho esquerdo pesado a esquerda ou equilibrado -> rotacao direita          (LL)
 *   fb >  1 e filho esquerdo pesado a direita                 -> rotacao esq. + direita   (LR)
 *   fb < -1 e filho direito pesado a direita ou equilibrado   -> rotacao esquerda         (RR)
 *   fb < -1 e filho direito pesado a esquerda                 -> rotacao dir. + esquerda  (RL)
 * O caso "filho equilibrado (fb == 0)" so ocorre na remocao e tambem
 * e resolvido com rotacao simples. */
Arvore* balancear(Arvore *no) {
    int fb;

    atualizarAltura(no);
    fb = fatorBalanceamento(no);

    if (fb > 1) {
        if (fatorBalanceamento(no->esquerda) < 0) {
            no->esquerda = rotacaoEsquerda(no->esquerda); /* caso LR */
        }
        return rotacaoDireita(no);
    }

    if (fb < -1) {
        if (fatorBalanceamento(no->direita) > 0) {
            no->direita = rotacaoDireita(no->direita);    /* caso RL */
        }
        return rotacaoEsquerda(no);
    }

    return no; /* ja estava balanceado */
}

/* ------------------------------ operacoes ------------------------------ */

/* Insere um novo valor na arvore respeitando a propriedade da BST.
 * Se *raiz for NULL, cria o no ali. Caso contrario, desce recursivamente
 * para a esquerda (valores menores) ou direita (valores maiores) ate
 * achar uma posicao livre. Valores iguais a um no ja existente sao
 * ignorados, ou seja, a arvore nunca guarda duplicados.
 * Na volta da recursao, cada no do caminho tem a altura atualizada e e
 * rebalanceado se necessario. */
void insec(Arvore **raiz, int n) {
    if (*raiz == NULL) {
        *raiz = malloc(sizeof(Arvore));
        if (*raiz == NULL) {
            printf("erro: memoria insuficiente\n");
            return;
        }
        (*raiz)->valor = n;
        (*raiz)->altura = 1; /* no novo e sempre folha */
        (*raiz)->esquerda = NULL;
        (*raiz)->direita = NULL;
        return;
    }

    if (n < (*raiz)->valor) {
        insec(&((*raiz)->esquerda), n);
    } else if (n > (*raiz)->valor) {
        insec(&((*raiz)->direita), n);
    } else {
        printf("esse valor ja esta na arvore\n");
        return; /* duplicado: nada mudou, nao precisa rebalancear */
    }

    *raiz = balancear(*raiz);
}

/* Remove um valor da arvore e retorna a nova raiz da (sub)arvore.
 * Cobre os tres casos classicos de remocao em BST:
 *   1) no folha           -> so libera o no e retorna NULL
 *   2) no com um filho    -> o filho assume o lugar do no removido
 *   3) no com dois filhos -> troca o valor pelo antecessor in-order
 *                             (o maior valor da subarvore esquerda) e
 *                             remove o antecessor recursivamente
 * Como a funcao sempre retorna a raiz atualizada e quem chama reatribui
 * o ponteiro (ex.: raiz = remover(raiz, n)), remover a raiz da arvore
 * inteira funciona normalmente, sem precisar de tratamento especial.
 * Em todos os caminhos que mantem o no vivo, o retorno passa por
 * balancear(), pois a remocao pode reduzir a altura de uma subarvore
 * e desbalancear qualquer ancestral (nao so o pai). */
Arvore* remover(Arvore *raiz, int n) {
    if (raiz == NULL) {
        printf("valor nao encontrado\n");
        return NULL;
    }

    if (n < raiz->valor) {
        raiz->esquerda = remover(raiz->esquerda, n);
        return balancear(raiz);
    }
    if (n > raiz->valor) {
        raiz->direita = remover(raiz->direita, n);
        return balancear(raiz);
    }

    /* raiz->valor == n: achamos o no que precisa ser removido */
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        free(raiz);
        printf("o valor %d foi removido\n", n);
        return NULL;
    }

    if (raiz->esquerda != NULL && raiz->direita != NULL) {
        Arvore *substituto = raiz->esquerda;
        while (substituto->direita != NULL) {
            substituto = substituto->direita; /* maior valor da subarvore esquerda */
        }
        raiz->valor = substituto->valor;      /* raiz recebe o valor do antecessor */
        substituto->valor = n;                /* antecessor recebe o valor a remover */
        raiz->esquerda = remover(raiz->esquerda, n); /* remove o antecessor, que agora
                                                          tem no maximo um filho */
        return balancear(raiz);
    }

    /* no com exatamente um filho: o filho assume o lugar da raiz removida.
     * O filho ja e uma subarvore AVL valida, entao nao precisa de ajuste. */
    Arvore *filho = (raiz->esquerda != NULL) ? raiz->esquerda : raiz->direita;
    free(raiz);
    printf("o valor %d foi removido\n", n);
    return filho;
}

/* Busca um valor na arvore. Retorna o ponteiro para o no encontrado,
 * ou NULL se o valor nao existir na arvore. */
Arvore* busca(Arvore *raiz, int n) {
    while (raiz) {
        if (n < raiz->valor) {
            raiz = raiz->esquerda;
        } else if (n > raiz->valor) {
            raiz = raiz->direita;
        } else {
            return raiz;
        }
    }
    return NULL;
}

/* Percurso pre-ordem: raiz -> esquerda -> direita */
void printarPreOrdem(Arvore *raiz) {
    if (raiz) {
        printf("%d ", raiz->valor);
        printarPreOrdem(raiz->esquerda);
        printarPreOrdem(raiz->direita);
    }
}

/* Percurso em ordem: esquerda -> raiz -> direita (imprime em ordem crescente) */
void printarEmOrdem(Arvore *raiz) {
    if (raiz) {
        printarEmOrdem(raiz->esquerda);
        printf("%d ", raiz->valor);
        printarEmOrdem(raiz->direita);
    }
}

/* Percurso pos-ordem: esquerda -> direita -> raiz */
void printarPosOrdem(Arvore *raiz) {
    if (raiz) {
        printarPosOrdem(raiz->esquerda);
        printarPosOrdem(raiz->direita);
        printf("%d ", raiz->valor);
    }
}

/* ------------------------ visualizacao do balanceamento ------------------------ */

/* Desenha a arvore "deitada" (raiz a esquerda), mostrando para cada no o
 * fator de balanceamento (fb) e a altura (h). A subarvore direita aparece
 * em cima e a esquerda embaixo, entao basta girar a cabeca 90 graus para
 * a esquerda para ver a arvore na posicao normal.
 * Um no com |fb| > 1 e marcado com "<-- DESBALANCEADO" (na AVL isso
 * nunca deveria acontecer, mas serve como verificacao). */
void mostrarBalanceamento(Arvore *raiz, int nivel) {
    int i, fb;

    if (raiz == NULL) {
        return;
    }

    mostrarBalanceamento(raiz->direita, nivel + 1);

    for (i = 0; i < nivel; i++) {
        printf("        ");
    }
    fb = fatorBalanceamento(raiz);
    printf("%d (fb=%+d, h=%d)", raiz->valor, fb, raiz->altura);
    if (fb > 1 || fb < -1) {
        printf("  <-- DESBALANCEADO");
    }
    printf("\n");

    mostrarBalanceamento(raiz->esquerda, nivel + 1);
}

/* Retorna 1 se todos os nos da arvore tem |fb| <= 1, e 0 caso contrario. */
int estaBalanceada(Arvore *raiz) {
    int fb;

    if (raiz == NULL) {
        return 1;
    }
    fb = fatorBalanceamento(raiz);
    if (fb > 1 || fb < -1) {
        return 0;
    }
    return estaBalanceada(raiz->esquerda) && estaBalanceada(raiz->direita);
}

/* Libera toda a memoria alocada pela arvore (percurso pos-ordem: os dois
 * filhos sao liberados antes do proprio no) e zera o ponteiro da raiz. */
void liberarArvore(Arvore **raiz) {
    if (*raiz == NULL) {
        return;
    }
    liberarArvore(&((*raiz)->esquerda));
    liberarArvore(&((*raiz)->direita));
    free(*raiz);
    *raiz = NULL;
}

int main(void) {
    Arvore *raiz = NULL, *encontrado;
    int opcao, subOpcao, n;

    do {
        printf("\n1 - Inserir valor\n");
        printf("2 - Buscar valor\n");
        printf("3 - Remover valor\n");
        printf("4 - Percorrer arvore\n");
        printf("5 - Ver balanceamento da arvore\n");
        printf("0 - Sair\n");
        printf("escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {

            case 1:
                printf("digite o valor que deseja inserir: ");
                scanf("%d", &n);
                insec(&raiz, n);
                break;

            case 2:
                printf("digite o valor que deseja buscar: ");
                scanf("%d", &n);
                encontrado = busca(raiz, n);
                if (encontrado) {
                    printf("o valor %d foi encontrado na arvore\n", encontrado->valor);
                } else {
                    printf("o valor %d nao foi encontrado na arvore\n", n);
                }
                break;

            case 3:
                printf("digite o valor que deseja remover: ");
                scanf("%d", &n);
                raiz = remover(raiz, n);
                break;

            case 4:
                printf("\n1 - Pre-ordem\n");
                printf("2 - Em ordem\n");
                printf("3 - Pos-ordem\n");
                printf("escolha uma opcao: ");
                scanf("%d", &subOpcao);

                switch (subOpcao) {
                    case 1:
                        printf("Pre-Ordem: ");
                        printarPreOrdem(raiz);
                        printf("\n");
                        break;
                    case 2:
                        printf("Em Ordem: ");
                        printarEmOrdem(raiz);
                        printf("\n");
                        break;
                    case 3:
                        printf("Pos-Ordem: ");
                        printarPosOrdem(raiz);
                        printf("\n");
                        break;
                    default:
                        printf("opcao invalida\n");
                }
                break;

            case 5:
                if (raiz == NULL) {
                    printf("a arvore esta vazia\n");
                } else {
                    printf("\nArvore (raiz a esquerda, direita em cima):\n\n");
                    mostrarBalanceamento(raiz, 0);
                    printf("\naltura da arvore: %d\n", altura(raiz));
                    printf("fator de balanceamento da raiz: %+d\n", fatorBalanceamento(raiz));
                    printf("arvore balanceada: %s\n", estaBalanceada(raiz) ? "sim" : "nao");
                }
                break;

            case 0:
                liberarArvore(&raiz);
                printf("memoria liberada, encerrando...\n");
                break;

            default:
                printf("opcao invalida\n");
        }
    } while (opcao != 0);

    return 0;
}