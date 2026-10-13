#include <stdio.h>
#include <stdlib.h>

/* Codigos de cor do terminal (so para deixar o desenho bonito) */
#define COR_VERMELHA "\033[1;31m"
#define COR_BRANCA   "\033[1;37m"
#define COR_RESET    "\033[0m"

typedef enum { VERMELHO, PRETO } Cor;

typedef struct No {
    int valor;
    Cor cor;
    struct No *esq;   /* filho da esquerda */
    struct No *dir;   /* filho da direita  */
    struct No *pai;   /* pai do no         */
} No;

No *raiz = NULL;      /* comeca com arvore vazia */

/* ------------------------------------------------------------
 * Cria um no novo. Todo no novo nasce VERMELHO.
 * ------------------------------------------------------------ */
No *criar_no(int valor) {
    No *n = (No *)malloc(sizeof(No));
    if (n == NULL) {
        printf("Erro: sem memoria!\n");
        exit(1);
    }
    n->valor = valor;
    n->cor = VERMELHO;
    n->esq = NULL;
    n->dir = NULL;
    n->pai = NULL;
    return n;
}

void rotacao_esquerda(No *x) {
    No *y = x->dir;

    x->dir = y->esq;              /* B passa a ser filho direito de x */
    if (y->esq != NULL)
        y->esq->pai = x;

    y->pai = x->pai;              /* y assume o lugar de x */
    if (x->pai == NULL)
        raiz = y;
    else if (x == x->pai->esq)
        x->pai->esq = y;
    else
        x->pai->dir = y;

    y->esq = x;                   /* x desce para a esquerda de y */
    x->pai = y;
}

void rotacao_direita(No *x) {
    No *y = x->esq;

    x->esq = y->dir;
    if (y->dir != NULL)
        y->dir->pai = x;

    y->pai = x->pai;
    if (x->pai == NULL)
        raiz = y;
    else if (x == x->pai->dir)
        x->pai->dir = y;
    else
        x->pai->esq = y;

    y->dir = x;
    x->pai = y;
}

/* ------------------------------------------------------------
 *   Caso 1 - tio VERMELHO: RECOLORACAO
 *            pai e tio viram PRETOS, avo vira VERMELHO.
 *            Depois repetimos a verificacao a partir do avo.
 *
 *   Caso 2 - tio PRETO (ou NULL) e n esta "dobrado":
 *            primeiro uma rotacao para "esticar" (vira Caso 3).
 *
 *   Caso 3 - tio PRETO (ou NULL) e n esta "esticado":
 *            pai vira PRETO, avo vira VERMELHO, rotacao no avo.
 * ------------------------------------------------------------ */
void corrigir(No *n) {
    while (n != raiz && n->pai->cor == VERMELHO) {
        No *pai = n->pai;
        No *avo = pai->pai;   /* pai e vermelho, entao nao e a raiz: avo existe */

        if (pai == avo->esq) {
            /* ---- pai e filho ESQUERDO do avo ---- */
            No *tio = avo->dir;

            if (tio != NULL && tio->cor == VERMELHO) {
                /* Caso 1: recoloracao */
                printf("  -> Recoloracao: pai (%d) e tio (%d) viram PRETOS, avo (%d) vira VERMELHO\n",
                       pai->valor, tio->valor, avo->valor);
                pai->cor = PRETO;
                tio->cor = PRETO;
                avo->cor = VERMELHO;
                n = avo;      /* o avo pode ter criado outro problema acima */
            } else {
                if (n == pai->dir) {
                    /* Caso 2: n esta "dobrado" -> rotacao esquerda no pai */
                    printf("  -> Rotacao esquerda em %d\n", pai->valor);
                    n = pai;
                    rotacao_esquerda(n);
                    pai = n->pai;
                }
                /* Caso 3: n esta "esticado" -> recolore e rotacao direita no avo */
                printf("  -> Recoloracao: %d vira PRETO e %d vira VERMELHO\n",
                       pai->valor, avo->valor);
                printf("  -> Rotacao direita em %d\n", avo->valor);
                pai->cor = PRETO;
                avo->cor = VERMELHO;
                rotacao_direita(avo);
            }
        } else {
            /* ---- pai e filho DIREITO do avo (tudo espelhado) ---- */
            No *tio = avo->esq;

            if (tio != NULL && tio->cor == VERMELHO) {
                /* Caso 1: recoloracao */
                printf("  -> Recoloracao: pai (%d) e tio (%d) viram PRETOS, avo (%d) vira VERMELHO\n",
                       pai->valor, tio->valor, avo->valor);
                pai->cor = PRETO;
                tio->cor = PRETO;
                avo->cor = VERMELHO;
                n = avo;
            } else {
                if (n == pai->esq) {
                    /* Caso 2: rotacao direita no pai */
                    printf("  -> Rotacao direita em %d\n", pai->valor);
                    n = pai;
                    rotacao_direita(n);
                    pai = n->pai;
                }
                /* Caso 3: recolore e rotacao esquerda no avo */
                printf("  -> Recoloracao: %d vira PRETO e %d vira VERMELHO\n",
                       pai->valor, avo->valor);
                printf("  -> Rotacao esquerda em %d\n", avo->valor);
                pai->cor = PRETO;
                avo->cor = VERMELHO;
                rotacao_esquerda(avo);
            }
        }
    }

    /* Regra 2: a raiz sempre e PRETA */
    if (raiz->cor == VERMELHO) {
        printf("  -> Raiz (%d) volta a ser PRETA\n", raiz->valor);
        raiz->cor = PRETO;
    }
}

/* ------------------------------------------------------------
 * INSERIR: insere como numa arvore binaria de busca normal
 * e depois chama corrigir(). Retorna 1 se inseriu, 0 se repetido.
 * ------------------------------------------------------------ */
int inserir(int valor) {
    No *atual = raiz;
    No *pai = NULL;

    /* 1) Desce na arvore ate achar o lugar do novo no */
    while (atual != NULL) {
        pai = atual;
        if (valor == atual->valor)
            return 0;                 /* valor repetido: nao insere */
        else if (valor < atual->valor)
            atual = atual->esq;
        else
            atual = atual->dir;
    }

    /* 2) Cria o no (VERMELHO) e liga ao pai */
    No *novo = criar_no(valor);
    novo->pai = pai;

    if (pai == NULL)
        raiz = novo;                  /* arvore estava vazia */
    else if (valor < pai->valor)
        pai->esq = novo;
    else
        pai->dir = novo;

    /* 3) Arruma cores e rotacoes se necessario */
    corrigir(novo);
    return 1;
}

/* ------------------------------------------------------------
 * MOSTRAR: desenha a arvore "deitada" (de lado).
 * A raiz fica na esquerda; filhos DIREITOS aparecem em cima
 * e filhos ESQUERDOS embaixo.
 * ------------------------------------------------------------ */
void mostrar(No *n, int nivel) {
    if (n == NULL)
        return;

    mostrar(n->dir, nivel + 1);       /* primeiro o lado direito (fica em cima) */

    for (int i = 0; i < nivel; i++)
        printf("        ");           /* recuo proporcional ao nivel */

    if (n->cor == VERMELHO)
        printf(COR_VERMELHA "%d (V)" COR_RESET "\n", n->valor);
    else
        printf(COR_BRANCA "%d (P)" COR_RESET "\n", n->valor);

    mostrar(n->esq, nivel + 1);       /* depois o lado esquerdo (fica embaixo) */
}

/* Libera toda a memoria da arvore (usado ao sair) */
void liberar(No *n) {
    if (n == NULL)
        return;
    liberar(n->esq);
    liberar(n->dir);
    free(n);
}

/* Le um numero inteiro do teclado; retorna 0 se a entrada for invalida */
int ler_inteiro(int *numero) {
    if (scanf("%d", numero) == 1)
        return 1;
    /* entrada invalida: descarta o que foi digitado */
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    return 0;
}

/* ------------------------------------------------------------
 * MENU
 * ------------------------------------------------------------ */
int main(void) {
    int opcao = -1;
    int valor;

    while (opcao != 0) {
        printf("\n===== ARVORE RUBRO-NEGRA =====\n");
        printf("1 - Inserir numero\n");
        printf("2 - Mostrar arvore\n");
        printf("0 - Sair\n");
        printf("Escolha: ");

        if (!ler_inteiro(&opcao)) {
            opcao = -1;
            printf("Opcao invalida!\n");
            continue;
        }

        if (opcao == 1) {
            printf("Numero a inserir: ");
            if (!ler_inteiro(&valor)) {
                printf("Digite apenas numeros inteiros!\n");
                continue;
            }
            if (inserir(valor)) {
                printf("%d inserido com sucesso!\n", valor);
                printf("\nArvore atual:\n");
                mostrar(raiz, 0);
            } else {
                printf("%d ja existe na arvore!\n", valor);
            }

        } else if (opcao == 2) {
            if (raiz == NULL) {
                printf("A arvore esta vazia.\n");
            } else {
                printf("\n(V) = vermelho | (P) = preto | raiz na esquerda\n\n");
                mostrar(raiz, 0);
            }

        } else if (opcao == 0) {
            printf("Saindo...\n");

        } else {
            printf("Opcao invalida!\n");
        }
    }

    liberar(raiz);
    return 0;
}