Grupo que desenvolveu o projeto:
  - Grupo SO-003

  - 53563 Ana Luís
  - 55306 João Teixeira
  - 56312 Benjamim Gomes

Escolhas de Implementação:
  - Depois de haver um erro de input inválido, a linha atual é lida até ao fim e
    descartada para evitar que um input inválido com varios tokens lance mais do
    que um aviso : ( cleanLine() )

  - Avisos que possam aparecer a qualquer altura e interromper o pedido de input
    ao utilizador, como por exemplo um aviso a dizer que um cliente recebeu um
    pedido, imprimem o caracter '>' (que indica a zona para escrever input),
    para o utilizador saber que pode executar comandos depois de receber tais
    avisos.

  - Foi criado uma estrutura no configuration.h para que conseguirmos ler toda
  a informaçao do ficheiro "config.txt" e guardar a informação adicional que não
  faz parte da estrutura main_data.

  - Foi decidido que os ficheiros de escrita do log e das estatisticas na classe
  configuration.c pois é esta a classe que trata da leitura dos dados do ficheiro
  com a informação necessária para correr o programa.

  - De forma a conseguir passar a informaçao que está na estrutura Configuration
  foi declarada uma variável estática na main para guardar os dados adicionais
  que a chamada da função configure le, e assim consegurimos usar os seus resultados
  noutras funções que vão necessitar dos mesmos, mas que não podem ser tratadas
  na funcao main_args.

  - Foi criado uma variável estática stop que serve para conseguirmos sair do loop
  infinito quando se faz ctrlC, de outra forma o programa fechava mas não sai do
  loop e teriamos assim um erro.

  - Na leitura do ficheiro config.txt foi considerado que se forem introduzidos
  menos parâmetros que aos necessários então deveria ser dado um aviso e fechar
  o programa.

  - No caso de o ficheiro config.txt ter mais parâmetros que os 7 necessários então
  também consideramos tal como sendo um ficheiro inválido.

Erros conhecidos:
  - Como não há sincronização de processos vários condutores podem ser capazes de
    ler um pedido de um restaurante ao mesmo tempo e o pedido ser recebido pelo
    cliente varias vezes. -- FOI RESOLVIDO NA PARTE 2 DO PROJETO

  - Como não há sincronização de processos varios condutores podem ser capazes de
    ler o mesmo pedido e actualizam o pointer out do buffer circular, até ficar
    maior que in, o que faz com que condutores futuros leiam pedidos que não
    existem ou releiam pedidos já tratados que ainda estejam no buffer. -- FOI
    RESOLVIDO NA PARTE 2 DO PROJETO
