/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "main-private.h"

/*variavel para verificar se o sistema recebeu um ctrlC e poder fechar
o while em user_interaction*/
static int stop = 0;
/*variavel para guardar a estrutura recebida de configure() no main_args
para conseguirmos utilizar os ficheiros e o alarmtime nas outras funcoes que deles
vao precisar */
static struct configuration* configInfo;

int main(int argc, char *argv[])
{
  //init data structures
  data = create_dynamic_memory(sizeof(struct main_data));
  buffers = create_dynamic_memory(sizeof(struct communication_buffers));
  buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
  buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
  buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

  //estrutura do configuration que contem os ficheiros onde vao ser escritas coisas e o alarmtime
  configInfo = create_dynamic_memory(sizeof(struct configuration));

  // init semaphore data structure
  sems = create_dynamic_memory(sizeof(struct semaphores));
  sems->main_rest = create_dynamic_memory(sizeof(struct prodcons));
  sems->rest_driv = create_dynamic_memory(sizeof(struct prodcons));
  sems->driv_cli = create_dynamic_memory(sizeof(struct prodcons));

  //execute main code
  main_args(argc, argv, data);
  create_dynamic_memory_buffers(data);
  create_shared_memory_buffers(data, buffers);
  create_semaphores(data, sems);
  launch_processes(buffers, data, sems);
  user_interaction(buffers, data, sems);

  //release memory before terminating
  destroy_dynamic_memory(data);
  destroy_dynamic_memory(buffers->main_rest);
  destroy_dynamic_memory(buffers->rest_driv);
  destroy_dynamic_memory(buffers->driv_cli);
  destroy_dynamic_memory(buffers);
  destroy_dynamic_memory(sems->main_rest);
  destroy_dynamic_memory(sems->rest_driv);
  destroy_dynamic_memory(sems->driv_cli);
  destroy_dynamic_memory(sems);
}


/* Função que lê os argumentos da aplicação, nomeadamente o número
* máximo de operações, o tamanho dos buffers de memória partilhada
* usados para comunicação, e o número de clientes, de motoristas e de
* restaurantes. Guarda esta informação nos campos apropriados da
* estrutura main_data.
*/
void main_args(int argc, char* argv[], struct main_data* data)
{
  if( argc != 2 ) /*se não forem recebido exatamente um parametro adicional*/
  {
    if( argc > 1 ) /*se for recebido algum parametro*/
      print_warning("Invalid Parameters");

    printf("\n |%s\n\n%s%s <%s> \n\n%s%s: %s\n\n",
    "How to use",
    SPACE, "magnaeats", "configuration file",
    SPACE, "Example", "./bin/magnaeats config.txt");
    exit(1);
  }

  //guarda a informaçao nos campos apropriados da main_data e recebe a informaçao extra
  configure(configInfo, data, argv[1]);
}

/* Função que reserva a memória dinâmica necessária para a execução
* do MAGNAEATS, nomeadamente para os arrays *_pids e *_stats da estrutura
* main_data. Para tal, pode ser usada a função create_dynamic_memory.
*/
void create_dynamic_memory_buffers(struct main_data* data)
{
  int size = sizeof(int);

  data->restaurant_pids = create_dynamic_memory( size * data->n_restaurants );
  data->driver_pids = create_dynamic_memory( size * data->n_drivers );
  data->client_pids = create_dynamic_memory( size * data->n_clients );

  data->restaurant_stats = create_dynamic_memory(size * data->n_restaurants );
  data->driver_stats = create_dynamic_memory(size * data->n_drivers);
  data->client_stats = create_dynamic_memory(size * data->n_clients);
}

/* Função que reserva a memória partilhada necessária para a execução do
* MAGNAEATS. É necessário reservar memória partilhada para todos os buffers da
* estrutura communication_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory.
*/
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers)
{
  int int_size = sizeof(int);

  /*reserva memoria data results e terminate*/
  data -> results =
  create_shared_memory("/results", sizeof(struct operation) * data->max_ops);
  data -> terminate = create_shared_memory("/terminate", int_size);

  /*reserva memoria buffers*/
  int buffer_size = data->buffers_size;

  /*main_rest buffer*/
  buffers -> main_rest->ptrs =
  create_shared_memory("/main_rest_ptrs", int_size * buffer_size);
  buffers -> main_rest->buffer =
  create_shared_memory("/main_rest_buffer", sizeof(struct operation) * buffer_size);

  /*rest_driv buffer*/
  buffers -> rest_driv->ptrs =
  create_shared_memory("/rest_driv_ptrs", sizeof(struct pointers));
  buffers -> rest_driv->buffer =
  create_shared_memory("/rest_driv_buffer", sizeof(struct operation) * buffer_size);

  /*driv_cli buffer*/
  buffers -> driv_cli->ptrs = create_shared_memory("/driv_cli_ptrs", int_size * buffer_size);
  buffers -> driv_cli->buffer =
  create_shared_memory("/driv_cli_buffer", sizeof(struct operation) * buffer_size);
}

/* Função que inicia os processos dos restaurantes, motoristas e
* clientes. Para tal, pode usar as funções launch_*,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
  int i;

  /*iniciar restaurantes*/
  for(i = 0; i < data->n_restaurants; i++)
    data->restaurant_pids[i] = launch_restaurant(i, buffers, data, sems);

  /*iniciar motoristas*/
  for(i = 0; i < data->n_drivers; i++)
    data->driver_pids[i] = launch_driver(i, buffers, data, sems);

  /*iniciar clientes*/
  for(i = 0; i < data->n_clients; i++)
    data->client_pids[i] = launch_client(i, buffers, data, sems);

}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* request - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do MAGNAEATS através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
  char command[8];
  int op_counter = 0;

  print_help_message(); /*imprime os comandos validos uma primeira vez antes de ler
                          os comandos usados pelo utilizador*/

  //inicializa o alarme - na classe mesignal.c
  setAlarm(configInfo -> alarmtime);
  //apanha o sinal do ctrlC - na classe mesignal.c
  catchCtrlC();

  /*lê os comandos usados pelo utilizador e executa-os*/
  while(1)
  {

    printf("\n> ");
    scanf("%7s", command);

    if(stop == 1)
    break;

    if( strcmp(command, "request") == 0 ) /*executa o comando request*/
    {
      create_request(&op_counter, buffers, data, sems);
    }
    else if ( strcmp(command, "status") == 0 ) /*executa o comando status*/
    {
      read_status(data, sems);
    }
    else if ( strcmp(command, "stop") == 0 ) /*executa o comando stop*/
    {
      printLog(configInfo -> log_out_file, "stop", -1, -1, -1, NULL);/*coloca no
      ficheiro log_out_file o comando inserido*/
      stop_execution(data, buffers, sems);
      break;
    }
    else if ( strcmp(command, "help") == 0) /*executa o comando help*/
    {

      printLog(configInfo -> log_out_file, "INVALID COMAND!", -1, -1, -1, NULL); /*coloca no
      ficheiro log_out_file o comando inserido*/
      print_help_message();
    }
    else /*comando invalido, imprime mensagem de aviso*/
    {
      printLog(configInfo -> log_out_file, "stop", -1, -1, -1, NULL);/*coloca no
      ficheiro log_out_file o comando inserido*/
      print_command_warning("Invalid Command", "Please try again"); /*imprime aviso*/
    }
  }
}

/*Imprime uma mensagem de ajuda com os comandos validos do programa
*/
void print_help_message()
{
  printf("\n |%s\n\n%s%-22s - %s\n%s%-22s - %s\n%s%-22s - %s\n%s%-22s - %s\n",
  "Commands",
  SPACE, "Create new request", "request <client> <restaurant> <dish>",
  SPACE, "See the request status", "status <id>",
  SPACE, "Stop program execution","stop",
  SPACE, "Show this list again", "help");
}

/* Se o limite de operações ainda não tiver sido atingido, cria uma nova
* operação identificada pelo valor atual de op_counter e com os dados introduzidos
* pelo utilizador na linha de comandos, escrevendo a mesma no buffer de memória
* partilhada entre main e restaurantes. Imprime o id da operação e incrementa o
* contador de operações op_counter.
*/
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{

  if (*op_counter < data->max_ops)
  {
    /*tenta ler os 3 parametros e actualiza um boolean para o caso de serem inválidos*/
    int client_id, rest_id, matched = 1;
    matched &= scanf("%d", &client_id);
    matched &= scanf("%d", &rest_id);

    char dish[100];
    matched &= scanf("%s", dish);

    //coloca no ficheiro log_out_file o comando inserido
    printLog(configInfo -> log_out_file, "request", -1, client_id , rest_id , dish);

    if(matched)
    {
      /*cria nova operação*/
      struct operation op = { .id = *op_counter,
        .requested_rest = rest_id,
        .requesting_client = client_id,
        .requested_dish = strdup(dish),
        .status = 'I',
        .receiving_rest = -1,
        .receiving_driver = -1,
        .receiving_client = -1,
        .start_time = getTimeCLock(),
        .rest_time = {0},
        .driver_time = {0},
        .client_end_time = {0}
      };


      semaphore_mutex_lock(sems->results_mutex); /*guarda operação no histórico*/
      data->results[(*op_counter)] = op;
      semaphore_mutex_unlock(sems->results_mutex);
      (*op_counter)++;

      /*Imprime id da operação e incrementa contador*/
      printf("\n |Request created with id #%03d!\n", *op_counter);

      produce_begin(sems->main_rest);
      write_main_rest_buffer(buffers->main_rest, data->buffers_size, &op);
      produce_end(sems->main_rest);

    } else /*os arguemntos dados são invalidos*/
      print_command_warning("Invalid arguments", "Request creation failed"); /*imprime aviso*/
  }
  else /*foi atingido o maximo de pedidos possiveis de criar*/
    print_command_warning("Max requests created reached", "Request creation failed"); /*imprime aviso*/
}

/* Função que lê um id de operação do utilizador e verifica se a mesma
* é valida. Em caso afirmativo,
* imprime informação da mesma, nomeadamente o seu estado, o id do cliente
* que fez o pedido, o id do restaurante requisitado, o nome do prato pedido
* e os ids do restaurante, motorista, e cliente que a receberam e processaram.
*/
void read_status(struct main_data* data, struct semaphores* sems)
{
  int id, matched = scanf("%d", &id);

  //coloca no ficheiro log_out_file o comando inserido
  printLog(configInfo -> log_out_file, "status", id, -1, -1, NULL);

  if(matched)
  {
    semaphore_mutex_lock(sems->results_mutex);
    struct operation op = data->results[id];
    semaphore_mutex_unlock(sems->results_mutex);

    if(op.status != 0)
    {
      /*imprime o id da operação e os status*/
      printf("\n |%s #%03d\n\n%s%s: '%c'\n",
      "Request", op.id,
      SPACE, "Status", op.status);

      /*imprime o id do client, restaurant requested e dish*/
      printf("\n%s%s: C#%03d\n%s%s: R#%03d\n%s%s: %s\n",
      SPACE, "Client Id", op.requesting_client,
      SPACE, "Restaurant requested", op.requested_rest,
      SPACE, "Requested dish", op.requested_dish);

      /*imprime o restaurant, driver e client que receberam e processaram o pedido*/
      if(op.receiving_rest != -1)/*se o restaurante foi encontrado*/
      {
        printf("\n%s%s: R#%03d\n", SPACE, "Treated by restaurant", op.receiving_rest);

        if(op.receiving_driver != -1) /*se um driver recebeu o pedido*/
        {

          printf("%s%s: D#%03d\n",
          SPACE, "Transported by driver", op.receiving_driver);

          if(op.receiving_client != -1)/*se o cliente recebeu o pedido*/
          {

            printf("%s%s: C#%03d\n",
            SPACE, "Received by client", op.receiving_client);

          }
          else /*cliente não encontrado*/
            printf("%s%s!\n", SPACE, "Client not found");

        }
        else /*nenhum driver recebeu o pedido*/
        {

          printf("%s%s...\n%s%s!\n",
          SPACE, "Waiting for driver",
          SPACE, "Food not received by client");

        }
      }
      else /*restaurante pedido não encontrado*/
      {

        printf("\n%s%s!\n%s%s!\n%s%s!\n",
        SPACE, "Requested restaurant not found",
        SPACE, "No food ready to transport",
        SPACE, "Food not received by client");

      }
    }
    else /*id não existe*/
      print_command_warning("Invalid Id", "Failed getting Id status"); /*imprime aviso*/

  }
  else /*não foi recebido um inteiro como argumento*/
    print_command_warning("Invalid Id", "Can't use a non integer value as Id"); /*imprime aviso*/
}

/* Função que termina a execução do programa MAGNAEATS. Deve começar por
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems)
{
  print_warning("Terminating MAGNAEATS"); /*imprime que o programa vai ser interrompido*/
  *(data->terminate) = 1; /*poe terminate a 1 (true) */
  wakeup_processes(data, sems);/*acorda processos adormecidos em semaphores*/
  wait_processes(data); /*espera processos filhos terminar*/
  write_statistics(data); /*escreve estatisticas finais*/
  closeFile(configInfo); /*fecha os ficheiros de escrita*/
  destroy_semaphores(sems); /*liberta todos os semáforos da estrutura semaphores.*/
  destroy_memory_buffers(data, buffers); /*liberta zonas de memoria previamente reservadas*/
  destroy_dynamic_memory(configInfo);/*liberta a memoria da estrutura configInfo*/
  stop = 1; /*atualiza a variavel de stop para 1 para caso seja o ctrlC podermos sair do while do user_interaction*/
}

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo restaurantes, motoristas e clientes. Para tal, pode usar a função
* wait_process do process.h.
*/
void wait_processes(struct main_data* data)
{
  int i;

  /*esperar fim restaurantes*/
  for(i = 0; i < data->n_restaurants; i++)
    data -> restaurant_stats[i] = wait_process(data -> restaurant_pids[i]);

  /*esperar fim drivers*/
  for(i = 0; i < data->n_drivers; i++)
    data -> driver_stats[i] = wait_process(data -> driver_pids[i]);

  /*esperar fim clientes*/
  for(i = 0; i < data->n_clients; i++)
    data -> client_stats[i] = wait_process(data -> client_pids[i]);
}

/* Função que imprime as estatisticas finais do MAGNAEATS, nomeadamente quantas
* operações foram processadas por cada restaurante, motorista e cliente.
*/
void write_statistics(struct main_data* data)
{
  writeStatistics(configInfo -> statistics_out_file, data);
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers)
{
  destroy_data_memory(data); /*destroi memoria de data*/
  destroy_buffers_memory(buffers, data->buffers_size); /*destroi memoria de buffers*/
}

/* Função que liberta toda a memoria reservada por "data".
*/
void destroy_data_memory(struct main_data* data)
{
  /*dá free dos mallocs feitos por strdup usado nos pratos*/
  int i;
  for(i = 0; i < data->max_ops; i++)
  {
    struct operation op = data->results[i];

    if(op.status == 0)
      break;

    destroy_dynamic_memory(op.requested_dish);
  }

  /*destroi as memorias partilhadas de results e terminate*/
  destroy_shared_memory("/results", data->results, sizeof(struct operation) * data->max_ops);
  destroy_shared_memory("/terminate", data->terminate, sizeof(int));

  /*destroi a memoria dinamica dos dados restantes quardados em data*/
  destroy_dynamic_memory(data->restaurant_pids);
  destroy_dynamic_memory(data->driver_pids);
  destroy_dynamic_memory(data->client_pids);

  destroy_dynamic_memory(data->restaurant_stats);
  destroy_dynamic_memory(data->driver_stats);
  destroy_dynamic_memory(data->client_stats);

}

/* Função que liberta toda a memoria reservada por "buffers".
*/
void destroy_buffers_memory(struct communication_buffers* buffers, int buffer_size)
{
  int int_size = sizeof(int);

  /*main_rest buffer*/
  destroy_shared_memory("/main_rest_ptrs", buffers->main_rest->ptrs, int_size * buffer_size);
  destroy_shared_memory("/main_rest_buffer", buffers->main_rest->buffer,
  sizeof(struct operation) * buffer_size);

  /*rest_driv buffer*/
  destroy_shared_memory("/rest_driv_ptrs", buffers->rest_driv->ptrs, sizeof(struct pointers));
  destroy_shared_memory("/rest_driv_buffer", buffers->rest_driv->buffer,
  sizeof(struct operation) * buffer_size);

  /*driv_cli buffer*/
  destroy_shared_memory("/driv_cli_ptrs", buffers->driv_cli->ptrs, int_size * buffer_size);
  destroy_shared_memory("/driv_cli_buffer", buffers->driv_cli->buffer,
  sizeof(struct operation) * buffer_size);

}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
* *_full devem ser inicializados com valor 0, semáforos *_empty com valor
* igual ao tamanho dos buffers de memória partilhada, e os *_mutex com valor
* igual a 1. Para tal pode ser usada a função semaphore_create.
*/
void create_semaphores(struct main_data* data, struct semaphores* sems)
{
  int size = data -> buffers_size;

  sems->main_rest->full = semaphore_create(STR_SEM_MAIN_REST_FULL, 0);
  sems->main_rest->empty = semaphore_create(STR_SEM_MAIN_REST_EMPTY, size);
  sems->main_rest->mutex = semaphore_create(STR_SEM_MAIN_REST_MUTEX, 1);

  sems->rest_driv->full = semaphore_create(STR_SEM_REST_DRIV_FULL, 0);
  sems->rest_driv->empty = semaphore_create(STR_SEM_REST_DRIV_EMPTY, size);
  sems->rest_driv->mutex = semaphore_create(STR_SEM_REST_DRIV_MUTEX, 1);

  sems->driv_cli->full = semaphore_create(STR_SEM_DRIV_CLI_FULL, 0);
  sems->driv_cli->empty = semaphore_create(STR_SEM_DRIV_CLI_EMPTY, size);
  sems->driv_cli->mutex = semaphore_create(STR_SEM_DRIV_CLI_MUTEX, 1);

  sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
* estes percebam que foi dada ordem de terminação do programa. Para tal,
* pode ser usada a função produce_end sobre todos os conjuntos de semáforos
* onde possam estar processos adormecidos e um número de vezes igual ao
* máximo de processos que possam lá estar.
*/
void wakeup_processes(struct main_data* data, struct semaphores* sems)
{

  for(int i = 0; i < data -> n_restaurants; i++)
    produce_end(sems -> main_rest);

  for(int i = 0; i < data -> n_drivers; i++)
    produce_end(sems -> rest_driv);

  for(int i = 0; i < data -> n_clients; i++)
    produce_end(sems -> driv_cli);

  for(int i = 0; i < data->max_ops; i++)
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que liberta todos os semáforos da estrutura semaphores.
*/
void destroy_semaphores(struct semaphores* sems)
{

  semaphore_destroy(STR_SEM_MAIN_REST_FULL, sems->main_rest->full);
  semaphore_destroy(STR_SEM_MAIN_REST_EMPTY, sems->main_rest->empty);
  semaphore_destroy(STR_SEM_MAIN_REST_MUTEX, sems->main_rest->mutex);

  semaphore_destroy(STR_SEM_REST_DRIV_FULL, sems->rest_driv->full);
  semaphore_destroy(STR_SEM_REST_DRIV_EMPTY, sems->rest_driv->empty);
  semaphore_destroy(STR_SEM_REST_DRIV_MUTEX, sems->rest_driv->mutex);

  semaphore_destroy(STR_SEM_DRIV_CLI_FULL, sems->driv_cli->full);
  semaphore_destroy(STR_SEM_DRIV_CLI_EMPTY, sems->driv_cli->empty);
  semaphore_destroy(STR_SEM_DRIV_CLI_MUTEX, sems->driv_cli->mutex);

  semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);

}

/* Imprime uma mensagem de aviso simples padrão.
*/
void print_warning(char* warning)
{
  printf("\n %s\n%s%s!\n %s\n",
  WARNING_DELIMITER,
  SPACE, warning,
  WARNING_DELIMITER);
}

/* Imprime uma mensagem de aviso padrão usando os parametros "warning" e "note"
* recebidos.
* Depois lê o que restar da linha do stdin para que os tokens que não tiverem
* sido lidos não criem um segundo aviso ao serem lidos pelo scanf que lê
* novos comandos.
*/
void print_command_warning(char* warning, char* note)
{
  printf("\n %s\n%s%s!\n%s%s...\n%s%s\n %s\n",
  WARNING_DELIMITER,
  SPACE, warning,
  SPACE, note,
  SPACE, TYPE_HELP,
  WARNING_DELIMITER);

  cleanLine(); /*lê o resto da linha*/
}

/* Lê o que resta de uma linha do stdin e descarta-a
*/
void cleanLine()
{
  char c;
  while ( (c = getchar() ) != '\n' && c != EOF ); /*enquanto não chegar ao fim da linha*/
}
