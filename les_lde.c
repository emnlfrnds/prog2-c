#include <stdio.h>
#include <stdlib.h>

// Nó para Lista Encadeada Simples (LES)
typedef struct No {
  int valor;
  struct No *proximo;
} No;

// Nó para Lista Duplamente Encadeada (LDE)
typedef struct NoDuplo {
  int valor;
  struct NoDuplo *proximo;
  struct NoDuplo *anterior;
} NoDuplo;

// Aloca memória e inicializa um nó simples
No *criar_no(int valor) {
  No *novo_no = (No *)malloc(sizeof(No));

  if (novo_no) {
    novo_no->valor = valor;
    novo_no->proximo = NULL;
  }

  return novo_no;
}

void inserir_posicao_les(No **cabeca, int valor, int posicao) {
  if (posicao < 0)
    return;

  No *novo = criar_no(valor);
  if (!novo)
    return;

  // Caso inserção no início (índice 0)
  if (posicao == 0) {
    novo->proximo = *cabeca;
    *cabeca = novo;
    return;
  }

  No *atual = *cabeca;
  // Percorre até achar o nó anterior à posição desejada
  for (int i = 0; atual != NULL && i < posicao - 1; i++) {
    atual = atual->proximo;
  }

  // Posição inválida
  if (!atual) {
    free(novo);
    return;
  }

  novo->proximo = atual->proximo;
  atual->proximo = novo;
}

// Libera toda a memória da lista
void liberar_les(No **cabeca) {
  No *atual = *cabeca;

  while (atual) {
    No *proximo = atual->proximo;
    free(atual);
    atual = proximo;
  }

  *cabeca = NULL;
}

// Remove um elemento específico buscando pelo valor
void remover_les(No **cabeca, int valor) {
  if (!*cabeca)
    return;

  No *temp = *cabeca;

  // O alvo é o primeiro nó da lista
  if (temp->valor == valor) {
    *cabeca = temp->proximo;
    free(temp);
    return;
  }

  No *anterior = NULL;

  // Passa pela lista até achar o valor ou chegar no NULL
  while (temp && temp->valor != valor) {
    anterior = temp;
    temp = temp->proximo;
  }

  if (!temp)
    return; // Não achou

  // O valor está no meio ou no fim
  anterior->proximo = temp->proximo;
  free(temp);
}

// Busca um valor na LES e retorna sua posição (0-based), ou -1 se não achar
int buscar_valor_les(No **cabeca, int valor) {
  No *atual = *cabeca;
  int posicao = 0;

  while (atual) {
    if (atual->valor == valor)
      return posicao;
    atual = atual->proximo;
    posicao++;
  }

  return -1;
}

void listar_NoSimples(No **inicio) {
  No *atual = *inicio;
  while (atual != NULL) {
    printf("%d ", atual->valor);
    atual = atual->proximo;
  }
  printf("\n");
}

void apagar_Primeiro_NoSimples(No **cabeca) {
  if (!*cabeca)
    return;

  No *tmp = *cabeca;
  *cabeca = (*cabeca)->proximo;
  free(tmp);
}

void push_front_simples(No **cabeca, int valor) {
  No *novo = criar_no(valor);

  // Verifica se o nó é null
  if (!novo)
    return;

  // Avisa que o novo cabeça da lista eh ele
  novo->proximo = *cabeca;
  *cabeca = novo;
}

void push_back_simples(No **cabeca, int valor) {
  No *novo = criar_no(valor);
  // Verifica se o nó é null
  if (!novo)
    return;

  // Verifica se nao tem nenhum nó na lista
  if (*cabeca == NULL) {
    *cabeca = novo;
    return;
  }

  // Procura o ultimo nó da lista
  No *aux = *cabeca;
  while (aux->proximo != NULL) {
    aux = aux->proximo;
  }

  // Adiciona o novo nó no final da lista que foi encontrado
  aux->proximo = novo;
}

// Aloca memória e inicializa um nó duplo
NoDuplo *criar_noDuplo(int valor) {
  NoDuplo *novo_no = (NoDuplo *)malloc(sizeof(NoDuplo));

  if (novo_no) {
    novo_no->valor = valor;
    novo_no->proximo = NULL;
    novo_no->anterior = NULL;
  }

  return novo_no;
}

void listar_NoDuplo(NoDuplo **inicio) {
  NoDuplo *atual = *inicio;
  while (atual != NULL) {
    printf("%d ", atual->valor);
    atual = atual->proximo;
  }
  printf("\n");
}

void apagar_Primeiro_NoDuplo(NoDuplo **cabeca) {
  if (!*cabeca)
    return;

  NoDuplo *tmp = *cabeca;
  *cabeca = (*cabeca)->proximo;

  if (*cabeca)
    (*cabeca)->anterior = NULL;

  free(tmp);
}

void push_front_duplo(NoDuplo **cabeca, int valor) {
  NoDuplo *novo = criar_noDuplo(valor);
  if (!novo)
    return;

  novo->proximo = *cabeca;

  // Se a lista nao estiver vazia, ele liga pra trás tbm
  if (*cabeca != NULL) {
    (*cabeca)->anterior = novo;
  }

  // Faz o ponteiro principal da lista apontar pro novo
  *cabeca = novo;
}

void push_back_duplo(NoDuplo **cabeca, int valor) {
  NoDuplo *novo = criar_noDuplo(valor);
  if (!novo)
    return;

  // Ve se a lista nao ta vazia
  if (*cabeca == NULL) {
    *cabeca = novo;
    return;
  }

  // Como no simples, ele procura o ultimo elemento
  NoDuplo *aux = *cabeca;
  while (aux->proximo != NULL) {
    aux = aux->proximo;
  }

  // Faz o encadeamento duplo do ultimo elemento com o novo
  aux->proximo = novo;
  novo->anterior = aux;
}

// Insere em uma posição específica na Lista Duplamente Encadeada

void inserir_posicao_lde(NoDuplo **cabeca, int valor, int posicao) {
  if (posicao < 0)
    return;

  NoDuplo *novo = criar_noDuplo(valor);
  if (!novo)
    return;

  // Caso inserção no início (índice 0)
  if (posicao == 0) {
    novo->proximo = *cabeca;

    if (*cabeca) {
      (*cabeca)->anterior = novo;
    }

    *cabeca = novo;

    return;
  }

  NoDuplo *atual = *cabeca;
  // Percorre até achar o nó anterior à posição desejada
  for (int i = 0; atual != NULL && i < posicao - 1; i++) {
    atual = atual->proximo;
  }

  // Posição inválida
  if (!atual) {
    free(novo);
    return;
  }

  novo->proximo = atual->proximo;
  novo->anterior = atual;

  if (atual->proximo) {
    atual->proximo->anterior = novo;
  }

  atual->proximo = novo;
}

// Libera a memória da LDE
void liberar_lde(NoDuplo **cabeca) {
  NoDuplo *atual = *cabeca;

  while (atual) {
    NoDuplo *proximo = atual->proximo;
    free(atual);
    atual = proximo;
  }
  *cabeca = NULL;
}

// Remove elemento da lista dupla reconectando vizinhos
void remover_lde(NoDuplo **cabeca, int valor) {
  if (!*cabeca)
    return;

  NoDuplo *temp = *cabeca;

  // Busca o nó a ser removido
  while (temp && temp->valor != valor) {
    temp = temp->proximo;
  }

  if (!temp)
    return; // Não achou

  // O alvo é o primeiro, move a cabeça
  if (temp == *cabeca) {
    *cabeca = temp->proximo;
  }

  // Reconecta o nó anterior com o próximo (pula o temp)
  if (temp->proximo) {
    temp->proximo->anterior = temp->anterior;
  }

  // Reconecta o próximo com o anterior
  if (temp->anterior) {
    temp->anterior->proximo = temp->proximo;
  }

  free(temp);
}

// Busca um valor na LDE e retorna sua posição (0-based), ou -1 se não achar
int buscar_valor_lde(NoDuplo **cabeca, int valor) {
  NoDuplo *atual = *cabeca;
  int posicao = 0;

  while (atual) {
    if (atual->valor == valor)
      return posicao;
    atual = atual->proximo;
    posicao++;
  }

  return -1;
}

void menuSimples(No **lista) {
  int opcao = -1, valor, pos;

  while (opcao != 0) {
    printf("\n=== MENU LISTA SIMPLESMENTE ENCADEADA ===\n");
    printf("1. Inserir em posição específica\n");
    printf("2. Inserir no final\n");
    printf("3. Remover um valor\n");
    printf("4. Buscar valor\n");
    printf("5. Listar\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
    case 1:
      printf("Digite o valor: ");
      scanf("%d", &valor);
      printf("Digite a posição (a partir de 1): ");
      scanf("%d", &pos);
      inserir_posicao_les(lista, valor, pos - 1);
      break;
    case 2:
      printf("Digite o valor: ");
      scanf("%d", &valor);
      push_back_simples(lista, valor);
      printf("Nó inserido no final da lista.\n");
      break;
    case 3:
      printf("Digite o valor a ser removido: ");
      scanf("%d", &valor);
      remover_les(lista, valor);
      break;
    case 4:
      printf("Digite o valor para busca: ");
      scanf("%d", &valor);
      pos = buscar_valor_les(lista, valor);
      if (pos != -1) {
        printf("Resultado: Valor %d encontrado na POSIÇÃO %d.\n", valor, pos);
      } else {
        printf("Resultado: Valor %d NÃO encontrado na lista.\n", valor);
      }
      break;
    case 5:
      listar_NoSimples(lista);
      break;
    case 0:
      liberar_les(lista);
      printf("Memória liberada. Encerrando programa...\n");
      break;
    default:
      printf("Opção inválida! Tente novamente.\n");
    }
  }
}

void menuDuplamente(NoDuplo **lista) {
  int opcao = -1, valor, pos;

  while (opcao != 0) {
    printf("\n=== MENU LISTA DUPLAMENTE ENCADEADA ===\n");
    printf("1. Inserir em posição específica\n");
    printf("2. Inserir no final\n");
    printf("3. Remover um valor\n");
    printf("4. Buscar valor\n");
    printf("5. Listar\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
    case 1:
      printf("Digite o valor: ");
      scanf("%d", &valor);
      printf("Digite a posição (a partir de 1): ");
      scanf("%d", &pos);
      inserir_posicao_lde(lista, valor, pos - 1);
      break;
    case 2:
      printf("Digite o valor: ");
      scanf("%d", &valor);
      push_back_duplo(lista, valor);
      printf("Nó inserido no final da lista.\n");
      break;
    case 3:
      printf("Digite o valor a ser removido: ");
      scanf("%d", &valor);
      remover_lde(lista, valor);
      break;
    case 4:
      printf("Digite o valor para busca: ");
      scanf("%d", &valor);
      pos = buscar_valor_lde(lista, valor);
      if (pos != -1) {
        printf("Resultado: Valor %d encontrado na POSIÇÃO %d.\n", valor, pos);
      } else {
        printf("Resultado: Valor %d NÃO encontrado na lista.\n", valor);
      }
      break;
    case 5:
      listar_NoDuplo(lista);
      break;
    case 0:
      liberar_lde(lista);
      printf("Memória liberada. Encerrando programa...\n");
      break;
    default:
      printf("Opção inválida! Tente novamente.\n");
    }
  }
}

int main() {
  No *simples = NULL;
  NoDuplo *duplo = NULL;

  int resp;
  printf("Deseja utilizar a lista simples(1) ou duplamente "
         "encadeada(2)?\nResposta: ");
  scanf("%d", &resp);

  if (resp == 1) {
    menuSimples(&simples);
  } else if (resp == 2) {
    menuDuplamente(&duplo);
  }

  return 0;
}