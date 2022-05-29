/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "process.h"
#include "restaurant.h"
#include "driver.h"
#include "client.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/* Função que inicia um novo processo restaurante através da função fork do SO. O novo
* processo irá executar a função execute_restaurant respetiva, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_restaurant(int restaurant_id, struct communication_buffers*
  buffers, struct main_data* data, struct semaphores* sems)
  {
    int pid;
    while( (pid = fork()) == -1); /*Garante a criação de um novo processo processo*/

    if(pid == 0)
    {
      int stats = execute_restaurant(restaurant_id, buffers, data, sems);
      exit(stats);
    }

    return pid;
  }

  /* Função que inicia um novo processo motorista através da função fork do SO. O novo
  * processo irá executar a função execute_driver, fazendo exit do retorno.
  * O processo pai devolve o pid do processo criado.
  */
  int launch_driver(int driver_id, struct communication_buffers* buffers,
    struct main_data* data, struct semaphores* sems)
    {
      int pid;
      while( (pid = fork()) == -1); /*Garante a criação de um novo processo processo*/

      if(pid == 0)
      {
        int stats = execute_driver(driver_id, buffers, data, sems);
        exit(stats);
      }

      return pid;
    }

    /* Função que inicia um novo processo cliente através da função fork do SO. O novo
    * processo irá executar a função execute_client, fazendo exit do retorno.
    * O processo pai devolve o pid do processo criado.
    */
    int launch_client(int client_id, struct communication_buffers* buffers,
      struct main_data* data, struct semaphores* sems)
      {
        int pid;
        while( (pid = fork()) == -1); /*Garante a criação de um novo processo processo*/

        if(pid == 0)
        {
          int stats = execute_client(client_id, buffers, data, sems);
          exit(stats);
        }

        return pid;
      }

      /* Função que espera que um processo termine através da função waitpid.
      * Devolve o retorno do processo, se este tiver terminado normalmente.
      */
      int wait_process(int process_id)
      {
        int status;
        waitpid(process_id, &status, 0);

        if(WIFEXITED(status))
          return WEXITSTATUS(status);

        exit(2); /*se o processo por que esperámos der erro, fechar este thread e dar erro também*/
      }
