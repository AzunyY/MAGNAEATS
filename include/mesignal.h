/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#ifndef MESIGNAL_H_GUARD
#define MESGINAL_H_GUARD

#include "main-private.h"
#include <signal.h>
#include <sys/time.h>


/*Funcao que vai inicializar o alarme*/
void setAlarm(int);

/*Funcao que vai tratar do sinal do alarme e escreva
para o ecrã o estado atual de todos os pedidos, incluindo:
(i) os que já foram finalizados (recebidos pelo cliente) e
(ii) os que ainda estão em andamento com o seguinte formato:
<<request status start_time receiving_rest rest_time receiving_driver
driver_time receiving_client client_end_time>> */
void sinalAlarm();

/*Funcao que vai tratar do sinal ctrlC de forma a libertar todos os recursos do
MagnaEats e efetuar um fecho normal do programa recorrendo a chamada da funcao
stop_execution*/
void ctrlC();

/*Funcao que vai receber o sinal de interrupcao do programa*/
void catchCtrlC();

#endif
