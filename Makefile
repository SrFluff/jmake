COMPILER=clang
CFLAGS=-O3 -march=native

default:
	$(COMPILER) $(CFLAGS) -o jmake main.c -lcjson

install:
	cp jmake /usr/local/bin/
