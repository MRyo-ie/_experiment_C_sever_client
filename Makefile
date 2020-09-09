PROGRAM = server
OBJS = utils/file_rw.o  utils/http_parser.o  utils/http_builder.o
CC = cc
# CFLAGS = -O2

$(PROGRAM): $(PROGRAM).o $(OBJS)
	$(CC) -o $(PROGRAM).out $(PROGRAM).o $(OBJS) -lm

# $(OBJS) $(PROGRAM).o: server.h

all: clean $(PROGRAM)

clean:
	rm -f *.o utils/*.o

