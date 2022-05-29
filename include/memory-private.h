/*
* Grupo que desenvolveu o projeto:
*  - Grupo SO-003
*
*  - 53563 Ana Luís
*  - 55306 João Teixeira
*  - 56312 Benjamim Gomes
*/

/* Função que escreve uma operação num buffer de acesso random.
* A operação é escrita numa posição livre do buffer, tendo em conta
* o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rnd_access_buffer(struct rnd_access_buffer*, int, struct operation*);

/* Função que lê uma operação de um buffer de acesso random, se houver alguma disponível
* para ler dirijida ao "id" especificado.
* O id da operação que é verificado se é dirigido a "id", é o "requested_rest" ou o
* "requested_client", dependendo do valor recebido em "isRest".
* A leitura é feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse
* tipo. Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rnd_access_buffer(struct rnd_access_buffer*, int, int, struct operation*, int);
