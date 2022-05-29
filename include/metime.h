/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#ifndef METIME_H_GUARD
#define METIME_H_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/*Funcao que devolve o tempo em estrutura timespec*/
struct timespec getTimeCLock();

/* Funcao que converte o timespec e tm para string para colocarmos a data e
* o tempo na variavel line
*/
void strConvert(char* , time_t, struct timespec);

#endif
