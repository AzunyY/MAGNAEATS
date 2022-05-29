# Grupo que desenvolveu o projeto
#  - Grupo SO-003
#
#  - 53563 Ana Luís
#  - 55306 João Teixeira
#  - 56312 Benjamim Gomes

# Variável com os objetos para a criação do binário
OBJECTS = main.o \
	configuration.o synchronization.o \
	memory.o process.o \
	metime.o \
	log.o stats.o \
	restaurant.o driver.o client.o mesignal.o 

# Variáveis de diretorios
OBJ_DIR = obj
BIN_DIR = bin

# Variáveis de compilação
CC = gcc
CFLAGS = -Wall -iquote include -g
CLIBS = -lrt -lpthread -lm

# Variaveis de teste
RUN = ./$(BIN_DIR)/magnaeats
TEST = $(RUN) config.txt

# Variáveis com os ficheiros .h extra, necessários para a criação de cada objeto .o
main.o = main-private.h configuration.h \
			messages-private.h process.h \
			synchronization.h main.h \
			log.h mesignal.h stats.h
client.o = messages-private.h metime.h
configuration.o = configuration.h
driver.o = messages-private.h metime.h
log.o = log.h metime.h
stats.o = stats.h metime.h
memory.o = memory-private.h
mesignal.o = mesignal.h
metime.o = metime.h
process.o = restaurant.h driver.h client.h
restaurant.o = messages-private.h metime.h
synchronization.o = synchronization.h

vpath %.o $(OBJ_DIR)
vpath %.c src

# Compilação do binário do programa
magnaeats: $(OBJECTS)
	$(CC) $(addprefix $(OBJ_DIR)/, $(OBJECTS)) -o $(BIN_DIR)/$@ $(CLIBS)

# Criação dos ficheiros objeto
%.o: src/%.c include/%.h $($@)
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

# Limpeza dos ficheiros criados
clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/magnaeats

# Testes
test:
	$(TEST)

test_error_no_args:
	$(RUN)

test_error_2_args:
	$(TEST) config.txt

valgrind:
	valgrind --leak-check=yes $(TEST)

valgrind_full:
	valgrind --leak-check=full $(TEST)
