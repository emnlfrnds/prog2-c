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
    struct No *pai;   /* pai: necessario para achar o tio e o avo na correcao */
} No;

No *raiz = NULL;      /* comeca com arvore vazia */

/* ------------------------------------------------------------
 * Cria um no novo. Todo no novo nasce VERMELHO.
 * Por que vermelho? Nao altera a altura negra (regra 5),
 * entao so pode violar a regra 4 (vermelho com filho vermelho),
 * que e mais facil de consertar.
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

/* ------------------------------------------------------------
 * ROTACOES: mudam a forma da arvore mas MANTEM a ordem da ABP.
 * Em cada uma, so ~3 ligacoes mudam: custo O(1).
 * ------------------------------------------------------------ */
void rotacao_esquerda(No *x) {
    No *y = x->dir;

    x->dir = y->esq;              /* subarvore B de y passa a ser filho direito de x */
    if (y->esq != NULL)
        y->esq->pai = x;

    y->pai = x->pai;              /* y assume o lugar de x */
    if (x->pai == NULL)
        raiz = y;                 /* x era a raiz: atualiza a raiz global */
    else if (x == x->pai->esq)
        x->pai->esq = y;
    else
        x->pai->dir = y;

    y->esq = x;                   /* x desce para a esquerda de y */
    x->pai = y;
}

void rotacao_direita(No *x) {     /* espelho da rotacao esquerda */
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
 * CORRIGIR: restaura as regras depois da insercao.
 * Roda enquanto o pai de n for VERMELHO (violacao da regra 4).
 *
 *   Caso 1 - tio VERMELHO: RECOLORACAO
 *            pai e tio viram PRETOS, avo vira VERMELHO.
 *            Depois repetimos a verificacao a partir do avo.
 *
 *   Caso 2 - tio PRETO (ou NULL) e n esta "dobrado":
 *            primeiro uma rotacao para "esticar" (vira Caso 3).
 *
 *   Caso 3 - tio PRETO (ou NULL) e n esta "esticado":
 *            pai vira PRETO, avo vira VERMELHO, rotacao no avo.
 *
 * Total de rotacoes por insercao: no maximo 2 (casos 2 e 3).
 * A recoloracao e que pode subir ate a raiz.
 * ------------------------------------------------------------ */
void corrigir(No *n) {
    while (n != raiz && n->pai->cor == VERMELHO) {
        No *pai = n->pai;
        /* pai e vermelho, e a raiz e preta, entao pai nao e a raiz: avo existe */
        No *avo = pai->pai;

        if (pai == avo->esq) {
            /* ---- pai e filho ESQUERDO do avo ---- */
            No *tio = avo->dir;   /* NULL conta como preto */

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
                    pai = n->pai;     /* apos a rotacao, o pai mudou */
                }
                /* Caso 3: n esta "esticado" -> recolore e rotacao direita no avo */
                printf("  -> Recoloracao: %d vira PRETO e %d vira VERMELHO\n",
                       pai->valor, avo->valor);
                printf("  -> Rotacao direita em %d\n", avo->valor);
                pai->cor = PRETO;
                avo->cor = VERMELHO;
                rotacao_direita(avo);
                /* o laco termina: o novo pai (PRETO) encerra a violacao */
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

    /* Regra 2: a raiz sempre e PRETA (a recoloracao pode ter pintado de vermelho) */
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
 * Percurso em ordem invertida (direita, no, esquerda):
 * a raiz fica na esquerda; filhos DIREITOS em cima e ESQUERDOS embaixo.
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

/* Libera toda a memoria da arvore (pos-ordem: filhos antes do pai) */
void liberar(No *n) {
    if (n == NULL)
        return;
    liberar(n->esq);
    liberar(n->dir);
    free(n);
}

/* Le um inteiro do teclado e limpa o resto da linha.
 * Retorna 1 se leu, 0 se a entrada foi invalida, -1 se acabou a entrada (EOF). */
int ler_inteiro(int *numero) {
    int r = scanf("%d", numero);
    if (r == EOF)
        return -1;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    return r == 1;
}

/* ------------------------------------------------------------
 * MENU
 * ------------------------------------------------------------ */
int main(void) {
    int opcao = -1;
    int valor;
    int r;

    while (opcao != 0) {
        printf("\n===== ARVORE RUBRO-NEGRA =====\n");
        printf("1 - Inserir numero\n");
        printf("2 - Mostrar arvore\n");
        printf("0 - Sair\n");
        printf("Escolha: ");

        r = ler_inteiro(&opcao);
        if (r < 0)
            break;                    /* EOF: sai do laco e libera a memoria */
        if (r == 0) {
            opcao = -1;
            printf("Opcao invalida!\n");
            continue;
        }

        if (opcao == 1) {
            printf("Numero a inserir: ");
            r = ler_inteiro(&valor);
            if (r < 0)
                break;
            if (r == 0) {
                printf("Digite apenas numeros inteiros!\n");
                continue;
            }
            /* a arvore NAO e mostrada aqui; use a opcao 2 */
            if (inserir(valor))
                printf("%d inserido com sucesso!\n", valor);
            else
                printf("%d ja existe na arvore!\n", valor);

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