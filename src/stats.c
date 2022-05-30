/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/
#include "stats.h"

/*Funcao que coloca no ficheiro out_file as estatísticas dos vários processos
(restaurantes, motoristas e clientes) e as estatísticas dos vários pedidos*/
void writeStatistics(FILE* out_file, struct main_data* data){

  fputs("\n |Process Statistics: \n\n", out_file);
  writeProcessStats(out_file, data);

  fputs("\n\n |Request Statistics:", out_file);
  writeRequestStats(out_file, data);

}

/*Funcao que escreve no ficheiro out_file as estatisticas dos processos com
formato :
 << Restaurant ... prepared ... requests!>>
<<Driver ... delivered ... requests!>>
<<Client ... received ... requests!>> */
void writeProcessStats(FILE* out_file, struct main_data* data){

  int nClients =  data -> n_clients ;
  int nRestaurants = data -> n_restaurants;
  int nDrivers = data -> n_drivers;

  char line [1500];

  for(int i = 0; i < nRestaurants; i++)
  {
    sprintf(line, "%s Restaurant %d preared %d requests! \n",
                                          SPACE,
                                          i,
                                          data -> restaurant_stats[i]);

    fputs(line, out_file);
  }

  fputs("\n\n", out_file);

  for(int i = 0; i < nDrivers; i++)
  {
    sprintf(line, "%s Driver %d delivered %d requests! \n",
                                          SPACE,
                                          i,
                                          data -> driver_stats[i]);
    fputs(line, out_file);
  }

  fputs("\n\n", out_file);

  for(int i = 0; i < nClients; i++)
  {
    sprintf(line, "%s Client %d received %d requests! \n",
                                          SPACE,
                                          i,
                                          data -> client_stats[i]);
    fputs(line, out_file);
  }
}

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
void writeRequestStats(FILE* out_file, struct main_data* data){

  char line [1500];
  char* created = malloc(200* sizeof(char));
  char* restTIme = malloc(200* sizeof(char));
  char* clientTIme = malloc(200* sizeof(char));
  char* driverTIme = malloc(200* sizeof(char));

  for(int i = 0; i < data -> max_ops; i++)
  {

    char stat = data -> results[i].status;

    strConvert(created, data -> results[i].start_time.tv_sec,
                            data -> results[i].start_time);

    strConvert(restTIme, data -> results[i].rest_time.tv_sec,
                            data -> results[i].rest_time);

    strConvert(clientTIme, data -> results[i].client_end_time.tv_sec,
                            data -> results[i].client_end_time);

    strConvert(driverTIme, data -> results[i].driver_time.tv_sec,
                            data -> results[i].driver_time);

    if(stat == 'I' || stat == 'R' || stat == 'D' || stat == 'C')
    {
      sprintf(line, "\n\n\n%s Request: %d\n%s Status: %c\n%s Restaurant id: %d \n"
                    "%s Driver id: %d\n%s Client id: %d\n%s Created: %s \n"
                    "%s Restaurant time: %s\n%s Driver time: %s\n%s"
                    " Client time (end): %s\n%s Total time: %.3f\n\n",
                    SPACE,
                    i,
                    SPACE,
                    stat,
                    SPACE,
                    data -> results[i].requested_rest,
                    SPACE,
                    data -> results[i].receiving_driver,
                    SPACE,
                    data -> results[i].requesting_client,
                    SPACE,
                    created,
                    SPACE,
                    restTIme,
                    SPACE,
                    driverTIme,
                    SPACE,
                    clientTIme,
                    SPACE,
                    (data -> results[i].client_end_time.tv_sec +
                    data -> results[i].client_end_time.tv_nsec / 1E9)
                    - (data -> results[i].start_time.tv_sec +
                    data -> results[i].start_time.tv_nsec / 1E9));
      fputs(line, out_file);
    }
  }

  free(created);
  free(restTIme);
  free(driverTIme);
  free(clientTIme);
}
