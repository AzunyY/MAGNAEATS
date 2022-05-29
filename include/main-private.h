/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#ifndef MAINPRIVATE_H_GUARD
#define MAINPRIVATE_H_GUARD

#include "main.h"
#include "configuration.h"
#include "process.h"
#include "log.h"
#include "mesignal.h"
#include "stats.h"
#define TYPE_HELP "(Type 'help' to see a list of valid commands)"

struct communication_buffers* buffers;
struct semaphores* sems;
struct main_data* data;

/*Imprime uma mensagem de ajuda com os comandos validos do programa
*/
void print_help_message();

/* Função auxiliar de "write_statistics" que imprime as estatisticas
* do MAGNAEATS
*/
void print_statistics(struct main_data*);

/* Função que liberta toda a memoria reservada por "buffers".
*/
void destroy_buffers_memory(struct communication_buffers*, int);

/* Função que liberta toda a memoria reservada por "data".
*/
void destroy_data_memory(struct main_data*);

/* Imprime uma mensagem de aviso simples padrão.
*/
void print_warning(char*);

/* Imprime uma mensagem de aviso padrão usando os parametros "warning" e "note"
* recebidos.
* Depois lê o que restar da linha do stdin para que os tokens que não tiverem
* sido lidos não criem um segundo aviso ao serem lidos pelo scanf que lê
* novos comandos.
*/
void print_command_warning(char*, char*);

/* Lê o que resta de uma linha do stdin e descarda-a
*/
void cleanLine();

#endif
