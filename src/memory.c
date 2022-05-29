/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

#include "memory.h"
#include "memory-private.h"
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void* create_shared_memory(char* name, int size)
{
  int fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if(fd == -1)
    exit(3);

  if( ftruncate(fd, size) == -1 )
    exit(3);

  void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(ptr == MAP_FAILED)
    exit(3);

  return ptr;
}

/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um
* apontador para a mesma.
*/
void* create_dynamic_memory(int size)
{
  void* ptr = calloc(1, size);
  if(ptr == NULL)
    exit(4);

  return ptr;
}

/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size)
{
  int rmv = munmap(ptr, size);
  if (rmv == -1)
    exit(5);

  rmv = shm_unlink(name);
  if (rmv == -1)
    exit(5);
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_dynamic_memory(void* ptr)
{
  free(ptr);
}

/* Função que escreve uma operação no buffer de memória partilhada entre a Main
* e os restaurantes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op)
{
  write_rnd_access_buffer(buffer, buffer_size, op);

}

/* Função que escreve uma operação no buffer de memória partilhada entre os restaurantes
* e os motoristas. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op)
{
  int in = buffer->ptrs->in;
  int out = buffer->ptrs->out;

  int next = ((in + 1) % buffer_size);
  if( next != out ) /*posição livre para escrita*/
  {
    buffer->buffer[in] = *op;
    buffer->ptrs->in = next;
  }
}

/* Função que escreve uma operação no buffer de memória partilhada entre os motoristas
* e os clientes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op)
{
  write_rnd_access_buffer(buffer, buffer_size, op);
}

/* Função que escreve uma operação num buffer de acesso random.
* A operação é escrita numa posição livre do buffer, tendo em conta
* o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op)
{
  int i = 0;
  while( i < buffer_size )
  {
    int used = buffer->ptrs[i];
    if( !used )
    {
      buffer->buffer[i] = *op;
      buffer->ptrs[i] = 1;
      return;
    }
    i++;
  }
}

/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os restaurantes, se houver alguma disponível para ler que seja direcionada ao restaurante especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op)
{

  read_rnd_access_buffer(buffer, rest_id, buffer_size, op, 1);

}

/* Função que lê uma operação do buffer de memória partilhada entre os restaurantes e os motoristas,
* se houver alguma disponível para ler (qualquer motorista pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op)
{
  int in = buffer->ptrs->in;
  int out = buffer->ptrs->out;

  if( out != in ) /*operação disponivel para leitura*/
  {
    *op = buffer -> buffer[out];
    buffer -> ptrs -> out = (out + 1) % buffer_size;
  }
  else /*nenhuma operação disponivel para leitura*/
    op -> id = -1;

}

/* Função que lê uma operação do buffer de memória partilhada entre os motoristas e os clientes,
* se houver alguma disponível para ler dirijida ao cliente especificado. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op)
{
  read_rnd_access_buffer(buffer, client_id, buffer_size, op, 0);
}

/* Função que lê uma operação de um buffer de acesso random, se houver alguma disponível
* para ler dirijida ao "id" especificado.
* O id da operação que é verificado se é dirigido a "id", é o "requested_rest" ou o
* "requested_client", dependendo do valor recebido em "isRest".
* A leitura é feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse
* tipo. Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rnd_access_buffer(struct rnd_access_buffer* buffer, int id, int buffer_size, struct operation* op, int isRest)
{
  int i = 0;
  while(i < buffer_size )
  {
    int used = buffer->ptrs[i];
    if( used ) /*se houver alguma operação guardada no buffer no index i*/
    {
      /*vai há operação guardada e lê o id do restaurante requesitado ou do
      cliente que fez o pedido dependendo se "isRest" é 1 (true) ou 0 (false)*/
      int directed_id = isRest ? buffer->buffer[i].requested_rest :
      buffer->buffer[i].requesting_client;

      /*se o id direcionado for igual ao id, lê a operação da memoria*/
      if( id == directed_id )
      {
        *op = buffer->buffer[i];
        buffer->ptrs[i] = 0;
        return;
      }
    }
    i++;
  }

  op->id = -1; /*nenhuma operação dirigida a "id" encontrada*/
}
