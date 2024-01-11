#ifndef ARVOREB_H
#define ARVOREB_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ordem 7 // número máximo de filhos

typedef struct NO {
  int *valores;
  int qtd;
  struct NO **filhos;
} * ArvB;

// Funcoes padroes
ArvB *arvB_cria();
void destroi_noh(ArvB *raiz);
void arvB_destroi(ArvB *raiz);
int insere(ArvB *raiz, int valor);
int arvB_insere(ArvB *raiz, int valor);
int remover(ArvB *raiz, int valor);
int arvB_remove(ArvB *raiz, int valor);
int arvB_busca(ArvB *raiz, int valor);
int arvB_qtd_nos(ArvB *raiz);
int arvB_qtd_chaves(ArvB *raiz);

// Funcoes auxiliares
ArvB *cria_noh();
void print(ArvB *raiz);
void print_arvore(ArvB *raiz, int lvl);
void split_noh(ArvB *raiz, ArvB *pai);
void split_noh_raiz(ArvB *raiz);
void redistribuicao(ArvB *filhoDir, ArvB *filhoEsq, ArvB *pai);
void concatena(ArvB *dest, ArvB *src, ArvB *pai);
int debug_verificar_qtd(ArvB *raiz);

#endif // ARVOREB_H