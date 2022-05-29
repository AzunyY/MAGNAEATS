/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#define WARNING_DELIMITER "-----------------------------------------------------"
#define BIG_WARNING_DELIMITER "-------------------------------------------------------------------------------"
#define SPACE "   "

/* Imprime uma mensagem simples e curta de aviso.
* Cada linha é precedida pelo número de espaços indicados.
* Limite, warning deve ter 30 ou menos caracteres,
* mas é aconselhado que tenho até menos de 32 - 4 caracteres
* para não passar o comprimento dos delimitadores, contando
* com o tamanho de um `SPACE` adicional.
*/
void print_small_warning(int, char*);

/* Imprime o numero indicado de `SPACE`s e depois imprime a string indicada
*/
void print_after_spaces(int, char*);
