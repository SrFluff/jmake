COMPILER = CC
CFLAGS = -O3 -march=native
TARGET = bin/jmake
SOURCE = src/jmake.c
INSTALLDIR = /usr/local/bin

all:
	$(COMPILER) $(CFLAGS) -o $(TARGET) $(SOURCE) -lcjson

install:
	install -m 755 $(TARGET) $(INSTALLDIR)

clean:
	rm $(TARGET)
	rm src/buildinfo.h
