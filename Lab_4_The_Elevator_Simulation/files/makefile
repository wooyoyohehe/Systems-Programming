#EXECUTABLES = elevator_null elevator_part_1 reorder double-check
EXECUTABLES = elevator_null elevator_part_1 elevator_part_2 reorder double-check

CC = gcc
LIBS = libfdr.a
CFLAGS = -O2 -g -Wall

LIBFDROBJS = dllist.o fields.o jval.o jrb.o

all: $(EXECUTABLES)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c

double-check: double-check.o 
	$(CC) $(CFLAGS) -o double-check double-check.o $(LIBS) -lpthread -lm

reorder: reorder.o 
	$(CC) $(CFLAGS) -o reorder reorder.o $(LIBS) -lpthread -lm

elevator_null: elevator_skeleton.o elevator_null.o finesleep.o libfdr.a
	$(CC) $(CFLAGS) -o elevator_null elevator_skeleton.o elevator_null.o finesleep.o $(LIBS) -lpthread -lm

elevator_part_1: elevator_skeleton.o elevator_part_1.o finesleep.o libfdr.a
	$(CC) $(CFLAGS) -o elevator_part_1 elevator_skeleton.o elevator_part_1.o finesleep.o $(LIBS) -lpthread -lm

elevator_part_2: elevator_skeleton.o elevator_part_2.o finesleep.o libfdr.a
	$(CC) $(CFLAGS) -o elevator_part_2 elevator_skeleton.o elevator_part_2.o finesleep.o $(LIBS) -lpthread -lm

elevator_skeleton.o: elevator.h names.h
elevator.o: elevator.h

libfdr.a: $(LIBFDROBJS)
	ar ru libfdr.a $(LIBFDROBJS)
	ranlib libfdr.a 

clean:
	rm -f core *.o $(EXECUTABLES) *~ libfdr.a

