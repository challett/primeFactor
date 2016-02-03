CFLAGS=-g -O2 -Wall -lgmp
CC = mpicc

PROGRAM_NAME= factor
OBJS = main.o 

$(PROGRAM_NAME): $(OBJS)
	$(CC) $(CFLAGS) -o  $@ $? 

clean:
	rm  *.o $(PROGRAM_NAME)
