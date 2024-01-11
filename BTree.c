#include "BTree.h"

#define DEBUG 0

// funcoes auxiliares

ArvB *cria_noh()
{
  ArvB *noh = (ArvB *)malloc(sizeof(ArvB));
  *noh = (ArvB)malloc(sizeof(struct NO));
  (*noh)->qtd = 0;
  (*noh)->valores = (int *)calloc(
      ordem, sizeof(int)); // aqui nao eh ordem - 1 pq eu quero acomodar valor
                           // a mais para detectar overflow e tratar
  (*noh)->filhos = (ArvB *)calloc(ordem + 1, sizeof(struct NO));
  return noh;
}

void destroi_noh(ArvB *raiz)
{
  free((*raiz)->filhos);
  free((*raiz)->valores);
  free(*raiz);
}

void print(ArvB *raiz) { print_arvore(raiz, 0); }

void print_arvore(ArvB *raiz, int lvl)
{
  if (*raiz == NULL)
    return;

  for (int i = 0; i < lvl; i++)
  {
    printf("#"); // printa o nivel da arvore
  }
  printf(" ");
  for (int i = 0; i < (*raiz)->qtd; i++)
  {
    printf("%d ", (*raiz)->valores[i]);
  }
  printf("\n");

  for (int i = 0; i < (*raiz)->qtd + 1; i++)
  {
    print_arvore(&(*raiz)->filhos[i], lvl + 1);
  }
}

void split_noh(ArvB *raiz, ArvB *pai)
{                                       // 1 to 2
  int m = ordem >> 1;                   // shift 1 bit para a direita == divisao por 2
  int valor_meio = (*raiz)->valores[m]; // divisor, q vai ser promovido
  ArvB *nohEsq = cria_noh();
  ArvB *nohDir = cria_noh();
  // passando os valores e filhos do noh original para os dois em que ele se
  // dividirah
  int i;
  for (i = 0; i < m; i++)
  {
    (*nohEsq)->valores[i] = (*raiz)->valores[i];
    (*nohEsq)->filhos[i] = (*raiz)->filhos[i];
  }
  (*nohEsq)->filhos[i] = (*raiz)->filhos[i];
  (*nohEsq)->qtd = i;

  int index = 0;
  for (i = m + 1; i < ordem; i++, index++)
  {
    (*nohDir)->valores[index] = (*raiz)->valores[i];
    (*nohDir)->filhos[index] = (*raiz)->filhos[i];
  }
  (*nohDir)->filhos[index] = (*raiz)->filhos[i];
  (*nohDir)->qtd = index;

  // faz a inserção do elemento do meio no noh pai
  int posFatherInsertion;
  for (posFatherInsertion = (*pai)->qtd - 1;
       posFatherInsertion >= 0 &&
       (*pai)->valores[posFatherInsertion] > valor_meio;
       posFatherInsertion--)
  {
    (*pai)->valores[posFatherInsertion + 1] =
        (*pai)->valores[posFatherInsertion];
  }
  (*pai)->valores[posFatherInsertion + 1] = valor_meio;

  for (posFatherInsertion = (*pai)->qtd;
       posFatherInsertion > 0 &&
       (*pai)->valores[posFatherInsertion] > valor_meio;
       posFatherInsertion--)
  {
    (*pai)->filhos[posFatherInsertion + 1] = (*pai)->filhos[posFatherInsertion];
  }
  (*pai)->qtd++;

  // coloca o novo filho (nohEsq) resultado da divisao do noh original (*raiz) e
  // substitui o noh original (*raiz) por um com valores e filhos divididos com
  // o novo (nohDir)
  (*pai)->filhos[posFatherInsertion] = (*nohEsq);
  (*pai)->filhos[posFatherInsertion + 1] = (*nohDir);
}

void split_noh_raiz(ArvB *raiz)
{ // caso especifico de split na raiz
  int m = (ordem - 1) >> 1;
  int valor_meio = (*raiz)->valores[m];
  ArvB *nohEsq = cria_noh();
  ArvB *nohDir = cria_noh();
  ArvB *novaRaiz = cria_noh();

  // cria uma nova raiz com o valor_meio sendo seu primeiro valor
  (*novaRaiz)->valores[0] = valor_meio;
  (*novaRaiz)->qtd = 1;

  // apos criacao da nova raiz, setamos antiga raiz e um novo noh como seus
  // filhos
  int i;
  for (i = 0; i < m; i++)
  {
    (*nohEsq)->valores[i] = (*raiz)->valores[i];
    (*nohEsq)->filhos[i] = (*raiz)->filhos[i];
  }
  (*nohEsq)->filhos[i] = (*raiz)->filhos[i];
  (*nohEsq)->qtd = i;

  int index = 0;
  for (i = m + 1; i < ordem; i++, index++)
  {
    (*nohDir)->valores[index] = (*raiz)->valores[i];
    (*nohDir)->filhos[index] = (*raiz)->filhos[i];
  }
  (*nohDir)->filhos[index] = (*raiz)->filhos[i];
  (*nohDir)->qtd = index;

  (*novaRaiz)->filhos[0] = (*nohEsq);
  (*novaRaiz)->filhos[1] = (*nohDir);

  (*raiz) = (*novaRaiz);

  free(novaRaiz);
}

/*Funcoes do trabalho */
ArvB *arvB_cria()
{
  ArvB *raiz = cria_noh();
  return raiz;
}

void arvB_destroi(ArvB *raiz)
{
  if ((*raiz) == NULL)
  {
    return;
  }
  for (int i = 0; i <= (*raiz)->qtd; i++)
  {
    arvB_destroi(&(*raiz)->filhos[i]);
  }
  destroi_noh(raiz);
}

int insere(ArvB *raiz, int valor)
{
  int result = arvB_insere(raiz, valor);
  if (result == 1 && (*raiz)->qtd == ordem) // overflow na raiz
  {
    split_noh_raiz(raiz);
    return 1;
  }
  return result;
}

int arvB_insere(ArvB *raiz, int valor)
{
  // Se o noh estiver vazio
  if ((*raiz)->qtd == 0)
  {
    (*raiz)->valores[0] = valor;
    (*raiz)->qtd = 1;
    return 1;
  }

  int i = 0;
  int result = 0;

  if ((*raiz)->filhos[0] == NULL)
  { // se o noh eh folha
    // shiftar o vetor para poder inserir
    for (i = 0; i < (*raiz)->qtd; i++) // verificar se ele esta no vetor
    {
      if ((*raiz)->valores[i] == valor)
        return 0;
    }
    for (i = (*raiz)->qtd - 1; i >= 0 && (*raiz)->valores[i] > valor; i--)
    {
      (*raiz)->valores[i + 1] = (*raiz)->valores[i];
    }
    (*raiz)->qtd++;
    (*raiz)->valores[i + 1] = valor;
    return 1;
  }
  else
  { // noh nao eh folha
    while ((*raiz)->valores[i] <= valor && i < (*raiz)->qtd)
    {
      if ((*raiz)->valores[i] == valor)
      {
        return 0;
      }
      i++;
    }
    // desce recursivamente ateh encontrar o valor
    result = arvB_insere(&(*raiz)->filhos[i], valor);
  }

  if ((*raiz)->filhos[i]->qtd >= ordem) // overflow no noh
  {
    split_noh(&(*raiz)->filhos[i], &(*raiz));
  }
  return result;
}

int remover(ArvB *raiz, int valor)
{
  int back = arvB_remove(raiz, valor);
  if (back == 1 && (*raiz)->qtd == 0)
  { // se o tratamento de underflow deixou a
    // raiz sem elementos, rearranja a raiz
    // printf(" %d ", (*raiz)->qtd);
    (*raiz) = (*raiz)->filhos[0];
  }
  return back;
}

int arvB_remove(ArvB *raiz, int valor)
{
  if ((*raiz) == NULL || (*raiz)->qtd == 0)
  { // se der falha na remocao
    return 0;
  }

  int removeu = 0;
  int i = 0;
  while (i < (*raiz)->qtd && (*raiz)->valores[i] <= valor)
  {
    if ((*raiz)->valores[i] == valor)
    {
      if ((*raiz)->filhos[0] == NULL)
      { // se eh folha
        for (int j = i; j < (*raiz)->qtd; j++)
        {
          (*raiz)->valores[j] =
              (*raiz)->valores[j + 1]; // shifta o vetor para a esquerda
        }
        (*raiz)->qtd--;
        return 1;
      }
      else
      { // se n for folha, "troca" com o antecessor imediato
        ArvB *Aux = &(*raiz)->filhos[i];
        while ((*Aux)->filhos[(*Aux)->qtd] != NULL)
        {
          Aux = &(*Aux)->filhos[(*Aux)->qtd];
        }
        (*raiz)->valores[i] = (*Aux)->valores[(*Aux)->qtd - 1];
        removeu =
            arvB_remove(&(*raiz)->filhos[i], (*Aux)->valores[(*Aux)->qtd - 1]);
        break;
      }
    }
    i++;
  }
  // n achou naquele no, vai pra outro
  if (removeu == 0)
  {
    removeu = arvB_remove(&(*raiz)->filhos[i], valor);
  }

  if ((*raiz)->filhos[i]->qtd < ordem >> 1) // underflow
  {

    // se algum irmao adjacente tem mais elemento que o minimo,
    // fazer redistribuicao
    if (i > 0 &&
        (*raiz)->filhos[i - 1]->qtd > ordem >> 1)
    { // redistribuicao esq
      redistribuicao(&(*raiz)->filhos[i], &(*raiz)->filhos[i - 1], raiz);
    }
    else if (i < (*raiz)->qtd &&
             (*raiz)->filhos[i + 1]->qtd > ordem >> 1)
    { // redistribuicao dir
      redistribuicao(&(*raiz)->filhos[i + 1], &(*raiz)->filhos[i], raiz);
    }
    // se nenhum irmao adjacente tem mais elemento que o minimo, deve-se
    // realizar a concatenacao
    else if (i > 0 && (*raiz)->filhos[i - 1]->qtd <= ordem >>
                          1)
    { // concatenacao esquerda
      concatena(&(*raiz)->filhos[i], &(*raiz)->filhos[i - 1], raiz);
    }
    else
    { // concatenacao direita
      concatena(&(*raiz)->filhos[i + 1], &(*raiz)->filhos[i], raiz);
    }
  }
  return removeu;
}

void redistribuicao(ArvB *filhoDir, ArvB *filhoEsq, ArvB *pai)
{
  int qtd = (*filhoDir)->qtd + (*filhoEsq)->qtd + 1;
  int aux[qtd];
  int posPai;

  // encontra, no pai, o valor entre os dois filhos
  for (posPai = 0; (*filhoEsq) != (*pai)->filhos[posPai]; posPai++)
    ;

  int index = 0;
  for (int i = 0; i < (*filhoEsq)->qtd;
       i++)
  { // coloca os elementos do filhoEsq no vetor auxiliar
    aux[index++] = (*filhoEsq)->valores[i];
  }
  aux[index++] = (*pai)->valores[posPai]; // coloca o elemento do pai que
                                          // divide os filhos no vetor
  for (int i = 0; i < (*filhoDir)->qtd; i++)
  {
    aux[index++] = (*filhoDir)->valores[i]; // coloca os elementos do filhoDir no vetor
  }
  // substitui o valor que estava no pai pelo valor mediano dentre todos os
  // valores e shifta o vetor apos a insercao do elemento mediano no pai
  qtd--;
  int mid = qtd >> 1;
  (*pai)->valores[posPai] = aux[mid];
  for (int i = mid; i < qtd; i++)
  {
    aux[i] = aux[i + 1];
  }

  // divide os valores do vetor auxiliar entre os filhoEsq, igualmente (ou
  // quase isso)
  int posF1, posF2 = 0;
  index = 0;
  for (posF1 = 0; index < mid; posF1++)
  {
    (*filhoEsq)->valores[posF1] = aux[index++];
  }
  for (posF2 = 0; index < qtd; posF2++)
  {
    (*filhoDir)->valores[posF2] = aux[index++];
  }
  (*filhoDir)->qtd = posF1;
  (*filhoEsq)->qtd = posF2;
}

void concatena(ArvB *filhoDir, ArvB *filhoEsq, ArvB *pai)
{
  int posPai;
  // encontra, no pai, o valor entre os dois filhos
  for (posPai = 0; (*filhoEsq) != (*pai)->filhos[posPai]; posPai++)
    ;

  // coloca o elemento do pai que divide os filhos no vetor
  (*filhoEsq)->valores[(*filhoEsq)->qtd++] = (*pai)->valores[posPai];

  // passa o primeiro filho do nó a direita que será o filho direito do elemento vindo do pai
  (*filhoEsq)->filhos[(*filhoEsq)->qtd] = (*filhoDir)->filhos[0];
  int i;
  for (i = 0; i < (*filhoDir)->qtd; i++)
  {
    // coloca os elementos do filhoDir no vetor
    (*filhoEsq)->valores[(*filhoEsq)->qtd] = (*filhoDir)->valores[i];
    (*filhoEsq)->filhos[++(*filhoEsq)->qtd] = (*filhoDir)->filhos[i + 1];
  }

  (*pai)->qtd--;
  for (int i = posPai; i < (*pai)->qtd; i++)
  {
    (*pai)->valores[i] = (*pai)->valores[i + 1];
    // shiftando para a esquerda os filhos e valores do pai, pois um elemento "desceu"
    (*pai)->filhos[i + 1] = (*pai)->filhos[i + 2];
  }
}

int arvB_busca(ArvB *raiz, int valor)
{
  if ((*raiz) == NULL || (*raiz)->qtd == 0) // se for folha
  {
    return 0;
  }
  int i = 0;
  while (i < (*raiz)->qtd && (*raiz)->valores[i] <= valor)
  {
    if ((*raiz)->valores[i] == valor)
      return 1;
    i++;
  }
  // n achou naquele no, vai pra outro
  return arvB_busca(&(*raiz)->filhos[i], valor);
}

int arvB_qtd_nos(ArvB *raiz)
{
  if ((*raiz) == NULL)
  {
    return 0;
  }
  int sum = 1;
  for (int i = 0; i <= (*raiz)->qtd; i++)
  {
    sum += arvB_qtd_nos(&(*raiz)->filhos[i]);
  }
  return sum;
}

int arvB_qtd_chaves(ArvB *raiz)
{
  if ((*raiz) == NULL)
  {
    return 0;
  }
  int sum = (*raiz)->qtd;
  for (int i = 0; i <= (*raiz)->qtd; i++)
  {
    sum += arvB_qtd_chaves(&(*raiz)->filhos[i]);
  }
  return sum;
}

int debug_verificar_qtd(ArvB *raiz)
{
  if ((*raiz) == NULL) // caso base
  {
    return 1;
  }
  for (int i = 0; i <= (*raiz)->qtd; i++)
  {
    debug_verificar_qtd(&(*raiz)->filhos[i]);
  }
  if ((*raiz)->qtd >= ordem)
  {
    printf("noh com %d unidades, noh com valor %d", (*raiz)->qtd,
           (*raiz)->valores[0]);
    return 0;
  }
  return 1;
}
