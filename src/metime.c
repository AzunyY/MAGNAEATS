/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/
#include "metime.h"

/*Funcao que devolve o tempo em estrutura timespec*/
struct timespec getTimeCLock(){

  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);

  return time;
}


/*Funcao auxiliar que converte o tempo em string*/
void strConvert(char* lineH, time_t time, struct timespec mili){

    struct tm *info = localtime(&time);

    char buffer[100];
    char miliS[100];

    /*converte date para string*/
    strftime(buffer,sizeof(buffer),"%Y-%m-%d %I:%M:%S." , info);

    /*Converte segundos para milisegundos e converte os milisegundos para string*/
    sprintf(miliS, "%03.f ", (mili.tv_nsec / 1e06));
    strcat(buffer, miliS);

    strcpy(lineH, buffer);

}
