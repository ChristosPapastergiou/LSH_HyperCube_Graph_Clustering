INCLUDE = ../../include
MODULES = ../../modules

CC = gcc

DEPS = graph_search.h $(INCLUDE)/list.h $(INCLUDE)/graph.h $(INCLUDE)/vector.h $(INCLUDE)/common.h $(INCLUDE)/assign.h $(INCLUDE)/random.h $(INCLUDE)/search.h $(INCLUDE)/compare.h $(INCLUDE)/metrics.h $(INCLUDE)/macqueen.h $(INCLUDE)/hashtable.h $(INCLUDE)/hypercube.h 
OBJS = graph_search.o $(MODULES)/list.o $(MODULES)/graph.o $(MODULES)/vector.o $(MODULES)/common.o $(MODULES)/assign.o $(MODULES)/random.o $(MODULES)/search.o $(MODULES)/compare.o $(MODULES)/metrics.o $(MODULES)/macqueen.o $(MODULES)/hashtable.o $(MODULES)/hypercube.o

EXEC = graph_search

# ARGS = -d ../../data/compressed_input.dat -q ../../data/compressed_query.dat -k 60 -E 40 -R 8 -N 10 -l 30 -m 1 -o ../../data/graph_gnn_compressed_output.csv
ARGS = -d ../../data/compressed_input.dat -q ../../data/compressed_query.dat -k 40 -E 20 -R 3 -N 10 -l 248 -m 2 -o ../../data/graph_gg_compressed_output.csv
# ARGS = -d ../../data/compressed_input.dat -q ../../data/compressed_query.dat -o ../../data/graph_gnn_compressed_output.csv

# ARGS = -d ../../data/input.dat -q ../../data/query.dat -k 60 -E 40 -R 8 -N 10 -l 30 -m 1 -o ../../data/graph_gnn_output.csv
# ARGS = -d ../../data/input.dat -q ../../data/query.dat -k 40 -E 20 -R 3 -N 10 -l 248 -m 2 -o ../../data/graph_gg_output.csv
# ARGS = -d ../../data/input.dat -q ../../data/query.dat -o ../../data/graph_gnn_output.csv

CFLAGS = -Wall -Werror -g -lm -I$(INCLUDE)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(CFLAGS)

graph_search.o: graph_search.c $(DEPS)
	$(CC) -c graph_search.c $(CFLAGS)

list.o: $(MODULES)/list.c $(DEPS)
	$(CC) -c $(MODULES)/list.c $(CFLAGS)

graph.o: $(MODULES)/graph.c $(DEPS)
	$(CC) -c $(MODULES)/graph.c $(CFLAGS)

vector.o: $(MODULES)/vector.c $(DEPS)
	$(CC) -c $(MODULES)/vector.c $(CFLAGS)

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