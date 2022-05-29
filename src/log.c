/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "log.h"

/* FUncao que vai colocar a informaçao necessaria para impressao numa variavel que vai ser
* usada pra imprimir para o ficheiro out_file. Nomeadamente <time instruction argument>
*/
void printLog(FILE* out_file, char* command, int id, int clientId, int restId, char* dish){

  char *line;
  line = malloc(sizeof(char)*300);

  //vai converter o tempo em string
  strConvert(line, time(NULL) , getTimeCLock());
  //adicionamos o comando usado
  strcat(line, command);
  strcat(line, " ");

  //verifica se o comando e status e se for adiciona o respetivo argumento
  if(strcmp(command, "status") == 0)
  {
    char i[100];
    sprintf(i, "%d ", id);
    strcat(line, i);
  }

  //verifica se o comando e request e se for adiciona os respetivos argumentos
  else if(strcmp(command, "request") == 0)
  {
    char cI[100];
    sprintf(cI, "%d ", clientId);
    char rI[100];
    sprintf(rI, "%d ", restId);

    strcat(line, cI);
    strcat(line, rI);
    strcat(line, dish);
  }

  strcat(line, "\n");
  fputs(line, out_file);
  free(line);
}
