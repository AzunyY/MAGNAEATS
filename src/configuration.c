/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "configuration.h"

/* Funcao que faz a inicializacao do main_data e que verifica e valida o ficheiro
* config.txt e dos parametros escritos nele. E tambem inicializada a estrutura de
* configuration para termos a informaçao adicional que nao esta no main_data.
*/
void configure(struct configuration* info, struct main_data* data, char* filename)
{
  char line[MAXLINE]; //le a linha
  int count = 0;  // conta numero de linhas lidos

  FILE* file = NULL;
  is_txt(&file, filename, &count); //verifica se o ficheiro é .txt  e abre o ficheiro

  data -> max_ops = scanDigit(&count, fgets(line, MAXLINE, file));
  data -> buffers_size = scanDigit(&count, fgets(line, MAXLINE, file));
  data -> n_restaurants = scanDigit(&count, fgets(line, MAXLINE, file));
  data -> n_drivers = scanDigit(&count, fgets(line, MAXLINE, file));
  data -> n_clients = scanDigit(&count, fgets(line, MAXLINE, file));
  is_txt(&info -> log_out_file, fgets(line, MAXLINE, file), &count);//verifica se o ficheiro é .txt e abre o ficheiro
  is_txt(&info -> statistics_out_file, fgets(line, MAXLINE, file), &count);//verifica se o ficheiro é .txt e abre o ficheiro
  info -> alarmtime = scanDigit(&count, fgets(line, MAXLINE, file));

  if(fgets(line, MAXLINE, file) != NULL)
    error(&count, "File is invalid, there is too many parameters!");

  fclose(file);
}

/* Funcao que devolve os digitos se a linha for digitos,
* caso contrário sai do programa com mensagem de erro. O linecount vai ser
* atualizado conforme formos lendo as linhas do ficheiro.
*/
int scanDigit(int* linecount, char* line){

  if(line == NULL)
    error(linecount, "File is invalid, there is too little parameters!");

  //retiramos do fim da string o \n porque quando lemos do config.txt vem com \n
  strtok(line, "\n");
  int i = 0;

  while(line[i] != '\0'){
    if(!isdigit(line[i]))
      error(linecount, "Invalid parameters! Insert a positive number!");
    i++;
  }

  (*linecount)++;
  return atoi(line);
}

/* Funcao que verifica a validade dos nomes dos ficheiros e abre-os,
* caso contrário sai do programa com mensagem de erro. O linecount vai ser
* atualizado conforme formos lendo as linhas do ficheiro.
*/
void is_txt(FILE** file, char* filename, int* linecount)
{
  if(filename == NULL){
    error(linecount, "File is invalid, there is too little parameters!");
  }

  //separa a string por ex: ola.txt ficamos com .txt
  char *last = strrchr(filename, '.');

  //retiramos do fim da string o \n uma vez que no ficheiro de config
  //o nome dos ficheiros acaba com \n
  strtok(last, "\n");

  if (last != NULL){
    if(strcmp(last, ".txt") != 0)
      error(linecount, "Invalid parameters! The format is - namefile.txt");
  } else
      error(linecount, "Invalid parameters! The format is - namefile.txt");

    /*se for linecount = 0 estamos no ficheiro de config portanto o ficheiro
    e aberto como ficheiro de leitura caso contrario sao abertos como de escrita*/
    if(*linecount == 0)
      *file = fopen(filename,"r");
    else
      *file = fopen(filename, "w");

    if(file == NULL) /*verifica se ficheiro foi aberto com sucesso*/
      error(linecount, "Can't open file");

  (*linecount)++;
}

/*Funcao que imprime uma mensagem de erro apropriada para inputs invalidos
* e dá exit do programa.
*/
void error(int* line, char* msg_expected)
{
  printf("\n%s\n",BIG_WARNING_DELIMITER); //delimitador de inicio da mensagem

  printf("\n%s|%s ", SPACE, "Configuration file; error");

  if(*line != 0)
    printf("%s %d\n",  "on line", *line);
  else
    printf("\n");

  printf("\n%s%s%s!\n", SPACE, SPACE, msg_expected);

  printf("\n%s\n\n", BIG_WARNING_DELIMITER); //delimitador de fim da mensagem
  exit(1);
}

/* Funcao que vai fechar os ficheiros de log e statistics quando terminamos o programa*/
void closeFile(struct configuration* files){
  fclose(files-> log_out_file);
  fclose(files ->  statistics_out_file);
}
