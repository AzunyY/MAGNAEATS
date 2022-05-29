/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "mesignal.h"

/*Funcao que vai inicializar o alarme*/
void setAlarm(int alarmtime){

  struct itimerval val;
  signal(SIGALRM, sinalAlarm); /*indica ao sistema operativo que deve enviar o
  sinal SIGALRM ao processo após sinalAlarm segundos. */
  val.it_interval.tv_sec = alarmtime;
  val.it_interval.tv_usec = 0;
  val.it_value.tv_sec = alarmtime;
  val.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &val, 0); /*quando o temporizador expira é enviado ao
  processo um determinado sinal SIGALRM.*/

}

/*Funcao que vai tratar do sinal do alarme e escreva
para o ecrã o estado atual de todos os pedidos, incluindo:
(i) os que já foram finalizados (recebidos pelo cliente) e
(ii) os que ainda estão em andamento com o seguinte formato:
<<request status start_time receiving_rest rest_time receiving_driver
driver_time receiving_client client_end_time>> */
void sinalAlarm() {

  int i;
  for(i = 0; i < data -> max_ops; i++) /*imprime o estado dos pedidos*/
  {

    char stat = data -> results[i].status;

    if(stat == 'C')
    {

      printf("\n\n %s \n %s request: %d status: %c start: %ld \n %s"
             " client: %d client_time: %ld"
             "\n %s proxy: %d proxy_time: %ld \n %s "
             "server: %d server_time: %ld \n %s \n\n%c ",
              WARNING_DELIMITER,
              SPACE,
              i,
              stat,
              data -> results[i].start_time.tv_sec,
              SPACE,
              data -> results[i].receiving_rest,
              data -> results[i].rest_time.tv_sec,
              SPACE,
              data -> results[i].receiving_driver,
              data -> results[i].driver_time.tv_sec,
              SPACE,
              data -> results[i].receiving_client,
              data -> results[i].client_end_time.tv_sec,
              WARNING_DELIMITER,
              '>');
      fflush(stdout);

    }
    else if(stat == 'D' || stat == 'I' || stat == 'R')
    {

      printf("\n\n %s \n %s request: %d status: %c \n %s \n\n%c ",
              WARNING_DELIMITER,
              SPACE,
              i,
              stat,
              WARNING_DELIMITER,
              '>');
      fflush(stdout);

    }
    else
      break;
  }
}

/*Funcao que vai tratar do sinal ctrlC de forma a libertar todos os recursos do
MagnaEats e efetuar um fecho normal do programa recorrendo a chamada da funcao
stop_execution*/
void ctrlC(){

  signal(SIGINT, SIG_IGN);
  stop_execution(data, buffers, sems);

}

/*Funcao que vai receber o sinal de interrupcao do programa*/
void catchCtrlC(){

  struct sigaction sa;
  sa.sa_handler = ctrlC;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask); //apaga a máscara (nenhum sinal é bloqueado)

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("main:");
    exit(1);
  }

}
