/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/
#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include "messages-private.h"
#include "main.h"
#include "metime.h"

/*Funcao que coloca no ficheiro out_file as estatísticas dos vários processos
(restaurantes, motoristas e clientes) e as estatísticas dos vários pedidos*/
void writeStatistics(FILE*, struct main_data*);

/*Funcao que escreve no ficheiro out_file as estatisticas dos processos com
formato :
 << Restaurant ... prepared ... requests!>>
<<Driver ... delivered ... requests!>>
<<Client ... received ... requests!>> */
void writeProcessStats(FILE*, struct main_data*);

/*Funcao que escreve no ficheiro out_file as estatisticas dos pedidos com
formato :
  << Request: … >>
  << Status: … >>
  << Restaurant id: … >>
  << Driver id: … >>
  << Client id: … >>
  << Created: … >>
  << Restaurant time: … >>
  << Driver time: … >>
  << Client time (end): … >>
  << Total Time: … >> */
void writeRequestStats(FILE*, struct main_data*);

#endif
