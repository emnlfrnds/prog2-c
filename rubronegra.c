/*
 * ARVORE RUBRO-NEGRA (inteiros, alocacao dinamica)
 *
 * Operacoes: inserir, buscar, remover, percursos (pre/em/pos-ordem)
 * e exibicao de altura / altura negra / cor de cada no.
 *
 * Regras da rubro-negra:
 *   1. Todo no e VERMELHO ou PRETO.
 *   2. A raiz e PRETA.
 *   3. Filhos NULL (folhas) contam como PRETOS.
 *   4. No VERMELHO nao pode ter filho VERMELHO.
 *   5. Todo caminho de um no ate suas folhas NULL tem o mesmo
 *      numero de nos PRETOS (altura negra).
 *
 * Valores repetidos: sao IGNORADOS na insercao (a arvore nao
 * guarda duplicatas).
 */

#include <stdio.h>
#include <stdlib.h>

/* Codigos de cor do terminal (so para deixar o desenho bonito) */
#define COR_VERMELHA "\033[1;31m"
#define COR_BRANCA   "\033[1;37m"
#define COR_RESET    "\033[0m"

typedef enum { VERMELHO, PRETO } Cor;

/* No da arvore. Alem de valor/esq/dir (como na BST), tem a cor e o
 * ponteiro para o pai: o pai e necessario para achar o tio, o avo e
 * o irmao durante a correcao de insercao e de remocao. */
typedef struct No {
    int valor;
    Cor cor;
    struct No *esq;
    struct No *dir;
    struct No *pai;
} No;

No *raiz = NULL;      /* comeca com arvore vazia */

/* NULL conta como PRETO (regra 3). Evita testar NULL em todo lugar. */
Cor cor_do_no(No *n) {
    return (n == NULL) ? PRETO : n->cor;
}

/* Todo no novo nasce VERMELHO: isso nao altera a altura negra
 * (regra 5), entao so pode violar a regra 4, que e mais facil de
 * consertar. */
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

/* ============================================================
 * ROTACOES: mudam a forma da arvore mas MANTEM a ordem da BST.
 * Em cada uma so ~3 ligacoes mudam: custo O(1).
 * ============================================================ */
void rotacao_esquerda(No *x) {
    No *y = x->dir;

    x->dir = y->esq;              /* subarvore esquerda de y vira filho direito de x */
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

/* ============================================================
 * INSERCAO
 * ============================================================ */

/* Restaura as regras depois da insercao. Roda enquanto o pai de n
 * for VERMELHO (violacao da regra 4). Olha a cor do TIO:
 *
 *   Caso 1 - tio VERMELHO: pai e tio viram PRETOS, avo vira
 *            VERMELHO e repetimos a verificacao a partir do avo.
 *   Caso 2 - tio PRETO e n "dobrado" (joelho): rotacao no pai
 *            para "esticar", virando o caso 3.
 *   Caso 3 - tio PRETO e n "esticado": pai vira PRETO, avo vira
 *            VERMELHO e rotacao no avo. Termina o laco.
 *
 * No maximo 2 rotacoes por insercao (casos 2 e 3). */
void corrigir_insercao(No *n) {
    while (n != raiz && n->pai->cor == VERMELHO) {
        No *pai = n->pai;
        /* pai e vermelho e a raiz e preta, logo pai nao e a raiz: avo existe */
        No *avo = pai->pai;

        if (pai == avo->esq) {
            No *tio = avo->dir;
            if (cor_do_no(tio) == VERMELHO) {
                printf("  -> Recoloracao: pai (%d) e tio (%d) viram PRETOS, avo (%d) vira VERMELHO\n",
                       pai->valor, tio->valor, avo->valor);
                pai->cor = PRETO;
                tio->cor = PRETO;
                avo->cor = VERMELHO;
                n = avo;          /* o avo pode ter criado outro problema acima */
            } else {
                if (n == pai->dir) {
                    printf("  -> Rotacao esquerda em %d\n", pai->valor);
                    n = pai;
                    rotacao_esquerda(n);
                    pai = n->pai; /* apos a rotacao o pai mudou */
                }
                printf("  -> Recoloracao: %d vira PRETO e %d vira VERMELHO\n",
                       pai->valor, avo->valor);
                printf("  -> Rotacao direita em %d\n", avo->valor);
                pai->cor = PRETO;
                avo->cor = VERMELHO;
                rotacao_direita(avo);
            }
        } else {
            /* pai e filho DIREITO do avo: tudo espelhado */
            No *tio = avo->esq;
            if (cor_do_no(tio) == VERMELHO) {
                printf("  -> Recoloracao: pai (%d) e tio (%d) viram PRETOS, avo (%d) vira VERMELHO\n",
                       pai->valor, tio->valor, avo->valor);
                pai->cor = PRETO;
                tio->cor = PRETO;
                avo->cor = VERMELHO;
                n = avo;
            } else {
                if (n == pai->esq) {
                    printf("  -> Rotacao direita em %d\n", pai->valor);
                    n = pai;
                    rotacao_direita(n);
                    pai = n->pai;
                }
                printf("  -> Recoloracao: %d vira PRETO e %d vira VERMELHO\n",
                       pai->valor, avo->valor);
                printf("  -> Rotacao esquerda em %d\n", avo->valor);
                pai->cor = PRETO;
                avo->cor = VERMELHO;
                rotacao_esquerda(avo);
            }
        }
    }

    /* Regra 2: a raiz e sempre PRETA (a recoloracao pode te-la pintado de vermelho) */
    if (raiz->cor == VERMELHO) {
        printf("  -> Raiz (%d) volta a ser PRETA\n", raiz->valor);
        raiz->cor = PRETO;
    }
}

/* Insere como numa BST normal e depois corrige as cores.
 * Retorna 1 se inseriu, 0 se o valor ja existia (repetido e ignorado). */
int inserir(int valor) {
    No *atual = raiz;
    No *pai = NULL;

    while (atual != NULL) {       /* 1) desce ate achar o lugar */
        pai = atual;
        if (valor == atual->valor)
            return 0;
        else if (valor < atual->valor)
            atual = atual->esq;
        else
            atual = atual->dir;
    }

    No *novo = criar_no(valor);   /* 2) cria (VERMELHO) e liga ao pai */
    novo->pai = pai;
    if (pai == NULL)
        raiz = novo;
    else if (valor < pai->valor)
        pai->esq = novo;
    else
        pai->dir = novo;

    corrigir_insercao(novo);      /* 3) arruma cores e rotacoes */
    return 1;
}

/* ============================================================
 * BUSCA
 * ============================================================ */

/* Retorna o no com o valor, ou NULL se nao existir. O(log n). */
No *buscar(int valor) {
    No *atual = raiz;
    while (atual != NULL) {
        if (valor == atual->valor)
            return atual;
        atual = (valor < atual->valor) ? atual->esq : atual->dir;
    }
    return NULL;
}

/* ============================================================
 * REMOCAO
 * ============================================================ */

/* Coloca o no v no lugar do no u (u sai da posicao dele na arvore). */
void transplantar(No *u, No *v) {
    if (u->pai == NULL)
        raiz = v;
    else if (u == u->pai->esq)
        u->pai->esq = v;
    else
        u->pai->dir = v;
    if (v != NULL)
        v->pai = u->pai;
}

/* Menor valor da subarvore (sucessor in-order de quem tem dois filhos). */
No *minimo(No *n) {
    while (n->esq != NULL)
        n = n->esq;
    return n;
}

/* Remover um no PRETO tira um no preto de um caminho (quebra a regra 5).
 * Dizemos que x, o no que ocupou o lugar, carrega um "preto extra"
 * (duplo preto). Como x pode ser NULL, passamos o pai dele a parte.
 * Olhamos o IRMAO de x:
 *
 *   Caso 1 - irmao VERMELHO: troca cores e rotaciona no pai; cai
 *            em um dos casos 2, 3 ou 4 (agora com irmao preto).
 *   Caso 2 - irmao PRETO com dois sobrinhos PRETOS: irmao vira
 *            VERMELHO e o preto extra sobe para o pai.
 *   Caso 3 - irmao PRETO, sobrinho "de perto" VERMELHO e o "de
 *            longe" PRETO: rotacao no irmao, vira o caso 4.
 *   Caso 4 - irmao PRETO e sobrinho "de longe" VERMELHO: recolore
 *            e rotaciona no pai. Resolve de vez.
 * No maximo 3 rotacoes por remocao. */
void corrigir_remocao(No *x, No *pai) {
    while (x != raiz && cor_do_no(x) == PRETO) {
        if (x == pai->esq) {
            No *irmao = pai->dir;           /* existe: x tem preto extra */

            if (irmao->cor == VERMELHO) {   /* Caso 1 */
                printf("  -> Remocao caso 1: irmao (%d) vermelho, rotacao esquerda em %d\n",
                       irmao->valor, pai->valor);
                irmao->cor = PRETO;
                pai->cor = VERMELHO;
                rotacao_esquerda(pai);
                irmao = pai->dir;
            }
            if (cor_do_no(irmao->esq) == PRETO && cor_do_no(irmao->dir) == PRETO) {
                /* Caso 2 */
                printf("  -> Remocao caso 2: irmao (%d) vira VERMELHO, problema sobe\n",
                       irmao->valor);
                irmao->cor = VERMELHO;
                x = pai;
                pai = x->pai;
            } else {
                if (cor_do_no(irmao->dir) == PRETO) {   /* Caso 3 */
                    printf("  -> Remocao caso 3: rotacao direita em %d\n", irmao->valor);
                    irmao->esq->cor = PRETO;
                    irmao->cor = VERMELHO;
                    rotacao_direita(irmao);
                    irmao = pai->dir;
                }
                /* Caso 4 */
                printf("  -> Remocao caso 4: rotacao esquerda em %d\n", pai->valor);
                irmao->cor = pai->cor;
                pai->cor = PRETO;
                irmao->dir->cor = PRETO;
                rotacao_esquerda(pai);
                x = raiz;                           /* termina o laco */
            }
        } else {
            /* x e filho DIREITO: tudo espelhado */
            No *irmao = pai->esq;

            if (irmao->cor == VERMELHO) {           /* Caso 1 */
                printf("  -> Remocao caso 1: irmao (%d) vermelho, rotacao direita em %d\n",
                       irmao->valor, pai->valor);
                irmao->cor = PRETO;
                pai->cor = VERMELHO;
                rotacao_direita(pai);
                irmao = pai->esq;
            }
            if (cor_do_no(irmao->esq) == PRETO && cor_do_no(irmao->dir) == PRETO) {
                /* Caso 2 */
                printf("  -> Remocao caso 2: irmao (%d) vira VERMELHO, problema sobe\n",
                       irmao->valor);
                irmao->cor = VERMELHO;
                x = pai;
                pai = x->pai;
            } else {
                if (cor_do_no(irmao->esq) == PRETO) {   /* Caso 3 */
                    printf("  -> Remocao caso 3: rotacao esquerda em %d\n", irmao->valor);
                    irmao->dir->cor = PRETO;
                    irmao->cor = VERMELHO;
                    rotacao_esquerda(irmao);
                    irmao = pai->esq;
                }
                /* Caso 4 */
                printf("  -> Remocao caso 4: rotacao direita em %d\n", pai->valor);
                irmao->cor = pai->cor;
                pai->cor = PRETO;
                irmao->esq->cor = PRETO;
                rotacao_direita(pai);
                x = raiz;
            }
        }
    }
    if (x != NULL)
        x->cor = PRETO;           /* absorve o preto extra */
}

/* Remove o valor. Retorna 1 se removeu, 0 se nao existia.
 * Os tres casos classicos da BST:
 *   - folha ou so filho direito  (esq == NULL)
 *   - so filho esquerdo          (dir == NULL)
 *   - dois filhos: o sucessor in-order (menor da subarvore direita)
 *     ocupa o lugar de z e herda a cor dele.
 * Se o no que "sumiu" da posicao era PRETO, corrige a regra 5. */
int remover(int valor) {
    No *z = buscar(valor);
    if (z == NULL)
        return 0;

    No *y = z;                    /* no que sai (ou se move) */
    Cor cor_original = y->cor;
    No *x;                        /* no que ocupa a posicao liberada */
    No *x_pai;                    /* pai de x (x pode ser NULL) */

    if (z->esq == NULL) {                 /* folha ou apenas filho direito */
        x = z->dir;
        x_pai = z->pai;
        transplantar(z, z->dir);
    } else if (z->dir == NULL) {          /* apenas filho esquerdo */
        x = z->esq;
        x_pai = z->pai;
        transplantar(z, z->esq);
    } else {                              /* dois filhos: usa o sucessor */
        y = minimo(z->dir);
        cor_original = y->cor;
        x = y->dir;
        if (y->pai == z) {
            x_pai = y;
        } else {
            x_pai = y->pai;
            transplantar(y, y->dir);      /* tira y do lugar dele */
            y->dir = z->dir;
            y->dir->pai = y;
        }
        transplantar(z, y);               /* y assume o lugar de z */
        y->esq = z->esq;
        y->esq->pai = y;
        y->cor = z->cor;                  /* herda a cor: altura negra preservada */
    }

    free(z);

    /* So se o no removido/movido era PRETO a regra 5 pode ter quebrado */
    if (cor_original == PRETO)
        corrigir_remocao(x, x_pai);
    return 1;
}

/* ============================================================
 * PERCURSOS
 * ============================================================ */
void imprimir_no(No *n) {
    printf("%d(%c) ", n->valor, n->cor == VERMELHO ? 'V' : 'P');
}

void pre_ordem(No *n) {           /* raiz, esquerda, direita */
    if (n == NULL) return;
    imprimir_no(n);
    pre_ordem(n->esq);
    pre_ordem(n->dir);
}

void em_ordem(No *n) {            /* esquerda, raiz, direita: sai ordenado */
    if (n == NULL) return;
    em_ordem(n->esq);
    imprimir_no(n);
    em_ordem(n->dir);
}

void pos_ordem(No *n) {           /* esquerda, direita, raiz */
    if (n == NULL) return;
    pos_ordem(n->esq);
    pos_ordem(n->dir);
    imprimir_no(n);
}

/* ============================================================
 * ALTURA, ALTURA NEGRA E EXIBICAO
 * ============================================================ */

/* Altura em nos: arvore vazia = 0, um unico no = 1. */
int altura(No *n) {
    if (n == NULL) return 0;
    int e = altura(n->esq);
    int d = altura(n->dir);
    return 1 + (e > d ? e : d);
}

/* Altura negra: quantos nos PRETOS existem de n ate uma folha NULL
 * (contando n). Na rubro-negra valida, todo caminho da o mesmo valor. */
int altura_negra(No *n) {
    if (n == NULL) return 0;
    return altura_negra(n->esq) + (n->cor == PRETO ? 1 : 0);
}

/* Verifica as regras 4 e 5 na subarvore. Retorna a altura negra
 * ou -1 se alguma regra estiver violada. */
int verificar(No *n) {
    if (n == NULL) return 0;
    if (n->cor == VERMELHO &&
        (cor_do_no(n->esq) == VERMELHO || cor_do_no(n->dir) == VERMELHO))
        return -1;
    int e = verificar(n->esq);
    int d = verificar(n->dir);
    if (e < 0 || d < 0 || e != d)
        return -1;
    return e + (n->cor == PRETO ? 1 : 0);
}

/* Lista cada no em ordem com cor, altura e altura negra. */
void listar_alturas(No *n) {
    if (n == NULL) return;
    listar_alturas(n->esq);
    printf("  no %d | %s | altura = %d | altura negra = %d\n",
           n->valor,
           n->cor == VERMELHO ? "VERMELHO" : "PRETO   ",
           altura(n), altura_negra(n));
    listar_alturas(n->dir);
}

/* Desenha a arvore "deitada": raiz na esquerda, filhos DIREITOS em
 * cima e ESQUERDOS embaixo (percurso direita-no-esquerda). */
void desenhar(No *n, int nivel) {
    if (n == NULL) return;

    desenhar(n->dir, nivel + 1);

    for (int i = 0; i < nivel; i++)
        printf("        ");

    if (n->cor == VERMELHO)
        printf(COR_VERMELHA "%d (V)" COR_RESET "\n", n->valor);
    else
        printf(COR_BRANCA "%d (P)" COR_RESET "\n", n->valor);

    desenhar(n->esq, nivel + 1);
}

void exibir_altura_e_cores(void) {
    if (raiz == NULL) {
        printf("A arvore esta vazia.\n");
        return;
    }
    printf("\n(V) = vermelho | (P) = preto | raiz na esquerda\n\n");
    desenhar(raiz, 0);

    printf("\nAltura da arvore: %d\n", altura(raiz));
    printf("Altura negra da raiz: %d\n", altura_negra(raiz));
    printf("\nNo a no (em ordem):\n");
    listar_alturas(raiz);

    printf("\nRegras da rubro-negra: %s\n",
           (raiz->cor == PRETO && verificar(raiz) >= 0) ? "OK" : "VIOLADAS!");
}

/* Libera toda a memoria (pos-ordem: filhos antes do pai) */
void liberar(No *n) {
    if (n == NULL) return;
    liberar(n->esq);
    liberar(n->dir);
    free(n);
}

/* ============================================================
 * ENTRADA E MENUS
 * ============================================================ */

/* Le um inteiro e limpa o resto da linha.
 * Retorna 1 se leu, 0 se a entrada foi invalida, -1 se acabou (EOF). */
int ler_inteiro(int *numero) {
    int r = scanf("%d", numero);
    if (r == EOF)
        return -1;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    return r == 1;
}

/* Submenu de percursos (switch-case). Retorna -1 em caso de EOF. */
int menu_percursos(void) {
    int op;

    printf("\n--- Percorrer arvore ---\n");
    printf("1 - Pre-ordem\n");
    printf("2 - Em ordem\n");
    printf("3 - Pos-ordem\n");
    printf("Escolha: ");

    int r = ler_inteiro(&op);
    if (r < 0) return -1;
    if (r == 0) op = -1;

    printf("\n");
    switch (op) {
        case 1: printf("Pre-ordem: ");  pre_ordem(raiz);  printf("\n"); break;
        case 2: printf("Em ordem: ");   em_ordem(raiz);   printf("\n"); break;
        case 3: printf("Pos-ordem: ");  pos_ordem(raiz);  printf("\n"); break;
        default: printf("Opcao invalida!\n"); break;
    }
    return 0;
}

int main(void) {
    int opcao = -1;
    int valor, r;

    do {
        printf("\n===== ARVORE RUBRO-NEGRA =====\n");
        printf("1 - Inserir valor\n");
        printf("2 - Buscar valor\n");
        printf("3 - Remover valor\n");
        printf("4 - Percorrer arvore\n");
        printf("5 - Exibir altura e cores\n");
        printf("0 - Sair\n");
        printf("Escolha: ");

        r = ler_inteiro(&opcao);
        if (r < 0) break;                 /* EOF: sai e libera a memoria */
        if (r == 0) opcao = -1;           /* texto invalido cai no default */

        switch (opcao) {
            case 1:
                printf("Valor a inserir: ");
                r = ler_inteiro(&valor);
                if (r < 0) { opcao = 0; break; }
                if (r == 0) { printf("Digite apenas numeros inteiros!\n"); break; }
                if (inserir(valor))
                    printf("%d inserido com sucesso!\n", valor);
                else
                    printf("%d ja existe na arvore (repetidos sao ignorados).\n", valor);
                break;

            case 2:
                printf("Valor a buscar: ");
                r = ler_inteiro(&valor);
                if (r < 0) { opcao = 0; break; }
                if (r == 0) { printf("Digite apenas numeros inteiros!\n"); break; }
                {
                    No *achado = buscar(valor);
                    if (achado != NULL)
                        printf("%d esta na arvore (cor %s).\n", valor,
                               achado->cor == VERMELHO ? "VERMELHA" : "PRETA");
                    else
                        printf("%d NAO esta na arvore.\n", valor);
                }
                break;

            case 3:
                printf("Valor a remover: ");
                r = ler_inteiro(&valor);
                if (r < 0) { opcao = 0; break; }
                if (r == 0) { printf("Digite apenas numeros inteiros!\n"); break; }
                if (remover(valor))
                    printf("%d removido com sucesso!\n", valor);
                else
                    printf("%d nao existe na arvore.\n", valor);
                break;

            case 4:
                if (raiz == NULL)
                    printf("A arvore esta vazia.\n");
                else if (menu_percursos() < 0)
                    opcao = 0;
                break;

            case 5:
                exibir_altura_e_cores();
                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida!\n");
                break;
        }
    } while (opcao != 0);

    liberar(raiz);                        /* libera toda a memoria da arvore */
    return 0;
}