/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD


#include "main.h"
#include "messages-private.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#define MAXLINE 100

// Estrutura que contem as informaçoes adicionais do config.txt
struct configuration {

  FILE* log_out_file;
  FILE* statistics_out_file;
  int alarmtime;

};

/* Funcao que faz a inicializacao do main_data e que verifica e valida o ficheiro
* config.txt e dos parametros escritos nele. E tambem inicializada a estrutura de
* configuration para termos a informaçao adicional que nao esta no main_data.
*/
void configure(struct configuration*, struct main_data*, char*);

/* Funcao que devolve os digitos se a linha for digitos,
* caso contrário sai do programa com mensagem de erro. O linecount vai ser
* atualizado conforme formos lendo as linhas do ficheiro.
*/
int scanDigit(int* , char*);

/* Funcao que verifica a validade dos nomes dos ficheiros e abre-os,
* caso contrário sai do programa com mensagem de erro. O linecount vai ser
* atualizado conforme formos lendo as linhas do ficheiro.
*/
void is_txt(FILE**, char*, int*);

/*Funcao que imprime uma mensagem de erro apropriada para inputs invalidos
* e dá exit do programa.
*/
void error(int*, char*);


/* Funcao que vai fechar os ficheiros de log e statistics quando terminamos o programa*/
void closeFile(struct configuration*);

#endif
