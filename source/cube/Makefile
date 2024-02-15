INCLUDE = ../../include
MODULES = ../../modules

CC = gcc

DEPS = cube.h $(INCLUDE)/list.h $(INCLUDE)/graph.h $(INCLUDE)/common.h $(INCLUDE)/assign.h $(INCLUDE)/random.h $(INCLUDE)/search.h $(INCLUDE)/compare.h $(INCLUDE)/metrics.h $(INCLUDE)/macqueen.h $(INCLUDE)/hashtable.h $(INCLUDE)/hypercube.h 
OBJS = cube.o $(MODULES)/list.o $(MODULES)/graph.o $(MODULES)/common.o $(MODULES)/assign.o $(MODULES)/random.o $(MODULES)/search.o $(MODULES)/compare.o $(MODULES)/metrics.o $(MODULES)/macqueen.o $(MODULES)/hashtable.o $(MODULES)/hypercube.o 

EXEC = cube

ARGS = -d ../../data/compressed_input.dat -q ../../data/compressed_query.dat -k 12 -M 1000 -probes 80 -o ../../data/cube_compressed_output.csv -N 10 -R 5000
# ARGS = -d ../../data/compressed_input.dat -q ../../data/compressed_query.dat -o ../../cube_compressed_output.csv

# ARGS = -d ../../data/input.dat -q ../../data/query.dat -k 12 -M 1000 -probes 80 -o ../../data/cube_output.csv -N 10 -R 5000
# ARGS = -d ../../data/input.dat -q ../../data/query.dat -o ../../cube_output.csv

CFLAGS = -Wall -Werror -g -lm -I$(INCLUDE)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(CFLAGS)

cube.o: cube.c $(DEPS)
	$(CC) -c cube.c $(CFLAGS)

list.o: $(MODULES)/list.c $(DEPS)
	$(CC) -c $(MODULES)/list.c $(CFLAGS)

graph.o: $(MODULES)/graph.c $(DEPS)
	$(CC) -c $(MODULES)/graph.c $(CFLAGS)

common.o: $(MODULES)/common.c $(DEPS)
	$(CC) -c $(MODULES)/common.c $(CFLAGS)

assign.o: $(MODULES)/assign.c $(DEPS)
	$(CC) -c $(MODULES)/assign.c $(CFLAGS)

random.o: $(MODULES)/random.c $(DEPS)
	$(CC) -c $(MODULES)/random.c $(CFLAGS)

search.o: $(MODULES)/search.c $(DEPS)
	$(CC) -c $(MODULES)/search.c $(CFLAGS)

compare.o: $(MODULES)/compare.c $(DEPS)
	$(CC) -c $(MODULES)/compare.c $(CFLAGS)

metrics.o: $(MODULES)/metrics.c $(DEPS)
	$(CC) -c $(MODULES)/metrics.c $(CFLAGS)

macqueen.o: $(MODULES)/macqueen.c $(DEPS)
	$(CC) -c $(MODULES)/macqueen.c $(CFLAGS)

hashtable.o: $(MODULES)/hashtable.c $(DEPS)
	$(CC) -c $(MODULES)/hashtable.c $(CFLAGS)

hypercube.o: $(MODULES)/hypercube.c $(DEPS)
	$(CC) -c $(MODULES)/hypercube.c -I$(INCLUDE)

run: $(EXEC)
	./$(EXEC) $(ARGS)

clean:
	rm -f $(OBJS) $(EXEC)

time: $(EXEC)
	time ./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(EXEC) $(ARGS)