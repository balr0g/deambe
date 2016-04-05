CC = gcc
CXX = g++
DEAMBELIBS = -lsndfile -lmbe
IMBELIBS = -lsndfile -limbe_vocoder
CFLAGS += -Wall -g -I/opt/radio/include -I/usr/local/include
LDFLAGS=-L/opt/radio/lib -L/usr/local/lib

all: deambe deambe_b deambe_c imbe2wav wav2imbe

build: all

deambe.o: deambe.c dstar_const.h readambefile.h
	$(CC) $(CFLAGS) $(INCLUDES) -c deambe.c -o deambe.o

deambe_b.o: deambe_b.c dstar_const.h readambefile.h
	$(CC) $(CFLAGS) $(INCLUDES) -c deambe_b.c -o deambe_b.o

deambe_c.o: deambe_c.c dstar_const.h readambefile.h
	$(CC) $(CFLAGS) $(INCLUDES) -c deambe_c.c -o deambe_c.o

deambe: deambe.o
	$(CC) $(CFLAGS) deambe.o $(DEAMBELIBS) $(LDFLAGS) -o deambe

deambe_b: deambe_b.o
	$(CC) $(CFLAGS) deambe_b.o $(DEAMBELIBS) $(LDFLAGS) -o deambe_b

deambe_c: deambe_c.o
	$(CC) $(CFLAGS) deambe_c.o $(DEAMBELIBS) $(LDFLAGS) -o deambe_c

imbe2wav.o: imbe2wav.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c imbe2wav.cpp -o imbe2wav.o

imbe2wav: imbe2wav.o
	$(CXX) $(CFLAGS) imbe2wav.o $(IMBELIBS) $(LDFLAGS) -o imbe2wav

wav2imbe.o: wav2imbe.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c wav2imbe.cpp -o wav2imbe.o

wav2imbe: wav2imbe.o
	$(CXX) $(CFLAGS) wav2imbe.o $(IMBELIBS) $(LDFLAGS) -o wav2imbe

clean:
	rm -f *.o
	rm -f deambe imbe2wav wav2imbe
