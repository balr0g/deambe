CC = gcc
OBJS = deambe.o
LIBS = -lsndfile -lmbe

all: deambe

build: all

deambe.o: deambe.c dstar_const.h readambefile.h
	$(CC) $(CFLAGS) $(INCLUDES) -c deambe.c -o deambe.o


deambe: $(OBJS)
	$(CC) $(CFLAGS) -o deambe $(OBJS) $(LIBS) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f deambe