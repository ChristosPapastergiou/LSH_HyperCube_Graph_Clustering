INCLUDE = include

CC = gcc

CFLAGS = -Wall -Werror -g -lm -I$(INCLUDE)

SUBDIRS = source/lsh source/cube source/cluster source/graph_search

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
