/*
Our barbershop has three chairs, three barbers, and a waiting
area that can accommodate four customers on a sofa and that has
standing room for additional customers. Fire codes limit the total
number of customers in the shop to 20.
A customer will not enter the shop if it is filled to capacity with
other customers. Once inside, the customer takes a seat on the sofa
or stands if the sofa is filled. When a barber is free, the customer
that has been on the sofa the longest is served and, if there are any
standing customers, the one that has been in the shop the longest
takes a seat on the sofa. When a customer’s haircut is finished,
any barber can accept payment, but because there is only one cash
register, payment is accepted for one customer at a time. The bar-
bers divide their time among cutting hair, accepting payment, and
sleeping in their chair waiting for a customer.
In other words, the following synchronization constraints apply:
• Customers invoke the following functions in order: enterShop, sitOnSofa,
sitInBarberChair, pay, exitShop.
• Barbers invoke cutHair and acceptPayment.
• Customers cannot invoke enterShop if the shop is at capacity.
• If the sofa is full, an arriving customer cannot invoke sitOnSofa until one
of the customers on the sofa invokes sitInBarberChair.
• If all three barber chairs are busy, an arriving customer cannot invoke
sitInBarberChair until one of the customers in a chair invokes pay.
• The customer has to pay before the barber can acceptPayment.
• The barber must acceptPayment before the customer can exitShop.
*/

// Bibliotecas
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
/*
// System specific semaphore definition.
typedef struct {
  struct _pthread_fastlock __sem_lock;
  int __sem_value;
  _pthread_descr __sem_waiting;
} sem_t;
*/

#define NUM_BARBEIROS 3
#define NUM_CADEIRAS 3
#define NUM_SOFA 4
#define NUM_CLIENTES 25

// Funcoes dos Barbeiros
void *funcaoBarbeiro(void *args);
void cortarCabelo(int ind);
void aceitarPagamento(int ind);
// Funcoes dos clientes
void *funcaoCliente(void *args);
void entrarLoja(int ind);
void sentarSofa();
void sentarCadeiraBarbeiro(int ind);
void pagar(int ind);
void sairLoja(int ind);

// Variaveis globais
int clientes = 0, cadeira = 0, alguemPagando = 0;
sem_t s_mutex, s_sofa, s_barbeiro, s_cliente, s_cadeira, s_dinheiro, s_recibo,
    s_verificaSofa, s_esperandoCorte, s_verificaPagadores;

int fifoClientes[20], fifoPagante[20], posPagante = 0, fifoCadeira[3],
                                       posCadeira = 0;

int main(void) {

  // Inicializacao dos semaforos
  sem_init(&s_mutex, 0, 1);
  sem_init(&s_sofa, 0, 4);
  sem_init(&s_barbeiro, 0, 3);
  sem_init(&s_cadeira, 0, 3);
  sem_init(&s_cliente, 0, 3);
  sem_init(&s_dinheiro, 0, 1);
  sem_init(&s_recibo, 0, 1);
  sem_init(&s_verificaSofa, 0, 1);
  sem_init(&s_esperandoCorte, 0, 0);
  sem_init(&s_verificaPagadores, 0, 1);

  // Variaveis das Threads
  int t, status;

  // -=+=-     Inicializacao dos 3 Barbeiros    -=+=-

  pthread_t barbeiros[NUM_BARBEIROS];
  int argsBarbeiros[NUM_BARBEIROS]; // Vetor usado como parametros para as
                                    // threads

  for (t = 0; t < NUM_BARBEIROS; t++) {
    argsBarbeiros[t] = t;
    status = pthread_create(&barbeiros[t], NULL, funcaoBarbeiro,
                            (void *)&argsBarbeiros[t]);

    if (!status) {
      printf("main criou o Barbeiro %d\n", t);
    } else {
      printf("Falha da criacao do Barbeiro %d (%d)\n", t, status);
      return (1);
    }
  }

  // -=+=-     Inicializacao dos 25 Clientes    -=+=-

  // Observacao: A barbearia tem lotacao de 20 clientes, logo,
  // se tiver mais de 20 clientes, algo deu errado :(
  pthread_t clientes[NUM_CLIENTES];
  int argsClientes[NUM_CLIENTES]; // Vetor usado como parametros para as threads

  for (t = 0; t < NUM_CLIENTES; t++) {
    argsClientes[t] = t;
    status = pthread_create(&clientes[t], NULL, funcaoCliente,
                            (void *)&argsClientes[t]);

    if (!status) {
      printf("Main criou o Cliente %d\n", t);
    } else {
      printf("Falha da criacao do Cliente %d (%d)\n", t, status);
      return (1);
    }
  }

  // -=+=- Aguardando a conclusao dos 3 Barbeiros -=+=-
  for (t = 0; t < NUM_BARBEIROS; t++) {
    // join ignorando o valor de retorno
    status = pthread_join(barbeiros[t], NULL);

    if (!status) {
      printf("Barbeiro %d (%lu) retornou\n", t, (long unsigned)barbeiros[t]);
    } else {
      printf("Erro em pthread_join do Barbeiro %d (%d)\n", t, status);
      return (2);
    }
  }
  // -=+=- Aguardando a conclusao dos 25 Clientes -=+=-
  for (t = 0; t < NUM_CLIENTES; t++) {
    // join ignorando o valor de retorno
    status = pthread_join(clientes[t], NULL);

    if (!status) {
      printf("Cliente %d (%lu) retornou\n", t, (long unsigned)clientes[t]);
    } else {
      printf("Erro em pthread_join do Cliente %d (%d)\n", t, status);
      return (2);
    }
  }

  return 0;
}

void *funcaoBarbeiro(void *args) {
  int *num = (int *)args;
  int ind = *num;
  // printf("Barbeiro %d Iniciando o Trabalho...\n", ind);

  while (1) {
    cortarCabelo(ind);
    aceitarPagamento(ind);
  }

  sleep(1);
  pthread_exit(NULL);
}

void *funcaoCliente(void *args) {
  int *num = (int *)args;
  int ind = *num;
  // printf("Cliente %d existindo...\n", ind);

  // Gerando um valor aleatorio usando o proprio identificador da thread como
  // semente:
  pthread_t tid = pthread_self(); // Obtem o identificador exclusivo da thread
  unsigned int seed =
      (unsigned int)tid; // Usa o identificador como parte da semente
  srand(seed); // Inicializa a semente com o identificador exclusivo da thread
               // int tempoAleatorio =
  //     rand() % 300 + 5; // Gera um número aleatório entre 1 e 300

  // Os clientes vao esperar entre 5 seg a 5 min e entrar
  // while (1) {
  // sleep(tempoAleatorio);
  entrarLoja(ind);
  //}
  // obs: trocar tempoAleatorio por 1 para ver o que acontece se todos cliente
  // tentar entrar ao mesmo tempo
  sleep(1);
  pthread_exit(NULL);
}

void entrarLoja(int ind) {
  sem_wait(&s_mutex);   // 1
  if (clientes >= 20) { // se estiver cheio, cliente vai embora
    sem_post(&s_mutex); // unlock
    printf("Barbearia cheia, cliente %d saindo...\n", ind);
  } else {
    fifoClientes[clientes++] = ind; // senao, adiciona no fifo
    sem_post(&s_mutex);             // unlock
    sentarSofa(ind);
  }
}

void sentarSofa(int ind) {
  sem_wait(&s_sofa); // 4
  sleep(1);
  printf("Cliente %d sentou no sofa\n", ind);
  sentarCadeiraBarbeiro(ind);
}

void sentarCadeiraBarbeiro(int ind) {
  sem_wait(&s_cadeira); // 3
  sleep(1);
  printf("Cliente %d sentou na cadeira\n", ind);
  fifoCadeira[posCadeira++] = ind; // tem gente na cadeira
  sem_post(&s_sofa);
  sem_wait(&s_esperandoCorte); // 0 (travar o cliente da cadeira)
  pagar(ind);
}

void pagar(int ind) { // tem gente para pagar
  fifoPagante[posPagante++] = ind;
  sleep(1);
  printf("Cliente %d pagou\n", ind);
  sem_wait(&s_dinheiro); // 0
  sairLoja(ind);
}

void sairLoja(int ind) {
  sleep(1);
  printf("Cliente %d saiu da loja.\n", ind);
  clientes--;
}

void cortarCabelo(int ind) { // Acho que aqui precisa receber como parametro o
                             // numero do cliente que ele vai cortar ou a fila
                             // mesmo sem_post(&semaphore);
  sem_wait(&s_verificaSofa); // 1
  if (posCadeira > 0) {
    sleep(3);

    printf("Barbeiro: %d esta cortando cabelo do Cliente: %d\n", ind, fifoCadeira[0]);
    memcpy(fifoCadeira, &fifoCadeira[1], (posCadeira - 1) * sizeof(int));
    posCadeira--;

    sem_post(&s_verificaSofa); // 1

    int tempoAleatorio = (rand() % 5) + 2;
    sleep(tempoAleatorio);     // corta por [2..7] segundos
    
    sem_post(&s_esperandoCorte); // libera o cliente que tava travado na cadeira
    sem_post(&s_cadeira);
  } else {
    sem_post(&s_verificaSofa);
  }
}

void aceitarPagamento(int ind) {
  sem_wait(&s_verificaPagadores); // 1
  if (posPagante > 0) {
    sleep(1);
    printf("Barbeiro: %d recebeu de Cliente: %d\n", ind, fifoPagante[0]);
    memcpy(fifoPagante, &fifoPagante[1], (posPagante - 1) * sizeof(int));
    posPagante--;
    sem_post(&s_dinheiro);
    sem_post(&s_verificaPagadores);
  } else {
    sem_post(&s_verificaPagadores);
  }
}