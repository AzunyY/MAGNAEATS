/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "driver.h"
#include "messages-private.h"
#include "metime.h"
#include <stdio.h>
#include <time.h>

/* Função principal de um Motorista. Deve executar um ciclo infinito onde em
* cada iteração lê uma operação dos restaurantes e se a mesma tiver id
* diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
* escreve a resposta para os clientes. Operações com id igual a -1 são
* ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi
* dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em driver.h.
*/
int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    int n_ops = 0;
    struct operation op;

    while ( !*(data->terminate) )
    {
        driver_receive_operation(&op, buffers, data, sems);

        if( *(data->terminate) )
            break;

        if(op.id != -1)
        {
            printf("\n\n %s\n%s%s D#%03d %s #%03d!\n %s\n\n%c ",
                WARNING_DELIMITER,
                SPACE, "Driver", driver_id, "received request", op.id,
                WARNING_DELIMITER,
                '>');
            fflush(stdout);

            driver_process_operation(&op, driver_id, data, &n_ops, sems);
            driver_send_answer(&op, buffers, data, sems);
        }
    }
    return n_ops;
}


/* Função que lê uma operação do buffer de memória partilhada entre restaurantes e motoristas.
* Antes de tentar ler a operação, deve verificar se data->terminate tem valor 1.
* Em caso afirmativo, retorna imediatamente da função.
*/
void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    consume_begin(sems->rest_driv);
    read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    if(op->id == -1)
    {
        produce_end(sems->rest_driv);
    }
    else
    {
        consume_end(sems->rest_driv);
    }
}


/* Função que processa uma operação, alterando o seu campo receiving_driver para o id
* passado como argumento, alterando o estado da mesma para 'D' (driver), e
* incrementando o contador de operações. Atualiza também a operação na estrutura data.
*/
void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter, struct semaphores* sems)
{
    op->status = 'D';
    op->receiving_driver = driver_id;
    op->driver_time = getTimeCLock();

    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);

    (*counter)++;
}


/* Função que escreve uma operação no buffer de memória partilhada entre
* motoristas e clientes.
*/
void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    /*apenas põe a operação no buffer lido pelos clientes se o cliente pedido pela operação for válido*/
    if(op->requesting_client < data->n_clients)
    {
        produce_begin(sems->driv_cli);
        write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
        produce_end(sems->driv_cli);
    }
}
