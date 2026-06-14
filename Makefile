COMPILER = CC
CFLAGS = -O3 -march=native
TARGET = bin/jmake
SOURCE = src/jmake.c
INSTALLDIR = /usr/local/bin

all:
	CC $(CFLAGS) -lcjson -o $(TARGET) $(SOURCE)

install:
	install -m 755 $(TARGET) $(INSTALLDIR)

clean:
	rm $(TARGET)
