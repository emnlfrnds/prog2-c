#include <stdio.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------
 * Arvore de Busca Binaria (BST) para armazenar numeros inteiros.
 * Implementa insercao, busca, remocao (folha / um filho / dois filhos)
 * e os tres percursos classicos (pre-ordem, em ordem e pos-ordem).
 *
 * Valores repetidos: se o valor inserido ja existir na arvore, insec()
 * apenas avisa o usuario e nao cria um novo no (duplicados sao ignorados).
 * --------------------------------------------------------------------- */

typedef struct No {
    int valor;
    struct No *esquerda, *direita;
} Arvore;

/* Insere um novo valor na arvore respeitando a propriedade da BST.
 * Se *raiz for NULL, cria o no ali. Caso contrario, desce recursivamente
 * para a esquerda (valores menores) ou direita (valores maiores) ate
 * achar uma posicao livre. Valores iguais a um no ja existente sao
 * ignorados, ou seja, a arvore nunca guarda duplicados. */
void insec(Arvore **raiz, int n) {
    if (*raiz == NULL) {
        *raiz = malloc(sizeof(Arvore));
        if (*raiz == NULL) {
            printf("erro: memoria insuficiente\n");
            return;
        }
        (*raiz)->valor = n;
        (*raiz)->esquerda = NULL;
        (*raiz)->direita = NULL;
    } else if (n < (*raiz)->valor) {
        insec(&((*raiz)->esquerda), n);
    } else if (n > (*raiz)->valor) {
        insec(&((*raiz)->direita), n);
    } else {
        printf("esse valor ja esta na arvore\n");
    }
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
 * inteira funciona normalmente, sem precisar de tratamento especial. */
Arvore* remover(Arvore *raiz, int n) {
    if (raiz == NULL) {
        printf("valor nao encontrado\n");
        return NULL;
    }

    if (n < raiz->valor) {
        raiz->esquerda = remover(raiz->esquerda, n);
        return raiz;
    }
    if (n > raiz->valor) {
        raiz->direita = remover(raiz->direita, n);
        return raiz;
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
        return raiz;
    }

    /* no com exatamente um filho: o filho assume o lugar da raiz removida */
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
