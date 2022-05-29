/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "client.h"
#include "messages-private.h"
#include "metime.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>


/* Função principal de um Cliente. Deve executar um ciclo infinito onde em
* cada iteração lê uma operação da main e se for dirijida ao cliente client_id
* e data->terminate ainda for igual a 0, processa-a e guarda o resultado no histórico
* de operações da Main. Operações com id igual a -1 são ignoradas (op inválida) e se
* data->terminate for igual a 1 é porque foi dada ordem de terminação do programa,
* portanto deve-se fazer return do o número de operações processadas. Para efetuar
* estes passos, pode usar os outros métodos auxiliares definidos em client.h.
*/
int execute_client(int client_id, struct communication_buffers* buffers,
  struct main_data* data, struct semaphores* sems)
  {
    int n_ops = 0;
    struct operation op;

    signal(SIGINT, SIG_IGN);

    while (1)
    {
      client_get_operation(&op, client_id, buffers, data, sems);

      if( *(data->terminate) )
      break;


      if(op.id != -1)
      {
        printf("\n\n %s\n%s%s C#%03d %s #%03d!\n %s\n\n%c ",
        WARNING_DELIMITER,
        SPACE, "Client", client_id, "received request", op.id,
        WARNING_DELIMITER,
        '>');
        fflush(stdout);

        client_process_operation(&op, client_id, data, &n_ops, sems);
      }
     else
        produce_end(sems -> driv_cli);

    }

    return n_ops;
  }


  /* Função que lê uma operação do buffer de memória partilhada entre os motoristas
  * e clientes que seja direcionada a client_id. Antes de tentar ler a operação, deve
  * verificar se data->terminate tem valor 1. Em caso afirmativo, retorna imediatamente da função.
  */
  void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
  {
    if( *(data->terminate) )
      return;

    consume_begin(sems -> driv_cli); //o processo espera que haja dados para ler
    read_driver_client_buffer(buffers->driv_cli, client_id, data->buffers_size, op);

  }


  /* Função que processa uma operação, alterando o seu campo receiving_client para o id
  * passado como argumento, alterando o estado da mesma para 'C' (client), e
  * incrementando o contador de operações. Atualiza também a operação na estrutura data.
  */
  void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter, struct semaphores* sems)
  {
    op->status = 'C';
    op->receiving_client = client_id;
    op->client_end_time = getTimeCLock();
    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
    (*counter)++;
    consume_end(sems -> driv_cli); // o processo notifica o produtor que acabou de ler, i.e., tem uma slot vazio
  }
