COMPILER=clang
CFLAGS=-O3 -march=native

default:
	$(COMPILER) $(CFLAGS) -o jmake main.c -lcjson

install:
	cp jmake /usr/local/bin/

test:
	$(COMPILER) -fsyntax-only -lcjson main.c > /dev/null 2>&1
