#include <stdio.h>
#include <stdlib.h>

/*TODO: caso em que se delete a raiz da arvore binaria
        interface melhorada(do exato mesmo jeito e ordem que ta no classroom)
        interface de switch case na parte de printar os valores
        verificar se o uso do malloc ta certo e dar um free() no final pra liberar memoria
        polir o codigo (talvez)

*/

typedef struct No{
    int valor;
    struct No *esquerda , *direita;

}Arvore; //cria a arvore

void insec(Arvore **raiz, int n){
    if(*raiz == NULL){
        *raiz = malloc(sizeof(Arvore));
        (*raiz)->valor = n;
        (*raiz)->esquerda = NULL;
        (*raiz)->direita = NULL;
    }else{
        if(n < (*raiz)->valor){
            insec(&((*raiz)->esquerda), n);
        }else if(n > (*raiz)->valor){
            insec(&((*raiz)->direita), n);
        }else{
            printf("esse valor ja esta na arvore");
        }
    }
} /*insere um elemento na arvore;verifica se a raiz não tem valor, se nao tiver, ele insere o valor e define os ponteiros filhos como NULL.
Se a raiz ja tiver um valor, ao tentar inserir um novo valor, a funcao verifica se o novo valor é maior ou menor que a raiz, caso for menor,
o ponteiro esquerda aponta ao novo valor, se for maior, o ponteiro direita aponta para o novo valor, se for igual, nao insere o valor.
*/
Arvore* remover(Arvore *raiz, int n){
    if(raiz == NULL){ //verifica se o valor existe
        printf("valor nao encontrado");
        return NULL;
    }else{
        if(raiz->valor == n){ //no caso de ser uma folha, apenas libera a memoria da raiz
            if(raiz->esquerda == NULL && raiz->direita == NULL){
                free(raiz);
                printf("o valor %d foi removido", n);
                return NULL;
            }else{
                if(raiz->esquerda != NULL && raiz->direita != NULL){ //a gente vai fazer uma substituiçao com antecessor in-order aqui
                    Arvore *substituto = raiz->esquerda; //cria um substituto que aponta ao valor a esquerda da raiz
                    while(substituto->direita != NULL){
                        substituto = substituto->direita; //percorre os valores a direita da subarvore a esquerda do valor que vai ser deletado
                    }
                    raiz->valor = substituto->valor; //faz uma troca de valores, a raiz recebe o valor substituto
                    substituto->valor = n; //o substituto recebe o valor que a gente quer deletar
                    raiz->esquerda = remover(raiz->esquerda, n); /*perceba que, ao fazer essas substituições, a raiz que a gente quer deletar
                                                                   se torna uma raiz com 1 nó ou sem nós, podendo aplicar os modos de deletar raiz*/ 
                    return raiz;

                }else{ //caso tiver 1 nó, cria um ponteiro novo que aponta pro no da raiz que vai ser deletada, e depois retorna ela mesma (o nó vai para a raiz pai da raiz deletada)
                    Arvore *substituto;
                    if(raiz->esquerda != NULL){
                        substituto = raiz->esquerda;
                    }else{
                        substituto = raiz->direita;
                    }
                    free(raiz);
                    printf("o valor %d foi removido", n);
                    return substituto;
                }
            }

        }else{
            if(n < raiz->valor){
                raiz->esquerda = remover(raiz->esquerda, n);
            }else{
                raiz->direita = remover(raiz->direita, n);
            }
            return raiz;
        }
    }

}
void printar(Arvore *raiz){
    if(raiz){
        printf("%d ", raiz->valor);
        printar(raiz->esquerda);
        printar(raiz->direita);
    }
} /* printa o primeiro elemento da raiz, e depois chama a propria funcao pra printar os elementos da esquerda e direita da raiz até o final*/

void printarOrdem(Arvore *raiz){
    if(raiz){
        printarOrdem(raiz->esquerda);
        printf("%d ", raiz->valor);
        printarOrdem(raiz->direita);
    }
} /* printa em ordem, printando primeiro os elementos a esquerda (menores), depois a raiz e por fim os elementos da direita.*/

void printarPO(Arvore *raiz){
    if(raiz){
        printarPO(raiz->esquerda);
        printarPO(raiz->direita);
        printf("%d ", raiz->valor);
    }
} /*printa em pos ordem, printando na seguinte ordem:esquerda->direita->raiz*/

Arvore* busca(Arvore *raiz, int n){
    while(raiz){
        if(n < raiz->valor){
            raiz = raiz->esquerda;
        }else if(n > raiz->valor){
            raiz = raiz->direita;
        }else{
            return raiz;
        }
    }
    return NULL;
}/* verifica se o valor exitse, se existir, ele compara o valor buscado com a raiz, se for maior, ele passa a procurar a partir da 
direita, se for menor, a partir da esqurda.Retorna o valor quando encontrar ele.*/

int main (){
    Arvore *raiz = NULL, *b;
    int o, n;
    do{
        printf(" \n 1-inserir \n 2-remover \n 3-printar \n 4-buscar \n 0-sair");
        scanf("%d", &o);
        switch(o){

            case 1:
            printf("indique o valor que voce deseja inserir:\n");
            scanf("%d", &n);
            insec(&raiz, n);
            break;

            case 2:
            printf("indique o valor que voce deseja remover:\n");
            scanf("%d", &n);
            raiz = remover(raiz, n);
            break;

            case 3:
            printf("Pre-Ordem:\n");
            printar(raiz);
            printf("\n\n");
            printf("Ordem:\n");
            printarOrdem(raiz);
            printf("\n\n");
            printf("Pos-Ordem:\n");
            printarPO(raiz);

            break;
            case 4:
            printf("qual valor voce deseja procurar?\n");
            scanf("%d", &n);
            b = busca(raiz, n);
            if(b){
                printf("\n o valor %d foi encontrado! \n", b->valor);
            } else{
                printf("valor nao encontrado");
            }
            break;
            default:
            if(o != 0){
                printf("valor invalido \n");
            }
        }
    }while(o != 0);

    return 0;
}