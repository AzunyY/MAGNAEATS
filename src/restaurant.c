/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "restaurant.h"
#include "metime.h"
#include "messages-private.h"
#include <stdio.h>

/* Função principal de um Restaurante. Deve executar um ciclo infinito onde em
* cada iteração lê uma operação da main e se e data->terminate ainda for igual a 0, processa-a e
* escreve-a para os motoristas. Operações com id igual a -1 são ignoradas
* (op inválida) e se data->terminate for igual a 1 é porque foi dada ordem
* de terminação do programa, portanto deve-se fazer return do número de
* operações processadas. Para efetuar estes passos, pode usar os outros
* métodos auxiliares definidos em restaurant.h.
*/
int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    int n_ops = 0;
    struct operation op;

    while ( !*(data->terminate) )
    {
        restaurant_receive_operation(&op, rest_id, buffers, data, sems);

        if( *(data->terminate) )
            break;

        if(op.id != -1)
        {
            printf("\n\n %s\n%s%s R#%03d %s #%03d!\n %s\n\n%c ",
                WARNING_DELIMITER,
                SPACE, "Restaurant", rest_id, "received request", op.id,
                WARNING_DELIMITER,
                '>');
            fflush(stdout);

            restaurant_process_operation(&op, rest_id, data, &n_ops, sems);
            restaurant_forward_operation(&op, buffers, data, sems);
        }
    }
    return n_ops;
}

/* Função que lê uma operação do buffer de memória partilhada entre
* a Main e os restaurantes que seja direcionada ao restaurante rest_id.
* Antes de tentar ler a operação, o processo deve
* verificar se data->terminate tem valor 1.
* Em caso afirmativo, retorna imediatamente da função.
*/
void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    consume_begin(sems->main_rest);
    read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
    if(op->id == -1)
    {
        produce_end(sems->main_rest);
    }
    else
    {
        consume_end(sems->main_rest);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_rest para o id
* passado como argumento, alterando o estado da mesma para 'R', e
* incrementando o contador de operações. Atualiza também a operação na estrutura data.
*/
void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter, struct semaphores* sems)
{
    op->status = 'R';
    op->receiving_rest = rest_id;
    op->rest_time = getTimeCLock();

    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);

    (*counter)++;
}

/* Função que escreve uma operação no buffer de memória partilhada entre
* restaurantes e motoristas, efetuando a necessária sincronização antes e
* depois de escrever.
*/
void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    produce_begin(sems->rest_driv);
    write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    produce_end(sems->rest_driv);
}
