#include "BTree.h"

#define DEBUG 0

int main()
{
    // Criando a Arvore B:
    ArvB *RaizT = arvB_cria();

    // Menu do usuario
    int seletor;
    int elemento = 0;
    int retorno;
    do
    {
        printf("\n        -=+=- Menu -=+=-\n");
        printf("[0] Sair\n");
        printf("[1] Inserir elemento na árvore\n");
        printf("[2] Remover elemento da árvore\n");
        printf("[3] Printar toda a árvore\n");
        printf("[4] Buscar elemento na árvore\n");
        printf("[5] Printar qtd de nos na árvore\n");
        printf("[6] Printar qtd de chaves na árvore\n");
        printf("[7] Destruir a árvore (sair do programa)\n");
        printf("[8] Verificar arvore (DEBUG)\n");
        printf("              -=+=-\n");

        printf("Escolha a acao que sera realizada: ");
        scanf("%d", &seletor);
        switch (seletor)
        {
        case 0:
            printf("Saindo!\n");
            arvB_destroi(RaizT);
            break;
        case 1:
            printf("Digite o elemento que quer INSERIR: ");
            scanf("%d", &elemento);
            retorno = insere(RaizT, elemento);
            if (retorno)
            {
                printf("\nElemento %d inserido com sucesso!\n", elemento);
            }
            else
            {
                printf("\nErro ao inserir elemento!\n");
            }
            break;
        case 2:
            printf("Digite o elemento que quer REMOVER: ");
            scanf("%d", &elemento);
            retorno = remover(RaizT, elemento);
            if (retorno)
                printf("\nElemento %d removido com sucesso!\n", elemento);
            else
            {
                printf("\nErro ao remover elemento!\n");
            }
            break;
        case 3:
            printf("\nPrintando toda a árvore ('#' = nível do nó):\n");
            print(RaizT);
            break;
        case 4:
            printf("Digite o elemento que quer BUSCAR: ");
            scanf("%d", &elemento);
            retorno = arvB_busca(RaizT, elemento);
            if (retorno)
                printf("\nO elemento %d esta na árvore!\n", elemento);
            else
                printf("\nO elemento %d NAO esta na árvore!\n", elemento);
            break;
        case 5:
            printf("Quantidade de nós é: %d\n", arvB_qtd_nos(RaizT));
            break;
        case 6:
            printf("Quantidade de chaves é: %d\n", arvB_qtd_chaves(RaizT));
            break;
        case 7:
            printf("Destruindo a árvore... \n");
            arvB_destroi(RaizT);
            printf("Árvore destruida! \n");
            printf("Saindo do programa! \n");
            break;
        case 8:
            printf("DEBUGGING\n");
            retorno = debug_verificar_qtd(RaizT);
            if (retorno)
                printf("tudo certo\n");
            else
                printf("erro\n");
            break;
        default:
            printf("Opção invalida! Digite novamente\n");
            break;
        }
    } while (seletor != 0 && seletor != 7);

    return 0;
}