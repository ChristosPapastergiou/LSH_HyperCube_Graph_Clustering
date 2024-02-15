INCLUDE = ../../include
MODULES = ../../modules

CC = gcc

DEPS = cluster.h $(INCLUDE)/list.h $(INCLUDE)/graph.h $(INCLUDE)/kmeans.h $(INCLUDE)/common.h $(INCLUDE)/assign.h $(INCLUDE)/random.h $(INCLUDE)/search.h $(INCLUDE)/compare.h $(INCLUDE)/metrics.h $(INCLUDE)/macqueen.h $(INCLUDE)/hashtable.h $(INCLUDE)/hypercube.h
OBJS = cluster.o $(MODULES)/list.o $(MODULES)/graph.o $(MODULES)/kmeans.o $(MODULES)/common.o $(MODULES)/assign.o $(MODULES)/random.o $(MODULES)/search.o $(MODULES)/compare.o $(MODULES)/metrics.o $(MODULES)/macqueen.o $(MODULES)/hashtable.o $(MODULES)/hypercube.o

EXEC = cluster

ARGS = -i ../../data/compressed_input.dat -c ../../data/cluster.conf -o ../../data/cluster_classic_compressed_output.csv -complete -m Classic 
# ARGS = -i ../../data/compressed_input.dat -c ../../data/cluster.conf -o ../../data/cluster_cube_compressed_output.csv -complete -m Hypercube 
# ARGS = -i ../../data/compressed_input.dat -c ../../data/cluster.conf -o ../../data/cluster_lsh_compressed_output.csv -complete -m LSH 

# ARGS = -i ../../data/input.dat -c ../../data/cluster.conf -o ../../data/cluster_classic_output.csv -complete -m Classic 
# ARGS = -i ../../data/input.dat -c ../../data/cluster.conf -o ../../data/cluster_cube_output.csv -complete -m Hypercube 
# ARGS = -i ../../data/input.dat -c ../../data/cluster.conf -o ../../data/cluster_lsh_output.csv -complete -m LSH 

CFLAGS = -Wall -Werror -g -lm -I$(INCLUDE)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(CFLAGS)

cluster.o: cluster.c $(DEPS)
	$(CC) -c cluster.c -I$(INCLUDE)

list.o: $(MODULES)/list.c $(DEPS)
	$(CC) -c $(MODULES)/list.c $(CFLAGS)

graph.o: $(MODULES)/graph.c $(DEPS)
	$(CC) -c $(MODULES)/graph.c $(CFLAGS)

kmeans.o: $(MODULES)/kmeans.c $(DEPS)
	$(CC) -c $(MODULES)/kmeans.c $(CFLAGS)

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