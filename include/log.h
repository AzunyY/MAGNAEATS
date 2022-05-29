/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include "metime.h"

/* Funcao que vai colocar a informaçao necessaria para impressao numa variavel que vai ser
* usada pra imprimir para o ficheiro out_file. Nomeadamente <time instruction argument>
*/
void printLog (FILE*, char*, int, int, int, char*);

#endif
